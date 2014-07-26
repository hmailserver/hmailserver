using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Shared;
using RegressionTests.SSL;

namespace RegressionTests.POP3.Fetching
{
   [TestFixture]
   public class ExternalAccountSslTests : TestFixtureBase
   {


      [Test]
      [Description("Tests to connect to a normal non-SSL POP3 server using SSL. An error should be reported and hMailServer should disconnect correctly.")]
      public void TestBasicExternalAccountSSLEvenThoughNotSupported()
      {
         var messages = new List<string>();

         string message = "From: Martin@example.com\r\n" +
                          "To: Martin@example.com\r\n" +
                          "Subject: Test\r\n" +
                          "\r\n" +
                          "Hello!";

         messages.Add(message);

         int port = TestSetup.GetNextFreePort();
         using (var pop3Server = new POP3Server(2, port, messages))
         {
            pop3Server.DisconnectImmediate = true;
            pop3Server.StartListen();

            TestSetup.DeleteCurrentDefaultLog();

            Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@test.com", "test");
            FetchAccount fa = account.FetchAccounts.Add();

            fa.Enabled = true;
            fa.MinutesBetweenFetch = 10;
            fa.Name = "Test";
            fa.Username = "test@example.com";
            fa.Password = "test";
            fa.UseSSL = true;
            fa.ServerAddress = "localhost";
            fa.Port = port;
            fa.ProcessMIMERecipients = false;
            fa.Save();

            fa.DownloadNow();
            string contentSoFar = "";
            for (int i = 0; i <= 10; i++)
            {
               if (i == 5)
                  CustomAssert.Fail("No connection: " + contentSoFar);

               contentSoFar = TestSetup.ReadCurrentDefaultLog();
               if (contentSoFar.Contains("SSL handshake with client failed."))
                  break;

               Thread.Sleep(1000);
            }

            TestSetup.DeleteCurrentDefaultLog();

            // try again.
            fa.DownloadNow();

            contentSoFar = "";

            for (int i = 0; i <= 10; i++)
            {
               if (i == 5)
                  CustomAssert.Fail("No connection: " + contentSoFar);

               contentSoFar = TestSetup.ReadCurrentDefaultLog();
               if (contentSoFar.Contains("SSL handshake with client failed."))
                  break;

               Thread.Sleep(1000);
            }

            pop3Server.WaitForCompletion();

            fa.Delete();
         }
      }

      [Test]
      public void TestBasicExternalAccountSSL()
      {
         var messages = new List<string>();

         string message = "From: Martin@example.com\r\n" +
                          "To: Martin@example.com\r\n" +
                          "Subject: Test\r\n" +
                          "\r\n" +
                          "Hello!";

         messages.Add(message);

         int port = TestSetup.GetNextFreePort();
         using (var pop3Server = new POP3Server(1, port, messages, eConnectionSecurity.eCSTLS))
         {
            pop3Server.SetCertificate(SslSetup.GetCertificate());
            pop3Server.StartListen();

            TestSetup.DeleteCurrentDefaultLog();

            Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@test.com", "test");
            FetchAccount fa = account.FetchAccounts.Add();

            fa.Enabled = true;
            fa.MinutesBetweenFetch = 10;
            fa.Name = "Test";
            fa.Username = "test@example.com";
            fa.Password = "test";
            fa.ConnectionSecurity = eConnectionSecurity.eCSTLS;
            fa.ServerAddress = "localhost";
            fa.Port = port;
            fa.ProcessMIMERecipients = false;
            fa.Save();

            fa.DownloadNow();
            pop3Server.WaitForCompletion();

            POP3Simulator.AssertMessageCount("user@test.com", "test", 1);

            fa.Delete();
         }
      }

      [Test]
      public void TestBasicExternalAccountStartTlsEvenThoughNotSupported()
      {
         var messages = new List<string>();

         string message = "From: Martin@example.com\r\n" +
                          "To: Martin@example.com\r\n" +
                          "Subject: Test\r\n" +
                          "\r\n" +
                          "Hello!";

         messages.Add(message);

         int port = TestSetup.GetNextFreePort();
         using (var pop3Server = new POP3Server(1, port, messages, eConnectionSecurity.eCSNone))
         {
            pop3Server.SetCertificate(SslSetup.GetCertificate());
            pop3Server.StartListen();

            TestSetup.DeleteCurrentDefaultLog();

            Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@test.com", "test");
            FetchAccount fa = account.FetchAccounts.Add();

            fa.Enabled = true;
            fa.MinutesBetweenFetch = 10;
            fa.Name = "Test";
            fa.Username = "test@example.com";
            fa.Password = "test";
            fa.ConnectionSecurity = eConnectionSecurity.eCSSTARTTLS;
            fa.ServerAddress = "localhost";
            fa.Port = port;
            fa.ProcessMIMERecipients = false;
            fa.Save();

            fa.DownloadNow();

            string expectedMessage =
               string.Format(
                  "The download of messages from external account {0} failed. The external aAccount is configured to use STARTTLS connection security, but the POP3 server does not support it.", fa.Name);

            string contentSoFar = "";

            for (int i = 0; i <= 10; i++)
            {
               if (i == 5)
                  CustomAssert.Fail("No connection: " + contentSoFar);

               contentSoFar = TestSetup.ReadCurrentDefaultLog();
               if (contentSoFar.Contains(expectedMessage))
                  break;

               Thread.Sleep(1000);
            }

            pop3Server.WaitForCompletion();

            fa.Delete();
         }
      }

      [Test]
      public void TestBasicExternalAccountStartTls()
      {
         var messages = new List<string>();

         string message = "From: Martin@example.com\r\n" +
                          "To: Martin@example.com\r\n" +
                          "Subject: Test\r\n" +
                          "\r\n" +
                          "Hello!";

         messages.Add(message);

         int port = TestSetup.GetNextFreePort();
         using (var pop3Server = new POP3Server(1, port, messages, eConnectionSecurity.eCSSTARTTLS))
         {
            pop3Server.SetCertificate(SslSetup.GetCertificate());
            pop3Server.StartListen();

            TestSetup.DeleteCurrentDefaultLog();

            Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@test.com", "test");
            FetchAccount fa = account.FetchAccounts.Add();

            fa.Enabled = true;
            fa.MinutesBetweenFetch = 10;
            fa.Name = "Test";
            fa.Username = "test@example.com";
            fa.Password = "test";
            fa.ConnectionSecurity = eConnectionSecurity.eCSSTARTTLS;
            fa.ServerAddress = "localhost";
            fa.Port = port;
            fa.ProcessMIMERecipients = false;
            fa.Save();

            fa.DownloadNow();
            pop3Server.WaitForCompletion();

            POP3Simulator.AssertMessageCount("user@test.com", "test", 1);

            fa.Delete();
         }
      }
   }
}
