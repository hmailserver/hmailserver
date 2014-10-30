// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.IO;
using System.Text;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.AntiSpam
{
   [TestFixture]
   public class Basics : TestFixtureBase
   {
      #region Setup/Teardown

      [SetUp]
      public new void SetUp()
      {
         _antiSpam = _settings.AntiSpam;
      }

      #endregion

      private hMailServer.AntiSpam _antiSpam;

      [Test]
      public void TestDNSBlackList()
      {
         DNSBlackLists dnsBlackLists = SingletonProvider<TestSetup>.Instance.GetApp().Settings.AntiSpam.DNSBlackLists;

         DNSBlackList dnsBlackList = dnsBlackLists.Add();
         dnsBlackList.DNSHost = "zen.spamhaus.org";
         dnsBlackList.RejectMessage = "srv1";
         dnsBlackList.Score = 5;
         dnsBlackList.Active = true;
         dnsBlackList.Save();

         dnsBlackList = dnsBlackLists.Add();
         dnsBlackList.DNSHost = "bl.spamcop.net";
         dnsBlackList.RejectMessage = "srv2";
         dnsBlackList.Score = 5;
         dnsBlackList.Active = false;
         dnsBlackList.Save();

         dnsBlackList = dnsBlackLists.Add();
         dnsBlackList.DNSHost = "dnsbl.njabl.org";
         dnsBlackList.RejectMessage = "srv3";
         dnsBlackList.Score = 5;
         dnsBlackList.Active = true;
         dnsBlackList.Save();

         Application application = SingletonProvider<TestSetup>.Instance.GetApp();
         _antiSpam.SpamMarkThreshold = 1;
         _antiSpam.SpamDeleteThreshold = 100;

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "dnsbltest@test.com", "test");

         LogHandler.DeleteCurrentDefaultLog();

         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test", "TestBody");
         Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         string result = LogHandler.ReadCurrentDefaultLog();

         Assert.IsTrue(result.Contains(".zen.spamhaus.org, 0 addresses found: (none), Match: False"), result);
         Assert.IsTrue(result.Contains(".dnsbl.njabl.org, 0 addresses found: (none), Match: False"), result);
         Assert.IsFalse(result.Contains(".bl.spamcop.net, 0 addresses found:"), result);
      }

      [Test]
      public void TestHeloSpamTest()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();

         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         // Disallow incorrect line endings.
         _antiSpam.SpamDeleteThreshold = 100;

         _antiSpam.CheckHostInHelo = true;
         _antiSpam.CheckHostInHeloScore = 125;

         // Send a messages to this account.
         var smtpClientSimulator = new SmtpClientSimulator();

         CustomAsserts.Throws<DeliveryFailedException>(() => smtpClientSimulator.Send("whitelist@microsoft.com", "whitelist@test.com", "SURBL-Match", "Test"));
      }

      [Test]
      public void TestIncorrectLineEndings()
      {
         // Create a test account
         // Fetch the default domain
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain,
                                                                              "SpamProtectionLineEndings@test.com",
                                                                              "test");

         // Disallow incorrect line endings.
         SingletonProvider<TestSetup>.Instance.GetApp().Settings.AllowIncorrectLineEndings = false;

         // Send a messages to this account.

         var smtpClientSimulator = new SmtpClientSimulator();
         smtpClientSimulator.Send("SpamProtectionLineEndings@test.com", "SpamProtectionLineEndings@test.com", "INBOX",
            "This is a test message\r\n consisting of correct lines");

         CustomAsserts.Throws<DeliveryFailedException>(() => smtpClientSimulator.Send("SpamProtectionLineEndings@test.com",
            "SpamProtectionLineEndings@test.com", "INBOX",
            "This is a test message\r consisting of incorrect lines"));



         CustomAsserts.Throws<DeliveryFailedException>(
            () => smtpClientSimulator.Send("SpamProtectionLineEndings@test.com", "SpamProtectionLineEndings@test.com", "INBOX",
               "This is a test message\n consisting of incorrect lines"));


         CustomAsserts.Throws<DeliveryFailedException>(() => smtpClientSimulator.Send("SpamProtectionLineEndings@test.com",
            "SpamProtectionLineEndings@test.com", "INBOX",
            "This is a test message\n\r consisting of incorrect lines"));

         Pop3ClientSimulator.AssertMessageCount(oAccount1.Address, "test", 1);
      }

      [Test]
      public void TestMaxSizeLimit()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();
         // Create a test account
         // Fetch the default domain
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "surbltest@test.com", "test");

         // Disallow incorrect line endings.
         _antiSpam.SpamMarkThreshold = 1;
         _antiSpam.SpamDeleteThreshold = 100;
         _antiSpam.AddHeaderReason = true;
         _antiSpam.AddHeaderSpam = true;
         _antiSpam.PrependSubject = true;
         _antiSpam.PrependSubjectText = "ThisIsSpam";
         _antiSpam.MaximumMessageSize = 40;

         // Enable SURBL.
         SURBLServer oSURBLServer = _antiSpam.SURBLServers[0];
         oSURBLServer.Active = true;
         oSURBLServer.Score = 5;
         oSURBLServer.Save();

         // Send a messages to this account.
         var smtpClientSimulator = new SmtpClientSimulator();

         var sb = new StringBuilder();
         int iterations = ((40*1024)/100) + 1;
         for (int i = 0; i < iterations; i++)
         {
            sb.Append(
               "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\r\n");
         }

         smtpClientSimulator.Send("surbltest@test.com", "surbltest@test.com", "SURBL-No-Match",
                                  "This is a test message with a SURBL url: -> http://surbl-org-permanent-test-point.com/ <-\r\n" +
                                  sb);

         string sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");
         if (sMessageContents.Contains("X-hMailServer-Spam") ||
             sMessageContents.Contains("X-hMailServer-Reason") ||
             sMessageContents.Contains("ThisIsSpam"))
            throw new Exception("Spam message etected as spam even though it's larger than max spam size.");

         oSURBLServer.Active = false;
         oSURBLServer.Save();
      }

      [Test]
      public void TestMaxSizeNoLimit()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();
         // Create a test account
         // Fetch the default domain

         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "surbltest@test.com", "test");

         // Disallow incorrect line endings.
         _antiSpam.SpamMarkThreshold = 1;
         _antiSpam.SpamDeleteThreshold = 100;
         _antiSpam.AddHeaderReason = true;
         _antiSpam.AddHeaderSpam = true;
         _antiSpam.PrependSubject = true;
         _antiSpam.PrependSubjectText = "ThisIsSpam";
         _antiSpam.MaximumMessageSize = 0;

         // Enable SURBL.
         SURBLServer oSURBLServer = _antiSpam.SURBLServers[0];
         oSURBLServer.Active = true;
         oSURBLServer.Score = 5;
         oSURBLServer.Save();

         // Send a messages to this account.
         var smtpClientSimulator = new SmtpClientSimulator();

         var sb = new StringBuilder();
         int iterations = ((40*1024)/100) + 1;
         for (int i = 0; i < iterations; i++)
         {
            sb.Append(
               "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\r\n");
         }

         smtpClientSimulator.Send("surbltest@test.com", "surbltest@test.com", "SURBL-No-Match",
                                  "This is a test message with a SURBL url: -> http://surbl-org-permanent-test-point.com/ <-\r\n" +
                                  sb);

         string sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");
         if (!sMessageContents.Contains("X-hMailServer-Spam") ||
             !sMessageContents.Contains("X-hMailServer-Reason") ||
             !sMessageContents.Contains("ThisIsSpam"))
            throw new Exception("Spam message not detected as spam.");

         oSURBLServer.Active = false;
         oSURBLServer.Save();
      }

      [Test]
      public void TestMissingMXRecord()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();

         // Create a test account
         // Fetch the default domain

         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "missingmxrecords@test.com",
                                                                              "test");

         // Disallow incorrect line endings.

         _antiSpam.SpamDeleteThreshold = 1;
         _antiSpam.UseMXChecks = true;
         _antiSpam.UseMXChecksScore = 2;

         // Send a messages to this account.
         var smtpClientSimulator = new SmtpClientSimulator();

         smtpClientSimulator.Send("test@microsoft.com", "missingmxrecords@test.com", "INBOX", "This is a test message.");

         CustomAsserts.Throws<DeliveryFailedException>(
            () => smtpClientSimulator.Send("test@domain_without_mx_records421dfsam430sasd.com", oAccount1.Address, "INBOX",
               "This is a test message."));

         _antiSpam.UseMXChecks = false;

         Pop3ClientSimulator.AssertMessageCount(oAccount1.Address, "test", 1);
      }

      [Test]
      public void TestMultiLineSurblImproperlyTerminaetdLine()
      {
         // Create a test account
         // Fetch the default domain

         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "surbltest@test.com", "test");

         // Disallow incorrect line endings.
         _antiSpam.SpamMarkThreshold = 1;
         _antiSpam.SpamDeleteThreshold = 100;
         _antiSpam.AddHeaderReason = true;
         _antiSpam.AddHeaderSpam = true;
         _antiSpam.PrependSubject = true;
         _antiSpam.PrependSubjectText = "ThisIsSpam";

         // Enable SURBL.
         SURBLServer oSURBLServer = _antiSpam.SURBLServers[0];
         oSURBLServer.Active = true;
         oSURBLServer.Score = 5;
         oSURBLServer.Save();

         // Send a messages to this account.
         var smtpClientSimulator = new SmtpClientSimulator();

         smtpClientSimulator.Send("surbltest@test.com", "surbltest@test.com", "SURBL-Match",
                    "Wrapped URL - <a href=3D\"http://surbl-org-perm\r\nanent-test-point.com\">Test</a>");

         string sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");
         Assert.IsFalse(sMessageContents.Contains("X-hMailServer-Spam"), "Non-spam message detected as spam");

         oSURBLServer.Active = false;
         oSURBLServer.Save();
      }

      [Test]
      public void TestSPF()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();
         // Create a test account
         // Fetch the default domain

         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "spftest@test.com", "test");

         // Disallow incorrect line endings.
         _antiSpam.SpamMarkThreshold = 1;
         _antiSpam.SpamDeleteThreshold = 100;
         _antiSpam.AddHeaderReason = true;
         _antiSpam.AddHeaderSpam = true;
         _antiSpam.PrependSubject = true;
         _antiSpam.PrependSubjectText = "ThisIsSpam";

         // Enable SPF
         _antiSpam.UseSPF = true;
         _antiSpam.UseSPFScore = 5;

         // Send a messages to this account.
         var smtpClientSimulator = new SmtpClientSimulator();

         smtpClientSimulator.Send("spftest@openspf.org", oAccount1.Address, "SPF test", "This is a test message.");

         string sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");
         if (!sMessageContents.Contains("X-hMailServer-Spam"))
            throw new Exception("Spam message not detected as spam");

         _antiSpam.UseSPF = false;

         smtpClientSimulator.Send("spftest@openspf.org", oAccount1.Address, "SPF test", "This is a test message.");

         sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");
         if (sMessageContents.Contains("X-hMailServer-Spam"))
            throw new Exception("Non-spam message detected as spam");
      }


      [Test]
      public void TestSPFWithDebugLogging()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();

         string debugLog = _settings.Logging.CurrentDefaultLog;
         CustomAsserts.AssertDeleteFile(debugLog);

         // Create a test account
         // Fetch the default domain

         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "spftest@test.com", "test");

         // Disallow incorrect line endings.
         _antiSpam.SpamMarkThreshold = 1;
         _antiSpam.SpamDeleteThreshold = 100;
         _antiSpam.AddHeaderReason = true;
         _antiSpam.AddHeaderSpam = true;
         _antiSpam.PrependSubject = true;
         _antiSpam.PrependSubjectText = "ThisIsSpam";

         // Enable SPF
         _antiSpam.UseSPF = true;
         _antiSpam.UseSPFScore = 12;

         // Send a messages to this account.
         var smtpClientSimulator = new SmtpClientSimulator();

         smtpClientSimulator.Send("spftest@openspf.org", oAccount1.Address, "SPF test", "This is a test message.");

         string sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");
         if (!sMessageContents.Contains("X-hMailServer-Spam"))
            throw new Exception("Spam message not detected as spam");

         // Check that it has been logged.
         string contents = TestSetup.ReadExistingTextFile(debugLog);
         Assert.IsTrue(contents.Contains("Total spam score: 12"));
         Assert.IsTrue(contents.Contains("Spam test: SpamTestSPF, Score: 12"));

         CustomAsserts.AssertDeleteFile(debugLog);
      }

      [Test]
      public void TestSURBL()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();
         // Create a test account
         // Fetch the default domain

         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "surbltest@test.com", "test");

         // Disallow incorrect line endings.
         _antiSpam.SpamMarkThreshold = 1;
         _antiSpam.SpamDeleteThreshold = 100;
         _antiSpam.AddHeaderReason = true;
         _antiSpam.AddHeaderSpam = true;
         _antiSpam.PrependSubject = true;
         _antiSpam.PrependSubjectText = "ThisIsSpam";

         // Enable SURBL.
         SURBLServer oSURBLServer = _antiSpam.SURBLServers[0];
         oSURBLServer.Active = true;
         oSURBLServer.Score = 5;
         oSURBLServer.Save();

         // Send a messages to this account.
         var smtpClientSimulator = new SmtpClientSimulator();


         smtpClientSimulator.Send("surbltest@test.com", "surbltest@test.com", "SURBL-No-Match",
                    "This is a test message without a SURBL url.");

         string sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");
         if (sMessageContents.Length == 0 ||
             sMessageContents.Contains("X-hMailServer-Spam") ||
             sMessageContents.Contains("X-hMailServer-Reason") ||
             sMessageContents.Contains("ThisIsSpam"))
            throw new Exception("Non-Spam message detected as spam");


         smtpClientSimulator.Send("surbltest@test.com", "surbltest@test.com", "SURBL-Match",
                    "This is a test message with a SURBL url: -> http://surbl-org-permanent-test-point.com/ <-");

         sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");
         if (!sMessageContents.Contains("X-hMailServer-Spam") ||
             !sMessageContents.Contains("X-hMailServer-Reason") ||
             !sMessageContents.Contains("ThisIsSpam"))
            throw new Exception("Spam message not detected as spam");

         oSURBLServer.Active = false;
         oSURBLServer.Save();
      }

      [Test]
      public void TestSURBLCombinedWithSignature()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();
         // Create a test account
         // Fetch the default domain

         _domain.SignatureEnabled = true;
         _domain.SignaturePlainText = "MyDomainSignature";
         _domain.AddSignaturesToLocalMail = true;
         _domain.Save();

         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "surbltest@test.com", "test");

         // Disallow incorrect line endings.
         _antiSpam.SpamMarkThreshold = 1;
         _antiSpam.SpamDeleteThreshold = 100;
         _antiSpam.AddHeaderReason = true;
         _antiSpam.AddHeaderSpam = true;
         _antiSpam.PrependSubject = true;
         _antiSpam.PrependSubjectText = "ThisIsSpam";

         // Enable SURBL.
         SURBLServer oSURBLServer = _antiSpam.SURBLServers[0];
         oSURBLServer.Active = true;
         oSURBLServer.Score = 5;
         oSURBLServer.Save();

         // Send a messages to this account.
         var smtpClientSimulator = new SmtpClientSimulator();

         smtpClientSimulator.Send("surbltest@test.com", "surbltest@test.com", "SURBL-No-Match",
                    "This is a test message without a SURBL url.");

         string sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");
         if (sMessageContents.Length == 0 ||
             sMessageContents.Contains("X-hMailServer-Spam") ||
             sMessageContents.Contains("X-hMailServer-Reason") ||
             sMessageContents.Contains("ThisIsSpam"))
            throw new Exception("Non-Spam message detected as spam");

         Assert.IsTrue(sMessageContents.Contains(_domain.SignaturePlainText));

         smtpClientSimulator.Send("surbltest@test.com", "surbltest@test.com", "SURBL-No-Match",
                    "This is a test message with a SURBL url: -> http://surbl-org-permanent-test-point.com/ <-");

         sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");
         if (!sMessageContents.Contains("X-hMailServer-Spam") ||
             !sMessageContents.Contains("X-hMailServer-Reason") ||
             !sMessageContents.Contains("ThisIsSpam"))
            throw new Exception("Spam message not detected as spam");

         Assert.IsTrue(sMessageContents.Contains(_domain.SignaturePlainText));
      }

      [Test]
      public void TestSURBLCorrectNegative()
      {
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "surbltest@test.com", "test");

         // Disallow incorrect line endings.
         _antiSpam.SpamMarkThreshold = 1;
         _antiSpam.SpamDeleteThreshold = 100;
         _antiSpam.AddHeaderReason = true;
         _antiSpam.AddHeaderSpam = true;
         _antiSpam.PrependSubject = true;
         _antiSpam.PrependSubjectText = "ThisIsSpam";

         // Enable SURBL.
         SURBLServer oSURBLServer = _antiSpam.SURBLServers[0];
         oSURBLServer.Active = true;
         oSURBLServer.Score = 5;
         oSURBLServer.Save();

         // Send a messages to this account.
         SmtpClientSimulator.StaticSend("surbltest@test.com", "surbltest@test.com", "SURBL-Match",
                                                      "This is a test message without a SURBL url: -> http://www.youtube.com/ <-");

         string sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");
         if (sMessageContents.Contains("X-hMailServer-Spam"))
            throw new Exception("Non-spam message detected as spam");

         oSURBLServer.Active = false;
         oSURBLServer.Save();
      }

      [Test]
      public void TestSURBLWithWrappedURL()
      {
         // Create a test account
         // Fetch the default domain
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "surbltest@test.com", "test");

         // Disallow incorrect line endings.
         _antiSpam.SpamMarkThreshold = 1;
         _antiSpam.SpamDeleteThreshold = 100;
         _antiSpam.AddHeaderReason = true;
         _antiSpam.AddHeaderSpam = true;
         _antiSpam.PrependSubject = true;
         _antiSpam.PrependSubjectText = "ThisIsSpam";

         // Enable SURBL.
         SURBLServer oSURBLServer = _antiSpam.SURBLServers[0];
         oSURBLServer.Active = true;
         oSURBLServer.Score = 5;
         oSURBLServer.Save();

         // Send a messages to this account.
         var smtpClientSimulator = new SmtpClientSimulator();

         smtpClientSimulator.Send("surbltest@test.com", "surbltest@test.com", "SURBL-Match",
                    "Wrapped URL - <a href=3D\"http://surbl-org-perma=\r\nnent-test-point.com\">Test</a>");

         string sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");
         Assert.IsTrue(sMessageContents.Contains("X-hMailServer-Spam"), "Spam message not detected as spam");

         oSURBLServer.Active = false;
         oSURBLServer.Save();
      }

      [Test]
      public void TestSingleLineUrlFollowedByNewline()
      {
         // Create a test account
         // Fetch the default domain
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "surbltest@test.com", "test");

         // Disallow incorrect line endings.
         _antiSpam.SpamMarkThreshold = 1;
         _antiSpam.SpamDeleteThreshold = 100;
         _antiSpam.AddHeaderReason = true;
         _antiSpam.AddHeaderSpam = true;
         _antiSpam.PrependSubject = true;
         _antiSpam.PrependSubjectText = "ThisIsSpam";

         // Enable SURBL.
         SURBLServer oSURBLServer = _antiSpam.SURBLServers[0];
         oSURBLServer.Active = true;
         oSURBLServer.Score = 5;
         oSURBLServer.Save();

         // Send a messages to this account.
         var smtpClientSimulator = new SmtpClientSimulator();

         smtpClientSimulator.Send("surbltest@test.com", "surbltest@test.com", "SURBL-Match",
                    "Wrapped URL - <a href=3D\"http://surbl-org-permanent-test-point.com\r\nHello\">Test</a>");

         string sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");
         Assert.IsTrue(sMessageContents.Contains("X-hMailServer-Spam"), "Spam message not detected as spam");

         oSURBLServer.Active = false;
         oSURBLServer.Save();
      }

      [Test]
      public void TestSurblMultipleAddresses()
      {
         // Create a test account
         // Fetch the default domain
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "surbltest@test.com", "test");

         // Disallow incorrect line endings.
         _antiSpam.SpamMarkThreshold = 1;
         _antiSpam.SpamDeleteThreshold = 100;
         _antiSpam.AddHeaderReason = true;
         _antiSpam.AddHeaderSpam = true;
         _antiSpam.PrependSubject = true;
         _antiSpam.PrependSubjectText = "ThisIsSpam";

         // Enable SURBL.
         SURBLServer oSURBLServer = _antiSpam.SURBLServers[0];
         oSURBLServer.Active = true;
         oSURBLServer.Score = 5;
         oSURBLServer.Save();

         // Send a messages to this account.
         var smtpClientSimulator = new SmtpClientSimulator();

         smtpClientSimulator.Send("surbltest@test.com", "surbltest@test.com", "SURBL-Match",
                    "Wrapped URL - <a href=3D\"http://surbl-org-permanent-test-point.com\">Test</a>\r\nWrapped URL - <a href=3D\"http://surbl-org-permanent-test-point.com\">Test</a>\r\nWrapped URL - <a href=3D\"http://surbl-org-permanent-test-point.com\">Test</a>\r\n");

         string sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");
         Assert.IsTrue(sMessageContents.Contains("X-hMailServer-Spam"), "Spam message not detected as spam");

         oSURBLServer.Active = false;
         oSURBLServer.Save();
      }

      [Test]
      public void TestSurblAddressEndingWithSingleQuote()
      {
         LogHandler.DeleteCurrentDefaultLog();
         

         // Create a test account
         // Fetch the default domain
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "surbltest@test.com", "test");

         // Disallow incorrect line endings.
         _antiSpam.SpamMarkThreshold = 1;
         _antiSpam.SpamDeleteThreshold = 100;
         _antiSpam.AddHeaderReason = true;
         _antiSpam.AddHeaderSpam = true;
         _antiSpam.PrependSubject = true;
         _antiSpam.PrependSubjectText = "ThisIsSpam";

         // Enable SURBL.
         SURBLServer oSURBLServer = _antiSpam.SURBLServers[0];
         oSURBLServer.Active = true;
         oSURBLServer.Score = 5;
         oSURBLServer.Save();

         // Send a messages to this account.
         var smtpClientSimulator = new SmtpClientSimulator();

         smtpClientSimulator.Send("surbltest@test.com", "surbltest@test.com", "SURBL-Match",
                    "Wrapped URL - <a href=3D'http://surbl-org-permanent-test-point.com'>Test</a>\r\n");

         oSURBLServer.Active = false;
         oSURBLServer.Save();

         Assert.IsFalse(LogHandler.DefaultLogContains("Lookup: surbl-org-permanent-test-point.com'.multi.surbl.org"));
         Assert.IsTrue(LogHandler.DefaultLogContains("Lookup: surbl-org-permanent-test-point.com.multi.surbl.org"));
      }

      [Test]
      public void TestSurblMultipleNegatives()
      {
         LogHandler.DeleteCurrentDefaultLog();

         // Create a test account
         // Fetch the default domain
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "surbltest@test.com", "test");

         // Disallow incorrect line endings.
         _antiSpam.SpamMarkThreshold = 1;
         _antiSpam.SpamDeleteThreshold = 100;
         _antiSpam.AddHeaderReason = true;
         _antiSpam.AddHeaderSpam = true;
         _antiSpam.PrependSubject = true;
         _antiSpam.PrependSubjectText = "ThisIsSpam";

         // Enable SURBL.
         SURBLServer oSURBLServer = _antiSpam.SURBLServers[0];
         oSURBLServer.Active = true;
         oSURBLServer.Score = 5;
         oSURBLServer.Save();

         // Send a messages to this account.
         var smtpClientSimulator = new SmtpClientSimulator();

         smtpClientSimulator.Send("surbltest@test.com", "surbltest@test.com", "SURBL-Match",
                    "Wrapped URL - <a href=3D\"http://test.example1fdafdsfds.com\">Test</a>\r\nWrapped URL - <a href=3D\"http://test.example2fdafdsfds.com\">Test</a>\r\nWrapped URL - <a href=3D\"http://test.example3fdafdsfds.com\">Test</a>\r\n");

         string sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");
         Assert.IsFalse(sMessageContents.Contains("X-hMailServer-Spam"), "Spam message not detected as spam");

         oSURBLServer.Active = false;
         oSURBLServer.Save();

         Assert.IsTrue(LogHandler.DefaultLogContains("SURBL: Lookup: example1fdafdsfds.com.multi.surbl.org"));
         Assert.IsTrue(LogHandler.DefaultLogContains("SURBL: Lookup: example2fdafdsfds.com.multi.surbl.org"));
         Assert.IsTrue(LogHandler.DefaultLogContains("SURBL: Lookup: example3fdafdsfds.com.multi.surbl.org"));
      }

      [Test]
      public void SurblTestRealWorldBody1()
      {
         LogHandler.DeleteCurrentDefaultLog();

         // Create a test account
         // Fetch the default domain
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "surbltest@test.com", "test");

         // Disallow incorrect line endings.
         _antiSpam.SpamMarkThreshold = 1;
         _antiSpam.SpamDeleteThreshold = 100;
         _antiSpam.AddHeaderReason = true;
         _antiSpam.AddHeaderSpam = true;
         _antiSpam.PrependSubject = true;
         _antiSpam.PrependSubjectText = "ThisIsSpam";

         // Enable SURBL.
         SURBLServer oSURBLServer = _antiSpam.SURBLServers[0];
         oSURBLServer.Active = true;
         oSURBLServer.Score = 5;
         oSURBLServer.Save();

         // Send a messages to this account.
         var smtpClientSimulator = new SmtpClientSimulator();

         smtpClientSimulator.Send("surbltest@test.com", "surbltest@test.com", "SURBL-Match", TestResources.SecuniaBody1);

         string sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");
         Assert.IsFalse(sMessageContents.Contains("X-hMailServer-Spam"), "Spam message not detected as spam");

         oSURBLServer.Active = false;
         oSURBLServer.Save();


         Assert.IsTrue(LogHandler.DefaultLogContains("SURBL: 2 unique addresses found."));
         Assert.IsTrue(LogHandler.DefaultLogContains("SURBL: Found URL: secunia.com"));
         Assert.IsFalse(LogHandler.DefaultLogContains("SURBL: Found URL: ecunia.com"));
         Assert.IsTrue(LogHandler.DefaultLogContains("SURBL: Lookup: secunia.com.multi.surbl.org"));
         Assert.IsTrue(LogHandler.DefaultLogContains("SURBL: Lookup: ubuntu.com.multi.surbl.org"));
      }
   }
}