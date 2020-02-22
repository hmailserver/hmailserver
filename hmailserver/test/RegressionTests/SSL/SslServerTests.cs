// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.IO;
using System.Threading;
using NUnit.Framework;
using RegressionTests.Infrastructure;
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
            Assert.Fail("Could not connect to SSL server.");

         cs.Disconnect();

         for (int i = 0; i <= 40; i++)
         {
            Assert.IsTrue(i != 40);

            string liveLog = _application.Settings.Logging.LiveLog;
            if (liveLog.Contains("TCPConnection - TLS/SSL handshake failed."))
               break;

            Thread.Sleep(250);
         }

         _application.Settings.Logging.EnableLiveLogging(false);
      }

      [Test]
      public void TestIMAPServer()
      {
         LogHandler.DeleteCurrentDefaultLog();

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "imap-ssl@test.com", "test");

         for (int i = 0; i < 30; i++)
         {
            try
            {
               var imapSim = new ImapClientSimulator(true, 14301);
               imapSim.ConnectAndLogon(account.Address, "test");
               Assert.IsTrue(imapSim.SelectFolder("Inbox"), "SelectInbox");
               imapSim.CreateFolder("Test");
               Assert.IsTrue(imapSim.SelectFolder("Test"), "SelectTest");
               Assert.IsTrue(imapSim.Logout(), "Logout");

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

         var smtpSim = new SmtpClientSimulator();
         smtpSim.Send("test@test.com", account.Address, "Test", "MyBody");

         for (int i = 0; i < 10; i++)
         {
            try
            {
               Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);
               var pop3Sim = new Pop3ClientSimulator(true, 11001);
               string text = pop3Sim.GetFirstMessageText(account.Address, "test");

               Assert.IsTrue(text.Contains("MyBody"));

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
               var smtpSim = new SmtpClientSimulator(true, 25001);
               smtpSim.Send("test@test.com", account.Address, "Test", "MyBody");

               break;
            }
            catch (Exception)
            {
               if (i == 9)
                  throw;
            }
         }

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", i + 1);
         var pop3Sim = new Pop3ClientSimulator(false, 110);
         string text = pop3Sim.GetFirstMessageText(account.Address, "test");
         Assert.IsTrue(text.Contains("MyBody"));
      }

      [Test]
      public void ConnectionCountShouldDecreaseWhenClientDisconnects()
      {
         var countBefore = _application.Status.get_SessionCount(eSessionType.eSTSMTP);

         using (var tcpConn = new TcpConnection(false))
         {
            tcpConn.Connect(25001);
            
            // Since there may be other connections lingering, we just check the increased count if
            // it was zero previous to this test. Otherwise we might end up with false positives.
            if (countBefore == 0)
            {
               RetryHelper.TryAction(TimeSpan.FromSeconds(10), () =>
               {
                  var countWhileConnected = _application.Status.get_SessionCount(eSessionType.eSTSMTP);

                  Assert.AreEqual(1, countWhileConnected);
               });

            }
         }

         RetryHelper.TryAction(TimeSpan.FromSeconds(10), () =>
         {
            var countAfter = _application.Status.get_SessionCount(eSessionType.eSTSMTP);

            Assert.GreaterOrEqual(countBefore, countAfter);
         });
      }
   }
}