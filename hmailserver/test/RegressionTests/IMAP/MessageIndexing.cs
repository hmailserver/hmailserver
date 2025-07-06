// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using NUnit.Framework;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.IMAP
{
   [TestFixture]
   public class MessageIndexing : TestFixtureBase
   {
      #region Setup/Teardown

      [SetUp]
      public new void SetUp()
      {
         _application.Settings.IMAPSortEnabled = true;
         _indexing = _application.Settings.MessageIndexing;
      }

      #endregion

      private hMailServer.MessageIndexing _indexing;

      private void AssertAllMessagesIndexed()
      {
         if (_indexing.Enabled == false)
            _indexing.Enabled = true;

         _indexing.Index();

         for (int i = 0; i < 1000; i++)
         {
            if (_indexing.TotalIndexedCount == _indexing.TotalMessageCount)
               return;

            Thread.Sleep(20);
         }

         Assert.Fail("Messages not indexed. Message count: " + _indexing.TotalMessageCount + ", indexed count: " + _indexing.TotalIndexedCount);
      }

      private void SendMessage(string subject, string body, string to, string cc)
      {
         var message = new Message();

         message.Charset = "UTF-8";
         message.AddRecipient("test", "meta'data@test.com");
         message.From = "Test";
         message.FromAddress = "meta'data@test.com";
         message.Subject = subject;
         message.set_HeaderValue("To", to);
         message.set_HeaderValue("cc", cc);
         message.Body = body;
         message.Save();
      }

      [Test]
      [Description("Test message metadata date")]
      public void TestMetaDataSortCC()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "meta'data@test.com", "test");

         // disable...
         SendMessage("Test C", "Body", "", "ÄÄÄ");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 1);

         SendMessage("Test B", "Body", "", "ÖÖÖ");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 2);

         SendMessage("Test A", "Body", "", "ÅÅÅ");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 3);

         AssertAllMessagesIndexed();

         var sim = new ImapClientSimulator(account.Address, "test", "Inbox");

         string result = sim.Sort("(CC) UTF-8 ALL");

         var locale = GetSystemLocaleName();

         switch (locale)
         {
            case "en-US":
               Assert.AreEqual("1 3 2", result);
               break;
            case "sv-SE":
               Assert.AreEqual("3 1 2", result);
               break;
            default:
               throw new InvalidOperationException("Unsupported system locale: " + locale);
         }
         

         // Disable the indexing functionality
         _indexing.Enabled = false;
         _indexing.Clear();

         // Make sure the sort order is the same.
         string resultAfter = sim.Sort("(CC) UTF-8 ALL");

         Assert.AreEqual(result, resultAfter);
      }

      [Test]
      [Description("Test message metadata date")]
      public void TestMetaDataSortDate()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "meta'data@test.com", "test");

         // disable...
         SendMessage("Test A", "Body", "", "");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 1);

         SendMessage("Test B", "Body", "", "");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 2);

         SendMessage("Test C", "Body", "", "");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 3);

         SendMessage("Test D", "Body", "", "");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 4);

         SendMessage("Test E", "Body", "", "");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 5);

         AssertAllMessagesIndexed();

         var sim = new ImapClientSimulator(account.Address, "test", "Inbox");

         string result = sim.Sort("(DATE) UTF-8 ALL");

         Assert.AreEqual("1 2 3 4 5", result);

         // Disable the indexing functionality
         _indexing.Enabled = false;
         _indexing.Clear();

         // Make sure the sort order is the same.
         string resultAfter = sim.Sort("(SUBJECT) UTF-8 ALL");

         Assert.AreEqual(result, resultAfter);
      }

      [Test]
      [Description("Test message metadata subject")]
      public void TestMetaDataSortSubjectAnsi()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "meta'data@test.com", "test");

         // disable...
         SendMessage("Test 1", "Body", "", "");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 1);

         SendMessage("Test 2", "Body", "", "");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 2);

         AssertAllMessagesIndexed();

         var sim = new ImapClientSimulator(account.Address, "test", "Inbox");

         string result = sim.Sort("(SUBJECT) UTF-8 ALL");

         Assert.IsTrue(result.StartsWith("1 2"));

         // Disable the indexing functionality
         _indexing.Enabled = false;
         _indexing.Clear();

         // Make sure the sort order is the same.
         string resultAfter = sim.Sort("(SUBJECT) UTF-8 ALL");

         Assert.AreEqual(result, resultAfter);
      }


      [Test]
      [Description("Test message metadata subject")]
      public void TestMetaDataSortSubjectGreek()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "meta'data@test.com", "test");

         // disable...
         SendMessage("Test Σ", "Body", "", "");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 1);
         // pos: 18

         SendMessage("Test Α", "Body", "", "");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 2);
         // pos: 1

         SendMessage("Test Δ", "Body", "", "");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 3);
         // pos: 4

         SendMessage("Test β", "Body", "", "");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 4);
         // pos: 2

         SendMessage("Test κ", "Body", "", "");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 5);
         // pos: 10

         SendMessage("Test Ψ", "Body", "", "");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 6);
         // pos: 23

         AssertAllMessagesIndexed();

         var sim = new ImapClientSimulator(account.Address, "test", "Inbox");

         string result = sim.Sort("(SUBJECT) UTF-8 ALL");

         Assert.AreEqual("2 4 3 5 1 6", result);

         // Disable the indexing functionality
         _indexing.Enabled = false;
         _indexing.Clear();

         // Make sure the sort order is the same.
         string resultAfter = sim.Sort("(SUBJECT) UTF-8 ALL");

         Assert.AreEqual(result, resultAfter);
      }

      [Test]
      [Description("Test message metadata subject")]
      public void TestMetaDataSortSubjectSwedish()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "meta'data@test.com", "test");

         // disable...
         SendMessage("Test Ä", "Body", "", "");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 1);

         SendMessage("Test Ö", "Body", "", "");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 2);

         SendMessage("Test Å", "Body", "", "");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 3);

         AssertAllMessagesIndexed();

         var sim = new ImapClientSimulator(account.Address, "test", "Inbox");

         string result = sim.Sort("(SUBJECT) UTF-8 ALL");

         var locale = GetSystemLocaleName();

         switch (locale)
         {
            case "en-US":
               Assert.AreEqual("1 3 2", result);
               break;
            case "sv-SE":
               Assert.AreEqual("3 1 2", result);
               break;
            default:
               throw new InvalidOperationException("Unsupported system locale: " + locale);
         }

         // Disable the indexing functionality
         _indexing.Enabled = false;
         _indexing.Clear();

         // Make sure the sort order is the same.
         string resultAfter = sim.Sort("(SUBJECT) UTF-8 ALL");

         Assert.AreEqual(result, resultAfter);
      }

      [Test]
      [Description("Test message metadata subject")]
      public void TestMetaDataSortSubjectTurkish()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "meta'data@test.com", "test");

         // disable...
         SendMessage("Test Ç", "Body", "", "");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 1);
         // pos: 4

         SendMessage("Test C", "Body", "", "");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 2);
         // pos: 3

         SendMessage("Test B", "Body", "", "");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 3);
         // pos: 2

         SendMessage("Test Ğ", "Body", "", "");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 4);
         // pos: 9

         SendMessage("Test G", "Body", "", "");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 5);
         // pos: 8

         AssertAllMessagesIndexed();

         var sim = new ImapClientSimulator(account.Address, "test", "Inbox");

         string result = sim.Sort("(SUBJECT) UTF-8 ALL");

         Assert.AreEqual("3 2 1 5 4", result);

         // Disable the indexing functionality
         _indexing.Enabled = false;
         _indexing.Clear();

         // Make sure the sort order is the same.
         string resultAfter = sim.Sort("(SUBJECT) UTF-8 ALL");

         Assert.AreEqual(result, resultAfter);
      }

      [Test]
      [Description("Test message metadata date")]
      public void TestMetaDataSortTo()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "meta'data@test.com", "test");

         // disable...
         SendMessage("Test A", "Body", "ÅÅÅ", "");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 1);

         SendMessage("Test B", "Body", "ÖÖÖ", "");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 2);

         SendMessage("Test C", "Body", "ÄÄÄ", "");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 3);

         AssertAllMessagesIndexed();

         var sim = new ImapClientSimulator(account.Address, "test", "Inbox");

         string result = sim.Sort("(TO) UTF-8 ALL");

         var locale = GetSystemLocaleName();

         switch (locale)
         {
            case "en-US":
               Assert.AreEqual("3 1 2", result);
               break;
            case "sv-SE":
               Assert.AreEqual("1 3 2", result);
               break;
            default:
               throw new InvalidOperationException("Unsupported system locale: " + locale);
         }

         // Disable the indexing functionality
         _indexing.Enabled = false;
         _indexing.Clear();

         // Make sure the sort order is the same.
         string resultAfter = sim.Sort("(TO) UTF-8 ALL");

         Assert.AreEqual(result, resultAfter);
      }

      // P/Invoke signature
      [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
      private static extern int GetSystemDefaultLocaleName(
         [Out] StringBuilder lpLocaleName,
         int cchLocaleName
      );


      private string GetSystemLocaleName()
      {
         const int LOCALE_NAME_MAX_LENGTH = 85; // Max length per Windows API docs
         StringBuilder localeName = new StringBuilder(LOCALE_NAME_MAX_LENGTH);

         int result = GetSystemDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH);
         if (result > 0)
         {
            return localeName.ToString();
         }
         else
         {
            throw new InvalidOperationException("Unable to read system locale.");
         }
      }
   }
}