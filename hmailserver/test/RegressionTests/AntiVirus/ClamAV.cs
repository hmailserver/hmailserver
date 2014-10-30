using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.AntiVirus
{
   [TestFixture]
   public class ClamAV : TestFixtureBase
   {
      #region Setup/Teardown

      [SetUp]
      public new void SetUp()
      {
         CustomAsserts.AssertClamDRunning();

         _antiVirus = _application.Settings.AntiVirus;
      }

      #endregion

      private hMailServer.AntiVirus _antiVirus;

      [Test]
      public void TestIncorrectPort()
      {
         _antiVirus.ClamAVEnabled = true;
         _antiVirus.ClamAVPort = 110;

         Account account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         SmtpClientSimulator.StaticSend(account1.Address, account1.Address, "Mail 1", "DummyBody");
         Pop3ClientSimulator.AssertMessageCount(account1.Address, "test", 1);
         CustomAsserts.AssertReportedError("Protocol error. Unexpected response: +OK");
      }

      [Test]
      public void TestNoVirus()
      {
         _antiVirus.ClamAVEnabled = true;

         Account account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         SmtpClientSimulator.StaticSend(account1.Address, account1.Address, "Mail 1", "Mail 1");
         Pop3ClientSimulator.AssertMessageCount(account1.Address, "test", 1);
      }

      [Test]
      public void TestNotEnabled()
      {
         LogHandler.DeleteCurrentDefaultLog();
         Account account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         SmtpClientSimulator.StaticSend(account1.Address, account1.Address, "Mail 1", "Mail 1");
         Pop3ClientSimulator.AssertMessageCount(account1.Address, "test", 1);
         string defaultLog = LogHandler.ReadCurrentDefaultLog();
         Assert.IsFalse(defaultLog.Contains("Connecting to ClamAV"));
      }

      [Test]
      public void TestUnusedPort()
      {
         _antiVirus.ClamAVEnabled = true;
         _antiVirus.ClamAVPort = 54391;

         Account account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         SmtpClientSimulator.StaticSend(account1.Address, account1.Address, "Mail 1", "DummyBody");
         Pop3ClientSimulator.AssertMessageCount(account1.Address, "test", 1);
         CustomAsserts.AssertReportedError("Unable to connect to ClamAV server at localhost:54391.");
      }

      [Test]
      public void TestWithVirus()
      {
         _antiVirus.ClamAVEnabled = true;
         LogHandler.DeleteCurrentDefaultLog();

         Account account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         string firstPart = @"X5O!P%@AP[4\PZX54(P^)7CC)7}";
         string secondPart = @"$EICAR-STANDARD-ANTIVIRUS-TEST-FILE!$H+H*";
         SmtpClientSimulator.StaticSend(account1.Address, account1.Address, "Mail 1",
                                                      firstPart + secondPart);

         CustomAsserts.AssertRecipientsInDeliveryQueue(0);
         Pop3ClientSimulator.AssertMessageCount(account1.Address, "test", 0);

         string defaultLog = LogHandler.ReadCurrentDefaultLog();
         Assert.IsTrue(defaultLog.Contains("Connecting to ClamAV"));
         Assert.IsTrue(defaultLog.Contains("Message deleted (contained virus Eicar-Test-Signature)"));
      }
   }
}