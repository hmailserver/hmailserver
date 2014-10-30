// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.Security
{
   [TestFixture]
   public class AutoBan : TestFixtureBase
   {
      #region Setup/Teardown

      [SetUp]
      public new void SetUp()
      {
         _settings.ClearLogonFailureList();
         LogHandler.DeleteCurrentDefaultLog();
      }

      #endregion

      [Test]
      public void TestDisabled()
      {
         _settings.AutoBanOnLogonFailure = false;
         _settings.MaxInvalidLogonAttempts = 3;
         _settings.MaxInvalidLogonAttemptsWithin = 5;
         _settings.AutoBanMinutes = 3;

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         var sim = new Pop3ClientSimulator();
         Assert.IsTrue(sim.ConnectAndLogon(account.Address, "test"));
         sim.Disconnect();

         // confirm that we can retrieve welcome message.
         Assert.IsTrue(sim.GetWelcomeMessage().StartsWith("+OK"));

         string errorMessage;
         // fail to log on 3 times.
         for (int i = 0; i < 5; i++)
         {
            Assert.IsFalse(sim.ConnectAndLogon(account.Address, "testA", out errorMessage));
            sim.Disconnect();
         }

         Assert.IsTrue(sim.GetWelcomeMessage().StartsWith("+OK"));

         string logText = LogHandler.ReadCurrentDefaultLog();
         Assert.IsFalse(logText.Contains("Blocked either by IP range or by connection limit."), logText);
      }

      [Test]
      public void TestIMAPLogonFailure()
      {
         _settings.AutoBanOnLogonFailure = true;
         _settings.MaxInvalidLogonAttempts = 4;
         _settings.MaxInvalidLogonAttemptsWithin = 5;
         _settings.AutoBanMinutes = 3;

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         var sim = new ImapClientSimulator();
         Assert.IsTrue(sim.ConnectAndLogon(account.Address, "test"));
         sim.Disconnect();

         // confirm that we can retrieve welcome message.
         Assert.IsTrue(sim.GetWelcomeMessage().StartsWith("* OK"));

         // fail to log on 3 times.
         for (int i = 0; i < 4; i++)
         {
            string errorMessage;

            Assert.IsFalse(sim.ConnectAndLogon(account.Address, "testA", out errorMessage));
            sim.Disconnect();

            if (i == 3)
            {
               Assert.IsTrue(errorMessage.Contains("Too many invalid logon attempts."));
            }
         }

         Assert.IsTrue(sim.GetWelcomeMessage().Length == 0);

         string logText = LogHandler.ReadCurrentDefaultLog();
         Assert.IsTrue(logText.Contains("Blocked either by IP range or by connection limit."), logText);
      }

      [Test]
      public void TestIPRangeName()
      {
         _settings.AutoBanOnLogonFailure = true;
         _settings.MaxInvalidLogonAttempts = 3;
         _settings.MaxInvalidLogonAttemptsWithin = 5;
         _settings.AutoBanMinutes = 3;

         Assert.AreEqual(2, _settings.SecurityRanges.Count);

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         var sim = new Pop3ClientSimulator();
         Assert.IsTrue(sim.ConnectAndLogon(account.Address, "test"));
         sim.Disconnect();

         // confirm that we can retrieve welcome message.
         Assert.IsTrue(sim.GetWelcomeMessage().StartsWith("+OK"));

         string errorMessage;
         // fail to log on 3 times.
         for (int i = 0; i < 3; i++)
         {
            Assert.IsFalse(sim.ConnectAndLogon(account.Address, "testA", out errorMessage));
            sim.Disconnect();

            if (i == 2)
            {
               Assert.IsTrue(errorMessage.Contains("Too many invalid logon attempts."));
            }
         }

         Assert.IsTrue(sim.GetWelcomeMessage().Length == 0);

         Assert.AreEqual(3, _settings.SecurityRanges.Count);
         Assert.AreEqual("Auto-ban: test@test.com", _settings.SecurityRanges[2].Name);
      }

      [Test]
      public void TestPOP3LogonFailure()
      {
         _settings.AutoBanOnLogonFailure = true;
         _settings.MaxInvalidLogonAttempts = 3;
         _settings.MaxInvalidLogonAttemptsWithin = 5;
         _settings.AutoBanMinutes = 3;

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "te'st@test.com", "test");

         var sim = new Pop3ClientSimulator(TestSetup.GetLocalIpAddress(), false, 110);
         Assert.IsTrue(sim.ConnectAndLogon(account.Address, "test"));
         sim.Disconnect();

         // confirm that we can retrieve welcome message.
         Assert.IsTrue(sim.GetWelcomeMessage().StartsWith("+OK"));

         string errorMessage;
         // fail to log on 3 times.
         for (int i = 0; i < 3; i++)
         {
            Assert.IsFalse(sim.ConnectAndLogon(account.Address, "testA", out errorMessage));
            sim.Disconnect();

            if (i == 2)
            {
               Assert.IsTrue(errorMessage.Contains("Too many invalid logon attempts."));
            }
         }

         Assert.IsTrue(sim.GetWelcomeMessage().Length == 0);

         string logText = LogHandler.ReadCurrentDefaultLog();
         Assert.IsTrue(logText.Contains("Blocked either by IP range or by connection limit."), logText);
      }

      [Test]
      public void TestSMTPLogonFailure()
      {
         _settings.AutoBanOnLogonFailure = true;
         _settings.MaxInvalidLogonAttempts = 2;
         _settings.MaxInvalidLogonAttemptsWithin = 5;
         _settings.AutoBanMinutes = 3;

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         var sim = new SmtpClientSimulator();

         //test@test.com / test
         string errorMessage;
         sim.ConnectAndLogon("dGVzdEB0ZXN0LmNvbQ==", "dGVzdA==", out errorMessage);
         sim.Disconnect();

         // confirm that we can retrieve welcome message.
         Assert.IsTrue(sim.GetWelcomeMessage().StartsWith("220"));

         // fail to log on 3 times.
         for (int i = 0; i < 2; i++)
         {
            CustomAsserts.Throws<System.Exception>(() => sim.ConnectAndLogon("dGVzdEB0ZXN0LmNvbQ==", "Vaffe==", out errorMessage));
            sim.Disconnect();

            if (i == 2)
            {
               Assert.IsTrue(errorMessage.Contains("Too many invalid logon attempts."));
            }
         }

         Assert.IsTrue(sim.GetWelcomeMessage().Length == 0);

         string logText = LogHandler.ReadCurrentDefaultLog();
         Assert.IsTrue(logText.Contains("Blocked either by IP range or by connection limit."), logText);
      }
   }
}