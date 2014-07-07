// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using NUnit.Framework;
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
         TestSetup.DeleteCurrentDefaultLog();
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

         var sim = new POP3Simulator();
         CustomAssert.IsTrue(sim.ConnectAndLogon(account.Address, "test"));
         sim.Disconnect();

         // confirm that we can retrieve welcome message.
         CustomAssert.IsTrue(sim.GetWelcomeMessage().StartsWith("+OK"));

         string errorMessage;
         // fail to log on 3 times.
         for (int i = 0; i < 5; i++)
         {
            CustomAssert.IsFalse(sim.ConnectAndLogon(account.Address, "testA", out errorMessage));
            sim.Disconnect();
         }

         CustomAssert.IsTrue(sim.GetWelcomeMessage().StartsWith("+OK"));

         string logText = TestSetup.ReadCurrentDefaultLog();
         CustomAssert.IsFalse(logText.Contains("Blocked either by IP range or by connection limit."), logText);
      }

      [Test]
      public void TestIMAPLogonFailure()
      {
         _settings.AutoBanOnLogonFailure = true;
         _settings.MaxInvalidLogonAttempts = 4;
         _settings.MaxInvalidLogonAttemptsWithin = 5;
         _settings.AutoBanMinutes = 3;

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         var sim = new IMAPSimulator();
         CustomAssert.IsTrue(sim.ConnectAndLogon(account.Address, "test"));
         sim.Disconnect();

         // confirm that we can retrieve welcome message.
         CustomAssert.IsTrue(sim.GetWelcomeMessage().StartsWith("* OK"));

         // fail to log on 3 times.
         for (int i = 0; i < 4; i++)
         {
            string errorMessage;

            CustomAssert.IsFalse(sim.ConnectAndLogon(account.Address, "testA", out errorMessage));
            sim.Disconnect();

            if (i == 3)
            {
               CustomAssert.IsTrue(errorMessage.Contains("Too many invalid logon attempts."));
            }
         }

         CustomAssert.IsTrue(sim.GetWelcomeMessage().Length == 0);

         string logText = TestSetup.ReadCurrentDefaultLog();
         CustomAssert.IsTrue(logText.Contains("Blocked either by IP range or by connection limit."), logText);
      }

      [Test]
      public void TestIPRangeName()
      {
         _settings.AutoBanOnLogonFailure = true;
         _settings.MaxInvalidLogonAttempts = 3;
         _settings.MaxInvalidLogonAttemptsWithin = 5;
         _settings.AutoBanMinutes = 3;

         CustomAssert.AreEqual(2, _settings.SecurityRanges.Count);

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         var sim = new POP3Simulator();
         CustomAssert.IsTrue(sim.ConnectAndLogon(account.Address, "test"));
         sim.Disconnect();

         // confirm that we can retrieve welcome message.
         CustomAssert.IsTrue(sim.GetWelcomeMessage().StartsWith("+OK"));

         string errorMessage;
         // fail to log on 3 times.
         for (int i = 0; i < 3; i++)
         {
            CustomAssert.IsFalse(sim.ConnectAndLogon(account.Address, "testA", out errorMessage));
            sim.Disconnect();

            if (i == 2)
            {
               CustomAssert.IsTrue(errorMessage.Contains("Too many invalid logon attempts."));
            }
         }

         CustomAssert.IsTrue(sim.GetWelcomeMessage().Length == 0);

         CustomAssert.AreEqual(3, _settings.SecurityRanges.Count);
         CustomAssert.AreEqual("Auto-ban: test@test.com", _settings.SecurityRanges[2].Name);
      }

      [Test]
      public void TestPOP3LogonFailure()
      {
         _settings.AutoBanOnLogonFailure = true;
         _settings.MaxInvalidLogonAttempts = 3;
         _settings.MaxInvalidLogonAttemptsWithin = 5;
         _settings.AutoBanMinutes = 3;

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "te'st@test.com", "test");

         var sim = new POP3Simulator(TestSetup.GetLocalIpAddress(), false, 110);
         CustomAssert.IsTrue(sim.ConnectAndLogon(account.Address, "test"));
         sim.Disconnect();

         // confirm that we can retrieve welcome message.
         CustomAssert.IsTrue(sim.GetWelcomeMessage().StartsWith("+OK"));

         string errorMessage;
         // fail to log on 3 times.
         for (int i = 0; i < 3; i++)
         {
            CustomAssert.IsFalse(sim.ConnectAndLogon(account.Address, "testA", out errorMessage));
            sim.Disconnect();

            if (i == 2)
            {
               CustomAssert.IsTrue(errorMessage.Contains("Too many invalid logon attempts."));
            }
         }

         CustomAssert.IsTrue(sim.GetWelcomeMessage().Length == 0);

         string logText = TestSetup.ReadCurrentDefaultLog();
         CustomAssert.IsTrue(logText.Contains("Blocked either by IP range or by connection limit."), logText);
      }

      [Test]
      public void TestSMTPLogonFailure()
      {
         _settings.AutoBanOnLogonFailure = true;
         _settings.MaxInvalidLogonAttempts = 2;
         _settings.MaxInvalidLogonAttemptsWithin = 5;
         _settings.AutoBanMinutes = 3;

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         var sim = new SMTPClientSimulator();

         //test@test.com / test
         string errorMessage;
         CustomAssert.IsTrue(sim.ConnectAndLogon(25, "dGVzdEB0ZXN0LmNvbQ==", "dGVzdA==", out errorMessage));
         sim.Disconnect();

         // confirm that we can retrieve welcome message.
         CustomAssert.IsTrue(sim.GetWelcomeMessage().StartsWith("220"));

         // fail to log on 3 times.
         for (int i = 0; i < 2; i++)
         {
            CustomAssert.IsFalse(sim.ConnectAndLogon(25, "dGVzdEB0ZXN0LmNvbQ==", "Vaffe==", out errorMessage));
            sim.Disconnect();

            if (i == 2)
            {
               CustomAssert.IsTrue(errorMessage.Contains("Too many invalid logon attempts."));
            }
         }

         CustomAssert.IsTrue(sim.GetWelcomeMessage().Length == 0);

         string logText = TestSetup.ReadCurrentDefaultLog();
         CustomAssert.IsTrue(logText.Contains("Blocked either by IP range or by connection limit."), logText);
      }
   }
}