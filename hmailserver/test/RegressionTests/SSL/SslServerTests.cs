// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.IO;
using System.Threading;
using NUnit.Framework;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.SSL
{
   [TestFixture]
   public class Basic : TestFixtureBase
   {
      [TestFixtureSetUp]
      public new void TestFixtureSetUp()
      {
         SslSetup.SetupSSLPorts(_application);

         Thread.Sleep(1000);
      }

      
      [Test]
      [Category("SSL")]
      [Description("Confirm that the TCP/IP log contains information on when a SSL handshake fails")]
      public void SetupSSLPort()
      {
         _application.Settings.Logging.Enabled = true;
         _application.Settings.Logging.LogTCPIP = true;
         _application.Settings.Logging.EnableLiveLogging(true);

         var cs = new TcpConnection();
         if (!cs.Connect(25001))
            CustomAssert.Fail("Could not connect to SSL server.");

         cs.Disconnect();

         for (int i = 0; i <= 40; i++)
         {
            CustomAssert.IsTrue(i != 40);

            string liveLog = _application.Settings.Logging.LiveLog;
            if (liveLog.Contains("SSL handshake with client failed."))
               break;

            Thread.Sleep(250);
         }

         _application.Settings.Logging.EnableLiveLogging(false);
      }

      [Test]
      public void TestIMAPServer()
      {
         TestSetup.DeleteCurrentDefaultLog();

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "imap-ssl@test.com", "test");

         for (int i = 0; i < 30; i++)
         {
            try
            {
               var imapSim = new IMAPSimulator(true, 14301);
               imapSim.ConnectAndLogon(account.Address, "test");
               CustomAssert.IsTrue(imapSim.SelectFolder("Inbox"), "SelectInbox");
               imapSim.CreateFolder("Test");
               CustomAssert.IsTrue(imapSim.SelectFolder("Test"), "SelectTest");
               CustomAssert.IsTrue(imapSim.Logout(), "Logout");

               imapSim.Disconnect();
               break;
            }
            catch (Exception)
            {
               if (i == 29)
                  throw;
            }
         }
      }

      [Test]
      public void TestPOP3Server()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "pop3-ssl@test.com", "test");

         var smtpSim = new SMTPClientSimulator();
         CustomAssert.IsTrue(smtpSim.Send("test@test.com", account.Address, "Test", "MyBody"));

         for (int i = 0; i < 10; i++)
         {
            try
            {
               POP3Simulator.AssertMessageCount(account.Address, "test", 1);
               var pop3Sim = new POP3Simulator(true, 11001);
               string text = pop3Sim.GetFirstMessageText(account.Address, "test");

               CustomAssert.IsTrue(text.Contains("MyBody"));

               break;
            }
            catch (AssertionException)
            {
               throw;
            }
            catch (Exception)
            {
               if (i == 9)
                  throw;
            }
         }
      }

      [Test]
      public void TestSMTPServer()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "smtp-ssl@test.com", "test");

         int i = 0;
         for (i = 0; i < 10; i++)
         {
            try
            {
               var smtpSim = new SMTPClientSimulator(true, 25001);
               CustomAssert.IsTrue(smtpSim.Send("test@test.com", account.Address, "Test", "MyBody"));

               break;
            }
            catch (Exception)
            {
               if (i == 9)
                  throw;
            }
         }

         POP3Simulator.AssertMessageCount(account.Address, "test", i + 1);
         var pop3Sim = new POP3Simulator(false, 110);
         string text = pop3Sim.GetFirstMessageText(account.Address, "test");
         CustomAssert.IsTrue(text.Contains("MyBody"));
      }
   }
}