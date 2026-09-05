using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.SMTP
{
   [TestFixture]
   public class XOriginalRcptHeaderTests : TestFixtureBase
   {
      [SetUp]
      public void VerifyFeatureEnabled()
      {
         var hMailServerFile = IniFileLocator.GetIniFileName();

         var isEnabled =
            (from line in File.ReadAllLines(hMailServerFile)
               where line.StartsWith("AddXOriginalRcptTo") && line.EndsWith("1")
               select line).Any();

         if (!isEnabled) Assert.Inconclusive("Setting AddXOriginalRcptTo is not set to 1.");
      }

      private const string Password = "test";
      private static readonly Random random = new Random();

      [Test]
      public void ShouldAddSingleLocalAddress()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", Password);

         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test", "Test");

         var text = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, Password);
         StringAssert.Contains("X-Original-Rcpt-To: test@example.test" + Environment.NewLine, text);
      }

      [Test]
      public void ShouldAddMultipleLocalAddresses()
      {
         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test1@example.test", Password);
         var account2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test2@example.test", Password);

         var recipients = new List<string>
         {
            account1.Address,
            account2.Address
         };

         SmtpClientSimulator.StaticSend(account1.Address, recipients, "Test", "Test");

         var text = Pop3ClientSimulator.AssertGetFirstMessageText(account1.Address, Password);
         StringAssert.Contains("X-Original-Rcpt-To: test1@example.test,test2@example.test" + Environment.NewLine, text);
      }

      [Test]
      public void ShouldAddOriginalAliasAddress()
      {
         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test1@example.test", Password);
         var account2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test2@example.test", Password);
         var account3 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test3@example.test", Password);

         var alias = SingletonProvider<TestSetup>.Instance.AddAlias(_domain, "my-alias@example.test", account3.Address);

         var recipients = new List<string>
         {
            account1.Address,
            account2.Address,
            alias.Name
         };

         SmtpClientSimulator.StaticSend(account1.Address, recipients, "Test", "Test");

         var text = Pop3ClientSimulator.AssertGetFirstMessageText(account1.Address, Password);
         StringAssert.Contains(
            "X-Original-Rcpt-To: my-alias@example.test,test1@example.test," + Environment.NewLine +
            "\ttest2@example.test" + Environment.NewLine,
            text);
      }

      [Test]
      public void ShouldAddDistributionListAddress()
      {
         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test1@example.test", Password);
         var account2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test2@example.test", Password);

         var distributionList = SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain,
            "my-list@example.test", new List<string>
            {
               account1.Address,
               account2.Address
            });


         SmtpClientSimulator.StaticSend(account1.Address, distributionList.Address, "Test", "Test");

         var text = Pop3ClientSimulator.AssertGetFirstMessageText(account1.Address, Password);
         StringAssert.Contains("X-Original-Rcpt-To: my-list@example.test" + Environment.NewLine, text);
      }

      [Test]
      public void ShouldFoldLongLines()
      {
         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain,
            "a-very-very-very-very-very-very-very-very-very-long-address-1@example.test", Password);
         var account2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain,
            "a-very-very-very-very-very-very-very-very-very-long-address-2@example.test", Password);
         var account3 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain,
            "z-very-very-very-very-very-very-very-very-very-long-address-3@example.test", Password);
         var account4 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "short-1@example.test", Password);
         var account5 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "short-2@example.test", Password);
         var account6 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "short-3@example.test", Password);


         var recipients = new List<string>
         {
            account1.Address,
            account2.Address,
            account4.Address,
            account5.Address,
            account6.Address,
            account3.Address
         };

         SmtpClientSimulator.StaticSend(account1.Address, recipients, "Test", "Test");

         var text = Pop3ClientSimulator.AssertGetFirstMessageText(account1.Address, Password);

         var expected =
            "X-Original-Rcpt-To: a-very-very-very-very-very-very-very-very-very-long-address-1@example.test," +
            Environment.NewLine +
            "\ta-very-very-very-very-very-very-very-very-very-long-address-2@example.test," + Environment.NewLine +
            "\tshort-1@example.test,short-2@example.test,short-3@example.test," + Environment.NewLine +
            "\tz-very-very-very-very-very-very-very-very-very-long-address-3@example.test" + Environment.NewLine;

         StringAssert.Contains(expected, text);
      }

      [Test]
      public void ShouldAddAllAddresses_StressTest()
      {
         var accountAddresses = new List<string>();

         for (var i = 0; i < 25; i++)
         {
            var address = "test" + i + "-" + GenerateRandomNumberString() + "@example.test";
            var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, address, Password);
            accountAddresses.Add(address);

            SmtpClientSimulator.StaticSend(account.Address, accountAddresses, "Test", "Test");

            var text = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, Password);

            var headerStart = text.IndexOf("X-Original-Rcpt-To");
            var headerEnd = text.IndexOf("From: ");
            var header = text.Substring(headerStart, headerEnd - headerStart);

            foreach (var recipient in accountAddresses) StringAssert.Contains(recipient, header);
         }
      }

      public static string GenerateRandomNumberString()
      {
         var length = random.Next(0, 41); // length between 0 and 40 inclusive
         var digits = new char[length];

         for (var i = 0; i < length; i++) digits[i] = (char)('0' + random.Next(0, 10));

         return new string(digits);
      }
   }
}