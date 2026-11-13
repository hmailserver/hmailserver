using System.Text;
using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.MIME
{
   [TestFixture]
   public class MessageParsing : TestFixtureBase
   {
      [Test]
      [Description("Test to parse a multi-part message with no text in the main body directly after the header.")]
      public void TestParseMultipartNoBody()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "search@example.test", "test");
         var body = TestSetup.GetResource("Messages.MultipartMessageWithNoMainBodyText.txt");
         SmtpClientSimulator.StaticSendRaw(account.Address, account.Address, body);

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

         var imapSim = new ImapClientSimulator("search@example.test", "test", "INBOX");
         var result = imapSim.Fetch("1 (BODY.PEEK[HEADER] BODY.PEEK[TEXT])");

         imapSim.Logout();
      }

      [Test]
      public void TestParseDeeplyNestedMultipart()
      {
         const int nestingDepth = 10000;

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         var message = new StringBuilder();
         message.Append("From: test@example.test\r\n");
         message.Append("Subject: Nested\r\n");
         message.Append("MIME-Version: 1.0\r\n");

         for (int i = 0; i < nestingDepth; i++)
         {
            message.AppendFormat("Content-Type: multipart/mixed; boundary=\"b{0}\"\r\n", i);
            message.Append("\r\n");
            message.AppendFormat("--b{0}\r\n", i);
         }

         message.Append("Content-Type: text/plain\r\n");
         message.Append("\r\n");
         message.Append("Innermost\r\n");

         for (int i = nestingDepth - 1; i >= 0; i--)
            message.AppendFormat("--b{0}--\r\n", i);

         SmtpClientSimulator.StaticSendRaw(account.Address, account.Address, message.ToString());

         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 1);

         var sim = new ImapClientSimulator(account.Address, "test", "Inbox");
         var result = sim.Fetch("1 BODYSTRUCTURE");
         sim.Disconnect();

         StringAssert.Contains("BODYSTRUCTURE", result);

         // The server must still be running after parsing the message.
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 1);
      }
   }
}