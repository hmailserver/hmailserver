using System;
using System.IO;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.Infrastructure
{
   [TestFixture]
   public class SessionLimitingTests : TestFixtureBase
   {
      private TimeSpan _readTimeout = TimeSpan.FromSeconds(5);

      [Test]
      public void TestLimitImapSessionCount()
      {
         _settings.MaxIMAPConnections = 2;

         using (var conn1 = new TcpConnection())
         using (var conn2 = new TcpConnection())
         using (var conn3 = new TcpConnection())
         {
            Assert.IsTrue(conn1.Connect(143));
            conn1.Receive();
            Assert.IsTrue(conn2.Connect(143));
            conn2.Receive();
            Assert.IsTrue(conn3.Connect(143));
            Assert.IsEmpty(conn3.Receive());
         }

         var log2 = LogHandler.ReadCurrentDefaultLog();
         Assert.IsTrue(log2.Contains("Blocked either by IP range or by connection limit."));
      }

      [Test]
      public void TestLimitSMTPSessionCount()
      {
         _settings.MaxSMTPConnections = 2;

         using (var conn1 = new TcpConnection())
         using (var conn2 = new TcpConnection())
         using (var conn3 = new TcpConnection())
         {
            Assert.IsTrue(conn1.Connect(25));
            conn1.Receive();
            Assert.IsTrue(conn2.Connect(25));
            conn2.Receive();
            Assert.IsTrue(conn3.Connect(25));
            Assert.IsEmpty(conn3.Receive());
         }

         var log2 = LogHandler.ReadCurrentDefaultLog();
         Assert.IsTrue(log2.Contains("Blocked either by IP range or by connection limit."));
      }

      [Test]
      public void TestLimitPOP3SessionCount()
      {
         _settings.MaxPOP3Connections = 2;

         using (var conn1 = new TcpConnection())
         using (var conn2 = new TcpConnection())
         using (var conn3 = new TcpConnection())
         {
            Assert.IsTrue(conn1.Connect(110));
            conn1.Receive();
            Assert.IsTrue(conn2.Connect(110));
            conn2.Receive();
            Assert.IsTrue(conn3.Connect(110));
            Assert.IsEmpty(conn3.Receive());
         }
      }


      [Test]
      public void TestConnectingToSmtpServerIncreasesSmtpSessionCount()
      {
         var app = SingletonProvider<TestSetup>.Instance.GetApp();
         var status = app.Status;
         int countBefore = status.get_SessionCount(eSessionType.eSTSMTP);

         using (var conn1 = new TcpConnection())
         {
            Assert.IsTrue(conn1.Connect(25));

            CustomAsserts.AssertSessionCount(eSessionType.eSTSMTP,countBefore+1);
         }

      }

      [Test]
      public void TestConnectingToImapServerIncreasesImapSessionCount()
      {
         var app = SingletonProvider<TestSetup>.Instance.GetApp();
         var status = app.Status;
         int countBefore = status.get_SessionCount(eSessionType.eSTIMAP);

         using (var conn1 = new TcpConnection())
         {
            Assert.IsTrue(conn1.Connect(143));
            CustomAsserts.AssertSessionCount(eSessionType.eSTIMAP, countBefore + 1);
         }

      }

      [Test]
      public void TestConnectingToPop3ServerIncreasesPop3SessionCount()
      {
         var app = SingletonProvider<TestSetup>.Instance.GetApp();
         var status = app.Status;
         int countBefore = status.get_SessionCount(eSessionType.eSTPOP3);

         using (var conn1 = new TcpConnection())
         {
            Assert.IsTrue(conn1.Connect(110));
            CustomAsserts.AssertSessionCount(eSessionType.eSTPOP3, countBefore + 1);
         }

      }


      [Test]
      public void TestDisconnectingFromSmtpServerDecreasesIMAPPSmtpSessionCount()
      {
         var app = SingletonProvider<TestSetup>.Instance.GetApp();
         var status = app.Status;

         int countBefore = status.get_SessionCount(eSessionType.eSTSMTP);
         using (var conn1 = new TcpConnection())
         {
            Assert.IsTrue(conn1.Connect(25));
         }

         CustomAsserts.AssertSessionCount(eSessionType.eSTSMTP, countBefore);
      }

      [Test]
      public void TestDisconnectingFromImapServerDecreasesImapSessionCount()
      {
         var app = SingletonProvider<TestSetup>.Instance.GetApp();
         var status = app.Status;

         int countBefore = status.get_SessionCount(eSessionType.eSTIMAP);
         using (var conn1 = new TcpConnection())
         {
            Assert.IsTrue(conn1.Connect(143));
         }

         AssertMaxSessionCount(eSessionType.eSTPOP3, countBefore);
      }

      [Test]
      public void TestDisconnectingFromPop3ServerDecreasesPop3SessionCount()
      {
         var app = SingletonProvider<TestSetup>.Instance.GetApp();
         var status = app.Status;

         int countBefore = status.get_SessionCount(eSessionType.eSTPOP3);
         using (var conn1 = new TcpConnection())
         {
            Assert.IsTrue(conn1.Connect(110));
         }

         AssertMaxSessionCount(eSessionType.eSTPOP3, countBefore);
      }

      [Test]
      public void TestDisconnectingClientWithOnClientConnectEventShouldDecreaseConnectionCount()
      {
         Application app = SingletonProvider<TestSetup>.Instance.GetApp();

         var status = app.Status;
         int countBefore = status.get_SessionCount(eSessionType.eSTSMTP);

         Scripting scripting = app.Settings.Scripting;

         string script = "Sub OnClientConnect(oClient) " + Environment.NewLine +
                         "   Result.Value = 1" + Environment.NewLine +
                         "End Sub" + Environment.NewLine + Environment.NewLine;

         File.WriteAllText(scripting.CurrentScriptFile, script);
         scripting.Enabled = true;
         scripting.Reload();

         var socket = new TcpConnection();
         Assert.IsTrue(socket.Connect(110));
         Assert.IsEmpty(socket.Receive());

         AssertMaxSessionCount(eSessionType.eSTSMTP, countBefore);
      }


      public static void AssertMaxSessionCount(eSessionType sessionType, int maxExpectedCount)
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();

         RetryHelper.TryAction(TimeSpan.FromSeconds(10), () =>
         {
            int count = application.Status.get_SessionCount(sessionType);

            Assert.GreaterOrEqual(maxExpectedCount, count);
         });

      }
   }
}
