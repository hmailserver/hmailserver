// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using NUnit.Framework;
using System.IO;
using System.Threading;
using RegressionTests.Shared;


namespace StressTest
{
   [TestFixture]
   public class SpamAssassin : TestFixtureBase
   {
      private int _threadedMessageCount;

      [SetUp]
      public new void SetUp()
      {
         _threadedMessageCount = 0;

         hMailServer.AntiSpam antiSpam = _application.Settings.AntiSpam;

         antiSpam.SpamAssassinEnabled = true;
         antiSpam.SpamAssassinHost = "localhost";
         antiSpam.SpamAssassinHost = "192.168.127.128";

         TestSetup.AssertSpamAssassinIsRunning();
      }

      [Test]
      [Ignore]
      public void SingleThread()
      {
         hMailServer.Account account =
                    SingletonProvider<TestSetup>.Instance.AddAccount(_domain.Accounts, "test@test.com", "test");

         for (int i = 0; i < 15; i++)
         {
             SMTPClientSimulator.StaticSend("test@test.com", "test@test.com", "test", "test");
         }

         POP3Simulator.AssertMessageCount(account.Address, "test", 15);

         for (int i = 0; i < 15; i++)
         {
            string content = POP3Simulator.AssertGetFirstMessageText(account.Address, "test");

            Assert.IsTrue(content.Contains("X-Spam-Status"), content);
         }

      }

      [Test]
      [Ignore]
      public void MultiThread()
      {
         hMailServer.Account account =
                    SingletonProvider<TestSetup>.Instance.AddAccount(_domain.Accounts, "test@test.com", "test");

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

         POP3Simulator.AssertMessageCount(account.Address, "test", totalMessageCount);

         for (int i = 0; i < totalMessageCount; i++)
         {
            string content = POP3Simulator.AssertGetFirstMessageText(account.Address, "test");

            Assert.IsTrue(content.Contains("X-Spam-Status"), content);
         }
      }

      private void SendMessageThread()
      {
         for (int message = 0; message < _threadedMessageCount; message++)
         {
             SMTPClientSimulator.StaticSend("test@test.com", "test@test.com", "test", "test");
         }
      }

  }

}
