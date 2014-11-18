// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using NUnit.Framework;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.IMAP
{
   [TestFixture]
   public class Sort : TestFixtureBase
   {
      #region Setup/Teardown

      [SetUp]
      public new void SetUp()
      {
         _application.Settings.IMAPSortEnabled = true;

         base.SetUp();
      }

      #endregion

      [Test]
      [Description("Issue 340, Incorrect date sorting order")]
      public void TestDateSortOrder()
      {
         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "imapsort@test.com", "test");
         var oSimulator = new ImapClientSimulator();

         string sWelcomeMessage = oSimulator.Connect();
         oSimulator.LogonWithLiteral("imapsort@test.com", "test");
         Assert.IsTrue(oSimulator.SelectFolder("Inbox"));

         string response =
            oSimulator.SendSingleCommandWithLiteral("A04 APPEND INBOX \"22-Feb-2008 22:00:00 +0200\" {37}",
                                                    "Date: Wed, 15 Dec 2010 13:00:00 +0000");
         Assert.IsTrue(response.Contains("* 1 EXISTS"), response);

         response = oSimulator.SendSingleCommandWithLiteral("A04 APPEND INBOX \"22-Feb-2008 21:00:00 +0200\" {37}",
                                                            "Date: Wed, 15 Dec 2010 14:00:00 +0000");
         Assert.IsTrue(response.Contains("* 2 EXISTS"), response);

         response = oSimulator.SendSingleCommandWithLiteral("A04 APPEND INBOX \"22-Feb-2008 20:00:00 +0200\" {37}",
                                                            "Date: Wed, 15 Dec 2010 12:00:00 +0000");
         Assert.IsTrue(response.Contains("* 3 EXISTS"), response);

         response = oSimulator.SendSingleCommandWithLiteral("A04 APPEND INBOX \"23-Feb-2008 01:30:23 +0200\" {37}",
                                                            "Date: Wed, 15 Dec 2010 11:00:00 +0000");
         Assert.IsTrue(response.Contains("* 4 EXISTS"), response);

         string sortDateResponse = oSimulator.SendSingleCommand("A10 SORT (DATE) US-ASCII ALL");

         Assert.IsTrue(sortDateResponse.Contains(" 4 3 1 2"));
         oSimulator.Disconnect();
      }

      [Test]
      [Description("Issue 340, Incorrect date sorting order")]
      public void TestDateSortOrderNonexistantDate()
      {
         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "imapsort@test.com", "test");
         var oSimulator = new ImapClientSimulator();

         string sWelcomeMessage = oSimulator.Connect();
         oSimulator.LogonWithLiteral("imapsort@test.com", "test");
         Assert.IsTrue(oSimulator.SelectFolder("Inbox"));

         string response = oSimulator.SendSingleCommandWithLiteral(
            "A04 APPEND INBOX \"22-Feb-2008 22:00:00 +0200\" {4}", "ABCD");
         Assert.IsTrue(response.Contains("* 1 EXISTS"), response);

         response = oSimulator.SendSingleCommandWithLiteral("A04 APPEND INBOX \"22-Feb-2008 21:00:00 +0200\" {4}",
                                                            "ABCD");
         Assert.IsTrue(response.Contains("* 2 EXISTS"), response);

         response = oSimulator.SendSingleCommandWithLiteral("A04 APPEND INBOX \"22-Feb-2008 20:00:00 +0200\" {4}",
                                                            "ABCD");
         Assert.IsTrue(response.Contains("* 3 EXISTS"), response);

         response = oSimulator.SendSingleCommandWithLiteral("A04 APPEND INBOX \"23-Feb-2008 01:30:23 +0200\" {4}",
                                                            "ABCD");
         Assert.IsTrue(response.Contains("* 4 EXISTS"), response);

         /*
          * RFC 5256 "2.2. Sent Date" chapter. If the sent date cannot be determined (a Date: header is missing or cannot be parsed), 
          * the INTERNALDATE for that message is used as the sent date.
          */

         string sortDateResponse = oSimulator.SendSingleCommand("A10 SORT (DATE) US-ASCII ALL");
         string sortArivalDateResponse = oSimulator.SendSingleCommand("A10 SORT (ARRIVAL) US-ASCII ALL");

         Assert.IsTrue(sortArivalDateResponse.Contains(" 3 2 1 4"));
         Assert.AreEqual(sortDateResponse, sortArivalDateResponse);
         oSimulator.Disconnect();
      }

      [Test]
      [Description("Issue 168 - IMAP: Search for message with specific UID fails. ")]
      public void TestSearchSpecficUID()
      {
         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "search@test.com", "test");

         // Send a message to this account.
         var smtpClientSimulator = new SmtpClientSimulator();
         for (int i = 0; i < 5; i++)
            smtpClientSimulator.Send("search@test.com", "search@test.com", "Test1", "This is a test of IMAP Search");

         ImapClientSimulator.AssertMessageCount("search@test.com", "test", "INBOX", 5);

         Messages messages = oAccount.IMAPFolders.get_ItemByName("Inbox").Messages;

         int second = messages[1].UID;
         int third = messages[2].UID;
         int fourth = messages[3].UID;


         var oSimulator = new ImapClientSimulator();
         oSimulator.Connect();
         oSimulator.Logon("search@test.com", "test");
         Assert.IsTrue(oSimulator.SelectFolder("INBOX"));

         string result = oSimulator.SendSingleCommand(string.Format("a01 SORT (REVERSE DATE) UTF-8 ALL UID {0},{1}", second, third));
         AssertSortResultContains(result, 2, 3);

         result = oSimulator.SendSingleCommand(string.Format("a01 SORT (DATE) UTF-8 ALL UID {0},{1}", third, second));
         AssertSortResultContains(result, 2, 3);

         result = oSimulator.SendSingleCommand(string.Format("a01 SORT (DATE) UTF-8 ALL UID {0}:{1}", second, fourth));
         AssertSortResultContains(result, 2, 3, 4);

         result = oSimulator.SendSingleCommand(string.Format("a01 SORT (DATE) UTF-8 ALL UID {0}:*", second));
         AssertSortResultContains(result, 2, 3, 4, 5);
      }

      private void AssertSortResultContains(string sortResponse, params int[] expected)
      {
         var response = ParseSortResult(sortResponse);

         Assert.AreEqual(expected.Length, response.Count, sortResponse);

         foreach (var expectedItem in expected)
            Assert.IsTrue(response.Contains(expectedItem), sortResponse);
      }

      private List<int> ParseSortResult(string resultText)
      {
         // Parses a string such as * SORT 2 3 4 5
         string messageListPart = resultText.Substring("* SORT ".Length);
         int end = messageListPart.IndexOf("\r\n", StringComparison.CurrentCultureIgnoreCase);
         messageListPart = messageListPart.Substring(0, end);

         var messages = messageListPart.Split(' ');

         var result = new List<int>();

         foreach (var message in messages)
            result.Add(int.Parse(message));

         return result;
      }


      [Test]
      public void TestSortDeletedOrAnswered()
      {
         Domain oDomain = _application.Domains[0];
         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "search@test.com", "test");

         // Send a message to this account.
         var smtpClientSimulator = new SmtpClientSimulator();
         smtpClientSimulator.Send("search@test.com", "search@test.com", "aa", "This is a test of IMAP Search");
         ImapClientSimulator.AssertMessageCount("search@test.com", "test", "INBOX", 1);
         smtpClientSimulator.Send("search@test.com", "search@test.com", "bb", "This is a test of IMAP Search");
         ImapClientSimulator.AssertMessageCount("search@test.com", "test", "INBOX", 2);

         var oSimulator = new ImapClientSimulator();
         string sWelcomeMessage = oSimulator.Connect();
         oSimulator.Logon("search@test.com", "test");
         Assert.IsTrue(oSimulator.SelectFolder("INBOX"));

         Assert.AreEqual("", oSimulator.Sort("(DATE) UTF-8 ALL OR ANSWERED DELETED"));
      }

      [Test]
      public void TestSortReverseArrival()
      {
         Domain oDomain = _application.Domains[0];
         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "search@test.com", "test");

         // Send a message to this account.
         var smtpClientSimulator = new SmtpClientSimulator();
         smtpClientSimulator.Send("search@test.com", "search@test.com", "Test1", "This is a test of IMAP Search");
         ImapClientSimulator.AssertMessageCount("search@test.com", "test", "INBOX", 1);

         // The two messages needs to be sent a second apart, so we actually need to pause a bit here.

         Thread.Sleep(1000);
         smtpClientSimulator.Send("search@test.com", "search@test.com", "Test2", "This is a test of IMAP Search");
         ImapClientSimulator.AssertMessageCount("search@test.com", "test", "INBOX", 2);

         var oSimulator = new ImapClientSimulator();

         string sWelcomeMessage = oSimulator.Connect();
         oSimulator.Logon("search@test.com", "test");
         Assert.IsTrue(oSimulator.SelectFolder("INBOX"));

         Assert.AreEqual("1 2", oSimulator.Sort("(ARRIVAL) UTF-8 ALL"));
         Assert.AreEqual("2 1", oSimulator.Sort("(REVERSE ARRIVAL) UTF-8 ALL"));
      }

      [Test]
      public void TestSortReverseSize()
      {
         Domain oDomain = _application.Domains[0];
         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "search@test.com", "test");

         var longBodyString = new StringBuilder();
         longBodyString.Append('A', 10000);

         // Send a message to this account.
         var smtpClientSimulator = new SmtpClientSimulator();
         smtpClientSimulator.Send("search@test.com", "search@test.com", "Test1", longBodyString.ToString());
         ImapClientSimulator.AssertMessageCount("search@test.com", "test", "INBOX", 1);

         smtpClientSimulator.Send("search@test.com", "search@test.com", "Test2", "Test body");
         ImapClientSimulator.AssertMessageCount("search@test.com", "test", "INBOX", 2);

         var oSimulator = new ImapClientSimulator();

         string sWelcomeMessage = oSimulator.Connect();
         oSimulator.Logon("search@test.com", "test");
         Assert.IsTrue(oSimulator.SelectFolder("INBOX"));

         Assert.AreEqual("2 1", oSimulator.Sort("(SIZE) UTF-8 ALL"));
         Assert.AreEqual("1 2", oSimulator.Sort("(REVERSE SIZE) UTF-8 ALL"));
      }

      [Test]
      public void TestSortSubject()
      {
         Domain oDomain = _application.Domains[0];
         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "search@test.com", "test");

         // Send a message to this account.
         var smtpClientSimulator = new SmtpClientSimulator();
         smtpClientSimulator.Send("search@test.com", "search@test.com", "Test1", "This is a test of IMAP Search");
         ImapClientSimulator.AssertMessageCount("search@test.com", "test", "INBOX", 1);
         smtpClientSimulator.Send("search@test.com", "search@test.com", "Test2", "This is a test of IMAP Search");
         ImapClientSimulator.AssertMessageCount("search@test.com", "test", "INBOX", 2);

         var oSimulator = new ImapClientSimulator();

         string sWelcomeMessage = oSimulator.Connect();
         oSimulator.Logon("search@test.com", "test");
         Assert.IsTrue(oSimulator.SelectFolder("INBOX"));

         Assert.AreEqual("1 2", oSimulator.Sort("(SUBJECT) UTF-8 ALL"));
      }

      [Test]
      public void TestSortSubjectReverse()
      {
         Domain oDomain = _application.Domains[0];
         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "search@test.com", "test");

         // Send a message to this account.
         var smtpClientSimulator = new SmtpClientSimulator();
         smtpClientSimulator.Send("search@test.com", "search@test.com", "Test1", "This is a test of IMAP Search");
         ImapClientSimulator.AssertMessageCount("search@test.com", "test", "INBOX", 1);
         smtpClientSimulator.Send("search@test.com", "search@test.com", "Test2", "This is a test of IMAP Search");
         ImapClientSimulator.AssertMessageCount("search@test.com", "test", "INBOX", 2);

         var oSimulator = new ImapClientSimulator();

         string sWelcomeMessage = oSimulator.Connect();
         oSimulator.Logon("search@test.com", "test");
         Assert.IsTrue(oSimulator.SelectFolder("INBOX"));

         Assert.AreEqual("2 1", oSimulator.Sort("(REVERSE SUBJECT) UTF-8 ALL"));
      }

      [Test]
      public void TestSortSubjectSearch()
      {
         Domain oDomain = _application.Domains[0];
         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "search@test.com", "test");

         // Send a message to this account.
         var smtpClientSimulator = new SmtpClientSimulator();
         smtpClientSimulator.Send("search@test.com", "search@test.com", "aa", "This is a test of IMAP Search");
         ImapClientSimulator.AssertMessageCount("search@test.com", "test", "INBOX", 1);
         smtpClientSimulator.Send("search@test.com", "search@test.com", "bb", "This is a test of IMAP Search");
         ImapClientSimulator.AssertMessageCount("search@test.com", "test", "INBOX", 2);

         var oSimulator = new ImapClientSimulator();
         string sWelcomeMessage = oSimulator.Connect();
         oSimulator.Logon("search@test.com", "test");
         Assert.IsTrue(oSimulator.SelectFolder("INBOX"));

         Assert.AreEqual("1 2", oSimulator.Sort("(DATE) UTF-8 ALL UNANSWERED OR HEADER SUBJECT aa HEADER SUBJECT bb"));
         Assert.AreEqual("1 2",
                         oSimulator.Sort("(DATE) UTF-8 ALL UNANSWERED OR (HEADER SUBJECT aa) (HEADER SUBJECT bb)"));
         Assert.AreEqual("1 2",
                         oSimulator.Sort("(DATE) UTF-8 ALL UNANSWERED (OR HEADER SUBJECT aa HEADER SUBJECT bb)"));

         Assert.AreEqual("1", oSimulator.Sort("(DATE) UTF-8 ALL UNANSWERED OR HEADER SUBJECT aa HEADER SUBJECT cc"));
         Assert.AreEqual("1",
                         oSimulator.Sort("(DATE) UTF-8 ALL UNANSWERED OR (HEADER SUBJECT aa) (HEADER SUBJECT cc)"));
         Assert.AreEqual("1", oSimulator.Sort("(DATE) UTF-8 ALL UNANSWERED (OR HEADER SUBJECT aa HEADER SUBJECT cc)"));

         Assert.AreEqual("2", oSimulator.Sort("(DATE) UTF-8 ALL UNANSWERED OR HEADER SUBJECT bb HEADER SUBJECT cc"));
         Assert.AreEqual("2",
                         oSimulator.Sort("(DATE) UTF-8 ALL UNANSWERED OR (HEADER SUBJECT bb) (HEADER SUBJECT cc)"));
         Assert.AreEqual("2", oSimulator.Sort("(DATE) UTF-8 ALL UNANSWERED (OR HEADER SUBJECT bb HEADER SUBJECT cc)"));
      }


      [Test]
      public void TestSubjectSearch()
      {
         Domain oDomain = _application.Domains[0];
         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "search@test.com", "test");

         // Send a message to this account.
         var smtpClientSimulator = new SmtpClientSimulator();
         smtpClientSimulator.Send("search@test.com", "search@test.com", "Test1", "This is a test of IMAP Search");
         ImapClientSimulator.AssertMessageCount("search@test.com", "test", "INBOX", 1);
         smtpClientSimulator.Send("search@test.com", "search@test.com", "Test2", "This is a test of IMAP Search");
         ImapClientSimulator.AssertMessageCount("search@test.com", "test", "INBOX", 2);

         var oSimulator = new ImapClientSimulator();

         string sWelcomeMessage = oSimulator.Connect();
         oSimulator.Logon("search@test.com", "test");
         Assert.IsTrue(oSimulator.SelectFolder("INBOX"));

         Assert.AreEqual("1", oSimulator.Sort("(REVERSE SUBJECT) UTF-8 ALL HEADER SUBJECT \"Test1\""));
         Assert.AreEqual("2", oSimulator.Sort("(REVERSE SUBJECT) UTF-8 ALL HEADER SUBJECT \"Test2\""));
         Assert.AreEqual("1", oSimulator.Sort("(REVERSE SUBJECT) UTF-8 ALL (HEADER SUBJECT \"Test1\")"));
         Assert.AreEqual("2", oSimulator.Sort("(REVERSE SUBJECT) UTF-8 ALL (HEADER SUBJECT \"Test2\")"));
      }


      [Test]
      public void TestSubjectSearchMultipleMatches()
      {
         Domain oDomain = _application.Domains[0];
         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "search@test.com", "test");

         // Send a message to this account.
         var smtpClientSimulator = new SmtpClientSimulator();
         smtpClientSimulator.Send("search@test.com", "search@test.com", "Test1", "This is a test of IMAP Search");
         ImapClientSimulator.AssertMessageCount("search@test.com", "test", "INBOX", 1);
         smtpClientSimulator.Send("search@test.com", "search@test.com", "TestA", "This is a test of IMAP Search");
         ImapClientSimulator.AssertMessageCount("search@test.com", "test", "INBOX", 2);
         smtpClientSimulator.Send("search@test.com", "search@test.com", "Test1", "This is a test of IMAP Search");
         ImapClientSimulator.AssertMessageCount("search@test.com", "test", "INBOX", 3);

         var oSimulator = new ImapClientSimulator();

         string sWelcomeMessage = oSimulator.Connect();
         oSimulator.Logon("search@test.com", "test");
         Assert.IsTrue(oSimulator.SelectFolder("INBOX"));

         Assert.AreEqual("1 3", oSimulator.Sort("(SUBJECT) UTF-8 ALL HEADER SUBJECT \"Test1\""));
         Assert.AreEqual("2", oSimulator.Sort("(SUBJECT) UTF-8 ALL HEADER SUBJECT \"TestA\""));
         Assert.AreEqual("3 1", oSimulator.Sort("(REVERSE SUBJECT) UTF-8 ALL HEADER SUBJECT \"Test1\""));
         Assert.AreEqual("2", oSimulator.Sort("(REVERSE SUBJECT) UTF-8 ALL HEADER SUBJECT \"TestA\""));
         Assert.AreEqual("3 1", oSimulator.Sort("(REVERSE SUBJECT) UTF-8 ALL (HEADER SUBJECT) \"Test1\""));
         Assert.AreEqual("2", oSimulator.Sort("(REVERSE SUBJECT) UTF-8 ALL (HEADER SUBJECT) \"TestA\""));
      }

      [Test]
      public void TestSubjectSearchValueWithParanthesis()
      {
         Domain oDomain = _application.Domains[0];
         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "search@test.com", "test");

         // Send a message to this account.
         var smtpClientSimulator = new SmtpClientSimulator();
         smtpClientSimulator.Send("search@test.com", "search@test.com", "Te(st1", "This is a test of IMAP Search");
         ImapClientSimulator.AssertMessageCount("search@test.com", "test", "INBOX", 1);
         smtpClientSimulator.Send("search@test.com", "search@test.com", "Te)st2", "This is a test of IMAP Search");
         ImapClientSimulator.AssertMessageCount("search@test.com", "test", "INBOX", 2);

         var oSimulator = new ImapClientSimulator();

         string sWelcomeMessage = oSimulator.Connect();
         oSimulator.Logon("search@test.com", "test");
         Assert.IsTrue(oSimulator.SelectFolder("INBOX"));

         Assert.AreEqual("1", oSimulator.Sort("(SUBJECT) UTF-8 ALL HEADER SUBJECT \"Te(st1\""));
         Assert.AreEqual("2", oSimulator.Sort("(SUBJECT) UTF-8 ALL HEADER SUBJECT \"Te)st2\""));
      }
   }
}