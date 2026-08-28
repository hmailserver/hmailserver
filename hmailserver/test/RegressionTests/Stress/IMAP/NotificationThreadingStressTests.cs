using System;
using System.Collections.Generic;
using System.Threading;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;
using RegressionTests.SSL;

namespace RegressionTests.Stress.IMAP
{
   [TestFixture]
   [Explicit("Stress test - long running, run manually.")]
   [Category("Stress")]
   [Description(
      "Heap-corruption races around IMAP change notifications. MUST be run against a build with full page " +
      "heap enabled (gflags /p /enable hMailServer.exe /full); without it a corrupted free list is only " +
      "detected thousands of allocations later. Pair with RepeatRunsUntilFailure.ps1 - being races, a " +
      "single green run proves little.")]
   internal class NotificationThreadingStressTests : TestFixtureBase
   {
      private const int ImapPort = 143;
      private const int ImapsPort = 14301;

      private const int IdleSessionCount = 8;
      private const int ChurnSessionCount = 4;
      private const int DeliveryThreadCount = 2;
      private const int NotifierThreadCount = 4;

      // Each STORE notifies every other subscriber; keep the range small so the loop stays tight.
      private const int StoredMessageCount = 5;

      private static readonly TimeSpan RaceDuration = TimeSpan.FromSeconds(60);

      // The IDLE sessions re-select periodically, so a delivering thread can be inside
      // SendChangeNotification_ while the connection's own thread resets current_folder_.
      private static readonly TimeSpan IdleHoldTime = TimeSpan.FromMilliseconds(25);

      private const int PreloadedMessageCount = 200;

      // Large enough that UID FETCH 1:* (BODY[]) keeps an async_read in flight on the ssl::stream.
      private const int LargeMessageSize = 32768;
      private const int LargePreloadedMessageCount = 30;
      private const int FetchedMessageCount = 4;

      private static readonly TimeSpan ResponseTimeout = TimeSpan.FromSeconds(30);

      private class RaceState
      {
         public int Idles;
         public int Churns;
         public int Deliveries;
         public int Fetches;
         public int Stores;
      }

      [OneTimeSetUp]
      public new void TestFixtureSetUp()
      {
         SslSetup.SetupSSLPorts(_application);

         Thread.Sleep(1000);
      }

      [Test]
      [Description(
         "NotificationServer::SendNotification calls OnNotification synchronously on the delivering SMTP " +
         "thread, which then copies the target connection's current_folder_ shared_ptr and reads its " +
         "recent_messages_ set while that connection's own IO thread resets and reassigns both.")]
      public void ConcurrentIdleAndFolderChurnDuringDeliveryDoesNotCorruptHeap()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "notify@example.test", "test");

         PreloadMessages(account.Address, PreloadedMessageCount, "ABCD");

         var failures = new List<string>();
         var state = new RaceState();
         var deadline = DateTime.UtcNow + RaceDuration;

         var threads = new List<Thread>();

         for (var i = 0; i < IdleSessionCount; i++)
            threads.Add(new Thread(() => RepeatedlyIdle(account.Address, false, ImapPort, deadline, failures, state)));

         for (var i = 0; i < ChurnSessionCount; i++)
            threads.Add(new Thread(() => RepeatedlyChurnSelectedFolder(account.Address, false, ImapPort, deadline, failures, state)));

         for (var i = 0; i < DeliveryThreadCount; i++)
            threads.Add(new Thread(() => RepeatedlyDeliver(account.Address, deadline, failures, state)));

         for (var i = 0; i < NotifierThreadCount; i++)
            threads.Add(new Thread(() => RepeatedlyStoreFlags(account.Address, deadline, failures, state)));

         RunAll(threads);

         TestContext.WriteLine(
            "Notification race: {0} IDLE cycles, {1} EXAMINE/CLOSE cycles, {2} STOREs, {3} deliveries over {4}.",
            state.Idles, state.Churns, state.Stores, state.Deliveries, RaceDuration);

         CustomAsserts.AssertNoReportedError();

         Assert.IsEmpty(failures, string.Join(Environment.NewLine, failures));
      }

      [Test]
      [Description(
         "IOOperationQueue::Front lets a write start while a read is ongoing, and EnqueueWrite runs on the " +
         "caller's thread. Over IMAPS that means the delivering thread drives an async_write on the same " +
         "ssl::stream that the connection's IO thread is reading from.")]
      public void ConcurrentNotificationWriteAndSslReadDoesNotCorruptHeap()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "notify-ssl@example.test", "test");

         PreloadMessages(account.Address, LargePreloadedMessageCount, new string('x', LargeMessageSize));

         var failures = new List<string>();
         var state = new RaceState();
         var deadline = DateTime.UtcNow + RaceDuration;

         var threads = new List<Thread>();

         for (var i = 0; i < IdleSessionCount; i++)
            threads.Add(new Thread(() => RepeatedlyIdle(account.Address, true, ImapsPort, deadline, failures, state)));

         for (var i = 0; i < ChurnSessionCount; i++)
            threads.Add(new Thread(() => RepeatedlyChurnSelectedFolder(account.Address, true, ImapsPort, deadline, failures, state)));

         for (var i = 0; i < DeliveryThreadCount; i++)
            threads.Add(new Thread(() => RepeatedlyDeliver(account.Address, deadline, failures, state)));

         // The notifiers stay on plaintext: what matters is that they are not the SSL connection's
         // own IO thread when they drive its async_write.
         for (var i = 0; i < NotifierThreadCount; i++)
            threads.Add(new Thread(() => RepeatedlyStoreFlags(account.Address, deadline, failures, state)));

         RunAll(threads);

         TestContext.WriteLine(
            "SSL notification race: {0} IDLE cycles, {1} EXAMINE/CLOSE cycles, {2} FETCHes, {3} STOREs, {4} deliveries over {5}.",
            state.Idles, state.Churns, state.Fetches, state.Stores, state.Deliveries, RaceDuration);

         CustomAsserts.AssertNoReportedError();

         Assert.IsEmpty(failures, string.Join(Environment.NewLine, failures));
      }

      private static void RunAll(List<Thread> threads)
      {
         foreach (var thread in threads)
            thread.Start();

         foreach (var thread in threads)
            thread.Join();
      }

      private static ImapClientSimulator CreateSimulator(bool useSsl, int port)
      {
         return useSsl ? new ImapClientSimulator(true, port) : new ImapClientSimulator();
      }

      private static void RepeatedlyIdle(string address, bool useSsl, int port, DateTime deadline,
         List<string> failures, RaceState state)
      {
         try
         {
            var simulator = CreateSimulator(useSsl, port);

            if (!simulator.ConnectAndLogon(address, "test"))
            {
               Record(failures, "Idler could not log on.");
               return;
            }

            while (DateTime.UtcNow < deadline)
            {
               // EXAMINE, not SELECT: a read-only folder skips RemoveRecentFlags on CLOSE, which
               // holds the collection mutex across a folder-wide UPDATE. current_folder_ is set
               // and reset either way, so the window is the same but reached far more often.
               simulator.SendRaw("A15 EXAMINE INBOX\r\n");
               ReceiveUntil(simulator, "A15 OK", ResponseTimeout);

               // Over SSL, keep a large response streaming so an async_read is in flight when a
               // notifying thread starts an async_write on the same ssl::stream.
               if (useSsl)
               {
                  simulator.SendRaw(string.Format("A20 UID FETCH 1:{0} (BODY[])\r\n", FetchedMessageCount));
                  ReceiveUntil(simulator, "A20 OK", ResponseTimeout);

                  Interlocked.Increment(ref state.Fetches);
               }

               // Not StartIdle/EndIdle: those assume one response per read, but unsolicited
               // EXISTS/RECENT from the delivering threads arrive interleaved.
               simulator.SendRaw("A25 IDLE\r\n");
               ReceiveUntil(simulator, "+ idling", ResponseTimeout);

               Thread.Sleep(IdleHoldTime);

               simulator.SendRaw("DONE\r\n");
               ReceiveUntil(simulator, "A25 OK", ResponseTimeout);

               // Dropping the folder while a notifying thread may still be inside
               // SendChangeNotification_ is the window under test.
               simulator.SendRaw("A14 CLOSE\r\n");
               ReceiveUntil(simulator, "A14 OK", ResponseTimeout);

               Interlocked.Increment(ref state.Idles);
            }

            simulator.Disconnect();
         }
         catch (Exception ex)
         {
            Record(failures, "Idler aborted: " + ex.Message);
         }
      }

      private static void RepeatedlyChurnSelectedFolder(string address, bool useSsl, int port, DateTime deadline,
         List<string> failures, RaceState state)
      {
         try
         {
            var simulator = CreateSimulator(useSsl, port);

            if (!simulator.ConnectAndLogon(address, "test"))
            {
               Record(failures, "Churner could not log on.");
               return;
            }

            while (DateTime.UtcNow < deadline)
            {
               simulator.SendRaw("A15 EXAMINE INBOX\r\n");
               ReceiveUntil(simulator, "A15 OK", ResponseTimeout);

               // CLOSE unsubscribes and resets current_folder_ while notifications for the same
               // folder are being dispatched on the notifying threads.
               simulator.SendRaw("A14 CLOSE\r\n");
               ReceiveUntil(simulator, "A14 OK", ResponseTimeout);

               Interlocked.Increment(ref state.Churns);
            }

            simulator.Disconnect();
         }
         catch (Exception ex)
         {
            Record(failures, "Churner aborted: " + ex.Message);
         }
      }

      private static void RepeatedlyStoreFlags(string address, DateTime deadline, List<string> failures, RaceState state)
      {
         try
         {
            // One long-lived connection, so the notification rate is not gated by TCP setup the
            // way SMTP delivery is. Every STORE notifies the other subscribers on its own thread.
            var simulator = new ImapClientSimulator();

            if (!simulator.ConnectAndLogon(address, "test"))
            {
               Record(failures, "Notifier could not log on.");
               return;
            }

            simulator.SendRaw("A15 SELECT INBOX\r\n");
            ReceiveUntil(simulator, "A15 OK", ResponseTimeout);

            var setSeen = true;

            while (DateTime.UtcNow < deadline)
            {
               simulator.SendRaw(string.Format("A30 STORE 1:{0} {1}FLAGS (\\Seen)\r\n",
                  StoredMessageCount, setSeen ? "+" : "-"));
               ReceiveUntil(simulator, "A30 OK", ResponseTimeout);

               setSeen = !setSeen;

               Interlocked.Increment(ref state.Stores);
            }

            simulator.Disconnect();
         }
         catch (Exception ex)
         {
            Record(failures, "Notifier aborted: " + ex.Message);
         }
      }

      private static void RepeatedlyDeliver(string address, DateTime deadline, List<string> failures, RaceState state)
      {
         try
         {
            while (DateTime.UtcNow < deadline)
            {
               // Each delivery calls SendNotification on this thread, reaching into every
               // connection subscribed to the account's INBOX.
               var smtp = new SmtpClientSimulator();
               smtp.Send("sender@example.test", address, "Notification race", "Body");

               Interlocked.Increment(ref state.Deliveries);
            }
         }
         catch (Exception ex)
         {
            Record(failures, "Delivery aborted: " + ex.Message);
         }
      }

      private static void PreloadMessages(string address, int count, string body)
      {
         var simulator = new ImapClientSimulator();
         simulator.Connect();
         simulator.LogonWithLiteral(address, "test");

         var command = string.Format("A01 APPEND INBOX {{{0}}}", body.Length);

         for (var i = 0; i < count; i++)
            simulator.SendSingleCommandWithLiteral(command, body);

         Assert.AreEqual(count, simulator.GetMessageCount("INBOX"));

         simulator.Disconnect();
      }

      private static string ReceiveUntil(ImapClientSimulator simulator, string text, TimeSpan timeout)
      {
         var deadline = DateTime.UtcNow + timeout;
         var result = string.Empty;

         while (DateTime.UtcNow < deadline)
         {
            result += simulator.Receive();

            if (result.Contains(text))
               return result;
         }

         throw new TimeoutException("Timeout while waiting for: " + text);
      }

      private static void Record(List<string> failures, string message)
      {
         lock (failures)
            failures.Add(message);
      }
   }
}
