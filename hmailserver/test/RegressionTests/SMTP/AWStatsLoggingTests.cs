using System;
using System.IO;
using System.Net;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;

namespace RegressionTests.SMTP
{
   [TestFixture]
   public class AWStatsLoggingTests : TestFixtureBase
   {
      private Logging _logging;

      [OneTimeSetUp]
      public void OneTimeSetUp()
      {
         Settings settings = SingletonProvider<TestSetup>.Instance.GetApp().Settings;
         _logging = settings.Logging;

         _logging.AWStatsEnabled = true;
         _logging.Enabled = true;
      }

      [SetUp]
      public new void SetUp()
      {
         if (File.Exists(_logging.CurrentAwstatsLog))
            File.Delete(_logging.CurrentAwstatsLog);
      }

      [Test]
      public void SuccessfulDeliveriesShouldBeLogged()
      {
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         IPAddress localAddress = TestSetup.GetLocalIpAddress();
         var smtpClientSimulator = new SmtpClientSimulator(false, 25, localAddress);

         // Delivery from external to local.
         smtpClientSimulator.Send("test@external.com", "test@test.com", "Mail 1", "Mail 1");
         Pop3ClientSimulator.AssertMessageCount("test@test.com", "test", 1);
         
         // Verify that the delivery is logged
         string contents = TestSetup.ReadExistingTextFile(_logging.CurrentAwstatsLog);
         CustomAsserts.AssertDeleteFile(_logging.CurrentAwstatsLog);
         string expectedString = string.Format("\ttest@external.com\ttest@test.com\t{0}\t127.0.0.1\tSMTP\t?\t250\t", localAddress);
         StringAssert.Contains(expectedString, contents);
         
         // Verify there's just 1 logged line
         Assert.AreEqual(1, contents.Split(new string[] { "\r\n" }, StringSplitOptions.RemoveEmptyEntries).Length);
      }

      [Test]
      public void FailedDeliveriesDueToAuthErrorShouldBeLogged()
      {
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         IPAddress localAddress = TestSetup.GetLocalIpAddress();
         var smtpClientSimulator = new SmtpClientSimulator(false, 25, localAddress);

         // Failed delivery from local to local.
         CustomAsserts.Throws<DeliveryFailedException>(() =>
            smtpClientSimulator.Send("test@test.com", "test@test.com", "Mail 1", "Mail 1"));

         // Verify that the failed delivery is logged
         string contents = TestSetup.ReadExistingTextFile(_logging.CurrentAwstatsLog);
         CustomAsserts.AssertDeleteFile(_logging.CurrentAwstatsLog);
         string expectedString = string.Format("\ttest@test.com\ttest@test.com\t{0}\t127.0.0.1\tSMTP\t?\t530\t", localAddress);
         StringAssert.Contains(expectedString, contents);

         // Verify there's just 1 logged line
         Assert.AreEqual(1,  contents.Split(new string[] {"\r\n"}, StringSplitOptions.RemoveEmptyEntries).Length);
      }

      [Test]
      public void FailedDeliveriesDueToGlobalRulesShouldBeLogged()
      {
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         CreateDeleteAllMailRule();

         // Failed delivery from local to local.
         SmtpClientSimulator.StaticSend("test@test.com", "test@test.com", "Mail 1", "Mail 1");

         // Verify that the failed delivery is logged
         string contents = TestSetup.ReadExistingTextFile(_logging.CurrentAwstatsLog);
         CustomAsserts.AssertDeleteFile(_logging.CurrentAwstatsLog);
         string expectedString = string.Format("\ttest@test.com\ttest@test.com\t127.0.0.1\t127.0.0.1\tSMTP\t?\t550\t");
         StringAssert.Contains(expectedString, contents);

         // Verify there's just 1 logged line
         Assert.AreEqual(1, contents.Split(new string[] { "\r\n" }, StringSplitOptions.RemoveEmptyEntries).Length);
      }

      private void CreateDeleteAllMailRule()
      {
         Rule rule = SingletonProvider<TestSetup>.Instance.GetApp().Rules.Add();
         rule.Name = "Global rule test";
         rule.Active = true;

         RuleCriteria ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = true;
         ruleCriteria.PredefinedField = eRulePredefinedField.eFTMessageSize;
         ruleCriteria.MatchType = eRuleMatchType.eMTGreaterThan;
         ruleCriteria.MatchValue = "0";
         ruleCriteria.Save();

         // Add action
         RuleAction ruleAction = rule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRADeleteEmail;
         ruleAction.Save();

         // Save the rule in the database
         rule.Save();
      }
   }
}

