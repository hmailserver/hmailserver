using NUnit.Framework;
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
         TestSetup.AssertClamDRunning();

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
         Assert.IsTrue(SMTPClientSimulator.StaticSend(account1.Address, account1.Address, "Mail 1", "DummyBody"));
         POP3Simulator.AssertMessageCount(account1.Address, "test", 1);
         TestSetup.AssertReportedError("Protocol error. Unexpected response: +OK");
      }

      [Test]
      public void TestNoVirus()
      {
         _antiVirus.ClamAVEnabled = true;

         Account account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         Assert.IsTrue(SMTPClientSimulator.StaticSend(account1.Address, account1.Address, "Mail 1", "Mail 1"));
         POP3Simulator.AssertMessageCount(account1.Address, "test", 1);
      }

      [Test]
      public void TestNotEnabled()
      {
         TestSetup.DeleteCurrentDefaultLog();
         Account account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         Assert.IsTrue(SMTPClientSimulator.StaticSend(account1.Address, account1.Address, "Mail 1", "Mail 1"));
         POP3Simulator.AssertMessageCount(account1.Address, "test", 1);
         string defaultLog = TestSetup.ReadCurrentDefaultLog();
         Assert.IsFalse(defaultLog.Contains("Connecting to ClamAV"));
      }

      [Test]
      public void TestUnusedPort()
      {
         _antiVirus.ClamAVEnabled = true;
         _antiVirus.ClamAVPort = 54391;

         Account account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         Assert.IsTrue(SMTPClientSimulator.StaticSend(account1.Address, account1.Address, "Mail 1", "DummyBody"));
         POP3Simulator.AssertMessageCount(account1.Address, "test", 1);
         TestSetup.AssertReportedError("Unable to connect to ClamAV server at localhost:54391.");
      }

      [Test]
      public void TestWithVirus()
      {
         _antiVirus.ClamAVEnabled = true;
         TestSetup.DeleteCurrentDefaultLog();

         Account account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         string firstPart = @"X5O!P%@AP[4\PZX54(P^)7CC)7}";
         string secondPart = @"$EICAR-STANDARD-ANTIVIRUS-TEST-FILE!$H+H*";
         Assert.IsTrue(SMTPClientSimulator.StaticSend(account1.Address, account1.Address, "Mail 1",
                                                      firstPart + secondPart));

         TestSetup.AssertRecipientsInDeliveryQueue(0);
         POP3Simulator.AssertMessageCount(account1.Address, "test", 0);

         string defaultLog = TestSetup.ReadCurrentDefaultLog();
         Assert.IsTrue(defaultLog.Contains("Connecting to ClamAV"));
         Assert.IsTrue(defaultLog.Contains("Message deleted (contained virus Eicar-Test-Signature)"));
      }
   }
}