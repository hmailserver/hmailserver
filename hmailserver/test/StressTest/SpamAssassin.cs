// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;
using hMailServer;
using NUnit.Framework;
using System.Threading;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;


namespace StressTest
{
   [TestFixture]
   public class SpamAssassin : TestFixtureBase
   {
      private int _threadedMessageCount;
      private Account _account;

      [SetUp]
      public new void SetUp()
      {
         _threadedMessageCount = 0;

         hMailServer.AntiSpam antiSpam = _application.Settings.AntiSpam;

         antiSpam.SpamAssassinEnabled = true;
         antiSpam.SpamAssassinHost = "127.0.0.1";
         antiSpam.SpamAssassinPort = 783;

         CustomAsserts.AssertSpamAssassinIsRunning();

         _account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
      }

      [Test]
      public void SingleThread()
      {
         for (int i = 0; i < 15; i++)
         {
             SmtpClientSimulator.StaticSend("test@test.com", "test@test.com", "test", "test");
         }

         Pop3ClientSimulator.AssertMessageCount(_account.Address, "test", 15);

         for (int i = 0; i < 15; i++)
         {
            string content = Pop3ClientSimulator.AssertGetFirstMessageText(_account.Address, "test");

            Assert.IsTrue(content.Contains("X-Spam-Status"), content);
         }

      }

      [Test]
      public void MultiThread()
      {
         int threadCount = 5;
         _threadedMessageCount = 100;
         int totalMessageCount = threadCount * _threadedMessageCount;

         List<Thread> threads = new List<Thread>();
         for (int thread = 0; thread < threadCount; thread++)
         {
            Thread t = new Thread(new ThreadStart(SendMessageThread));

            threads.Add(t);

            t.Start();
         }

         foreach (Thread t in threads)
         {
            t.Join();
         }

         Pop3ClientSimulator.AssertMessageCount(_account.Address, "test", totalMessageCount);

         for (int i = 0; i < totalMessageCount; i++)
         {
            string content = Pop3ClientSimulator.AssertGetFirstMessageText(_account.Address, "test");

            Assert.IsTrue(content.Contains("X-Spam-Status"), content);
         }
      }

      [Test]
      public void TestVaryingMessageSizes()
      {
         const int messageSizeIncrease = 100;
         for (int i = 0; i < 100000; i += messageSizeIncrease)
         {
            var messageBuilder = new StringBuilder();
            messageBuilder.Append('A', messageSizeIncrease-2);
            messageBuilder.AppendLine();

            var message = messageBuilder.ToString();

            SmtpClientSimulator.StaticSendRaw("test@test.com", "test@test.com", message);

            var defaultLog = LogHandler.ReadCurrentDefaultLog();
            
            StringAssert.DoesNotContain("There was a communication error with SpamAssassin.", defaultLog);
            
            LogHandler.DeleteCurrentDefaultLog();

            Pop3ClientSimulator.AssertGetFirstMessageText("test@test.com", "test");

            Console.WriteLine("{0} / 100000", i);
         }
      }

      private void SendMessageThread()
      {
         for (int message = 0; message < _threadedMessageCount; message++)
         {
             SmtpClientSimulator.StaticSend("test@test.com", "test@test.com", "test", "test");
         }
      }

  }

}
