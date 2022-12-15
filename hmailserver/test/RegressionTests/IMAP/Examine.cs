using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.IMAP
{
   [TestFixture]
   public class Examine : TestFixtureBase
   {
      [Test]
      [Description("Assert that it's not possible to change flags while in READONLY-mode")]
      public void TestChangeFlags()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "examine@test.com", "test");

         SmtpClientSimulator.StaticSend("test@test.com", account.Address, "Test", "test");
         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

         var simulator = new ImapClientSimulator();
         simulator.ConnectAndLogon(account.Address, "test");
         simulator.ExamineFolder("Inbox");
         Assert.IsFalse(simulator.SetFlagOnMessage(1, true, @"\Deleted"));
      }

      [Test]
      [Description(
         "Assert that the \\RECENT flag isn't automatically changed when accessing a folder in READONLY-mode")]
      public void TestChangeRecentFlag()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "examine@test.com", "test");

         SmtpClientSimulator.StaticSend("test@test.com", account.Address, "Test", "test");
         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

         var simulator = new ImapClientSimulator();
         simulator.ConnectAndLogon(account.Address, "test");
         string result = simulator.ExamineFolder("Inbox");
         Assert.IsTrue(result.Contains("* 1 RECENT"), result);
         simulator.Close();
         simulator.Disconnect();

         simulator = new ImapClientSimulator();
         simulator.ConnectAndLogon(account.Address, "test");
         Assert.IsTrue(simulator.SelectFolder("Inbox", out result));
         Assert.IsTrue(result.Contains("* 1 RECENT"), result);
         simulator.Close();
         simulator.Disconnect();

         simulator = new ImapClientSimulator();
         simulator.ConnectAndLogon(account.Address, "test");
         result = simulator.ExamineFolder("Inbox");
         Assert.IsTrue(result.Contains("* 0 RECENT"), result);
         simulator.Close();
         simulator.Disconnect();
      }

      [Test]
      [Description("Assert that the \\SEEN flag isn't automatically changed when accessing a message in READONLY-mode"
         )]
      public void TestChangeSeenFlag()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "examine@test.com", "test");

         SmtpClientSimulator.StaticSend("test@test.com", account.Address, "Test", "test");
         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

         var simulator = new ImapClientSimulator();
         simulator.ConnectAndLogon(account.Address, "test");
         simulator.ExamineFolder("Inbox");
         string flags = simulator.GetFlags(1);
         string body = simulator.Fetch("1 RFC822");
         string flagsAfter = simulator.GetFlags(1);
         simulator.Close();
         simulator.Disconnect();

         Assert.AreEqual(flags, flagsAfter);

         var secondSimulator = new ImapClientSimulator();
         secondSimulator.ConnectAndLogon(account.Address, "test");
         secondSimulator.SelectFolder("Inbox");
         string secondFlags = secondSimulator.GetFlags(1);
         string secondBody = secondSimulator.Fetch("1 RFC822");
         string secondFlagsAfter = secondSimulator.GetFlags(1);
         secondSimulator.Close();
         secondSimulator.Disconnect();

         Assert.AreNotEqual(secondFlags, secondFlagsAfter);
      }

      [Test]
      public void TestExamine()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "examine@test.com", "test");

         var simulator = new ImapClientSimulator();

         string sWelcomeMessage = simulator.Connect();
         simulator.Logon(account.Address, "test");
         Assert.IsTrue(simulator.CreateFolder("TestFolder"));
         string result = simulator.ExamineFolder("TestFolder");

         Assert.IsTrue(result.Contains("[PERMANENTFLAGS ()]"), result);
         Assert.IsTrue(result.Contains("[READ-ONLY]"), result);
      }

      [Test]
      [Description("Assert that it's not possible to EXPUNGE while in READONLY-mode")]
      public void TestExpunge()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "examine@test.com", "test");

         SmtpClientSimulator.StaticSend("test@test.com", account.Address, "Test", "test");
         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

         var simulator = new ImapClientSimulator();
         simulator.ConnectAndLogon(account.Address, "test");
         simulator.SelectFolder("Inbox");
         Assert.IsTrue(simulator.SetFlagOnMessage(1, true, @"\Deleted"));

         var secondSimulator = new ImapClientSimulator();
         secondSimulator.ConnectAndLogon(account.Address, "test");
         string result = secondSimulator.ExamineFolder("INBOX");
         Assert.IsTrue(result.Contains("1 EXISTS"), result);
         Assert.IsFalse(secondSimulator.Expunge());

         simulator.SelectFolder("INBOX");
         Assert.IsTrue(simulator.Expunge());

         simulator.Close();
         secondSimulator.Close();
      }
   }
}