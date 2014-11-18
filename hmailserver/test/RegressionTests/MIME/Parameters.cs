// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using NUnit.Framework;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.MIME
{
   [TestFixture]
   public class Parameters : TestFixtureBase
   {
      [Test]
      [Description("Issue 238, If charset parameter contains double quotes, the string isn't parsed properly.")]
      public void TestFetchCharsetInQuotesWithSpaceAfter()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         SmtpClientSimulator.StaticSendRaw(account.Address, account.Address,
                                           "From: test@test.com\r\n" +
                                           "Content-Type: text/plain; charset =\"iso-8859-1\" \r\n" +
                                           "\r\n" +
                                           "Test\r\n");

         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 1);

         var sim = new ImapClientSimulator(account.Address, "test", "Inbox");
         string result = sim.Fetch("1 BODYSTRUCTURE");
         sim.Disconnect();

         Assert.IsTrue(result.Contains("(\"CHARSET\" \"iso-8859-1\")"), result);
      }

      [Test]
      [Description("Issue 238, If charset parameter contains double quotes, the string isn't parsed properly.")]
      public void TestFetchCharsetInQuotesWithSpaceBefore()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         SmtpClientSimulator.StaticSendRaw(account.Address, account.Address,
                                           "From: test@test.com\r\n" +
                                           "Content-Type: text/plain; charset = \"iso-8859-1\"\r\n" +
                                           "\r\n" +
                                           "Test\r\n");

         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 1);

         var sim = new ImapClientSimulator(account.Address, "test", "Inbox");
         string result = sim.Fetch("1 BODYSTRUCTURE");
         sim.Disconnect();

         Assert.IsTrue(result.Contains("(\"CHARSET\" \"iso-8859-1\")"), result);
      }

      [Test]
      [Description("Issue 238, If charset parameter contains double quotes, the string isn't parsed properly.")]
      public void TestFetchCharsetInQuotesWithoutQuotesWithSpace()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         SmtpClientSimulator.StaticSendRaw(account.Address, account.Address,
                                           "From: test@test.com\r\n" +
                                           "Content-Type: text/plain; charset = iso-8859-1 \r\n" +
                                           "\r\n" +
                                           "Test\r\n");

         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 1);

         var sim = new ImapClientSimulator(account.Address, "test", "Inbox");
         string result = sim.Fetch("1 BODYSTRUCTURE");
         sim.Disconnect();

         Assert.IsTrue(result.Contains("(\"CHARSET\" \"iso-8859-1\")"), result);
      }

      [Test]
      [Description("Issue 238, If charset parameter contains double quotes, the string isn't parsed properly.")]
      public void TestFetchCharsetInQuotesWithoutQuotesWithoutSpace()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         SmtpClientSimulator.StaticSendRaw(account.Address, account.Address,
                                           "From: test@test.com\r\n" +
                                           "Content-Type: text/plain; charset=iso-8859-1 \r\n" +
                                           "\r\n" +
                                           "Test\r\n");

         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 1);

         var sim = new ImapClientSimulator(account.Address, "test", "Inbox");
         string result = sim.Fetch("1 BODYSTRUCTURE");
         sim.Disconnect();

         Assert.IsTrue(result.Contains("(\"CHARSET\" \"iso-8859-1\")"), result);
      }

      [Test]
      [Description("Issue 238, If charset parameter contains double quotes, the string isn't parsed properly.")]
      public void TestFetchCharsetInQuotesWithoutSpace()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         SmtpClientSimulator.StaticSendRaw(account.Address, account.Address,
                                           "From: test@test.com\r\n" +
                                           "Content-Type: text/plain; charset =\"iso-8859-1\"\r\n" +
                                           "\r\n" +
                                           "Test\r\n");

         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 1);

         var sim = new ImapClientSimulator(account.Address, "test", "Inbox");
         string result = sim.Fetch("1 BODYSTRUCTURE");
         sim.Disconnect();

         Assert.IsTrue(result.Contains("(\"CHARSET\" \"iso-8859-1\")"), result);
      }
   }
}