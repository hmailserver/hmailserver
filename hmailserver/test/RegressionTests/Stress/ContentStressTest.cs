// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.IO;
using System.Net.Sockets;
using System.Text;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.Stress
{
   [TestFixture]
   public class ContentStressTest : TestFixtureBase
   {
      [Test]
      public void TestLongLineInData()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         var sb = new StringBuilder();
         for (int i = 0; i < 11000; i++)
         {
            sb.Append("1234567890");
         }

         var sim = new SmtpClientSimulator();
         CustomAsserts.Throws<DeliveryFailedException>(() => sim.SendRaw("test@test.com", "test@test.com", sb.ToString()));
      }


      /// <summary>
      ///    RFC 2683, 3.2.1.5. Long Command Lines
      ///       A client should limit the length of the command lines it generates to
      ///       approximately 1000 octets (including all quoted strings but not
      ///       including literals).  If the client is unable to group things into
      ///       ranges so that the command line is within that length, it should
      ///       split the request into multiple commands.  The client should use
      ///       literals instead of long quoted strings, in order to keep the command
      ///       length down.
      ///
      ///       For its part, a server should allow for a command line of at least
      ///       8000 octets.  This provides plenty of leeway for accepting reasonable
      ///       length commands from clients.  The server should send a BAD response
      ///       to a command that does not end within the server's maximum accepted
      ///       command length.
      /// </summary>
      [Test]
      public void TestExcessiveDataInIMAPConversation()
      {
         var sb = new StringBuilder();
         for (int i = 0; i < 100000; i++)
         {
            sb.Append("1234567890");
         }

         sb.Append(".com\r\n");

         string command = "A03 NOOP " + sb;

         var socket = new TcpConnection();
         Assert.IsTrue(socket.Connect(143));
         socket.Receive();
         socket.Send(command);

         try
         {
            string response = socket.Receive();
            Assert.IsTrue(response.StartsWith("* BYE"));
         }
         catch (System.IO.IOException ex)
         {
            AssertIsConnectionTerminatedException(ex);
         }


         socket.Disconnect();
      }

      [Test]
      public void TestExcessiveDataInPOP3Conversation()
      {
         var sb = new StringBuilder();
         for (int i = 0; i < 100000; i++)
         {
            sb.Append("1234567890");
         }
                
         
         string command = "HELP " + sb;

         var socket = new TcpConnection();
         Assert.IsTrue(socket.Connect(110));
         socket.Receive();
         socket.Send(command + "\r\n");

         try
         {
            string response = socket.Receive();
            Assert.IsTrue(response.StartsWith("-ERR"));

            socket.Disconnect();
         }
         catch (IOException ex)
         {
            AssertIsConnectionTerminatedException(ex);
         }
      }


      [Test]
      public void TestLongLineInPOP3Conversation()
      {
         var sb = new StringBuilder();
         for (int i = 0; i < 400; i++)
         {
            sb.Append("1234567890");
         }

         sb.Append(".com");

         string command = "NOOP " + sb;

         var socket = new TcpConnection();
         Assert.IsTrue(socket.Connect(110));
         socket.Receive();
         socket.Send(command + "\r\n");

         string response = socket.Receive();
         Assert.IsTrue(response.StartsWith("-ERR Line to long."));

         socket.Disconnect();
      }

      [Test]
      public void TestExcessiveDataInSMTPConversation()
      {
         var sb = new StringBuilder();
         for (int i = 0; i < 100000; i++)
         {
            sb.Append("1234567890");
         }

         sb.Append(".com");

         string command = "HELO " + sb;

         var socket = new TcpConnection();
         Assert.IsTrue(socket.Connect(25));
         socket.Receive();
         socket.Send(command + "\r\n");

         try
         {
            string response = socket.Receive();
            Assert.IsTrue(response.StartsWith("421"));

            socket.Disconnect();
         }
         catch (IOException ex)
         {
            AssertIsConnectionTerminatedException(ex);
         }
      }

      [Test]
      public void TestLongLineInSMTPConversation()
      {
         var sb = new StringBuilder();
         for (int i = 0; i < 400; i++)
         {
            sb.Append("1234567890");
         }

         sb.Append(".com");

         string command = "HELO " + sb;

         var socket = new TcpConnection();
         Assert.IsTrue(socket.Connect(25));
         socket.Receive();
         socket.Send(command + "\r\n");

         string response = socket.Receive();
         Assert.IsTrue(response.StartsWith("500"));

         socket.Disconnect();
      }

      /// <summary>
      /// Test to send a message where the MIME part boundaries overlap.
      /// i.e. one boundary string is a part of another boundary string.
      /// </summary>
      [Test]
      public void TestOverlappingBoundaryNames()
      {
         string content = "Return-Path: <check@test.com>\r\n" +
                          "From: \"test\" <check@test.com>\r\n" +
                          "To: \"Test\" <test@test.com>\r\n" +
                          "Subject: rtest\r\n" +
                          "Date: Thu, 22 Jan 2009 13:20:32 +0100\r\n" +
                          "MIME-Version: 1.0\r\n" +
                          "Content-Type: multipart/mixed;\r\n" +
                          "    boundary=\"----=_NextPart_000_000D_01C97C94.33D5E670\"\r\n" +
                          "\r\n" +
                          "This is a multi-part message in MIME format.\r\n" +
                          "\r\n" +
                          "------=_NextPart_000_000D_01C97C94.33D5E670\r\n" +
                          "Content-Type: multipart/alternative;\r\n" +
                          "    boundary=\"----=_NextPart_000_000D_01C97C94.33D5E670.ALT\"\r\n" +
                          "\r\n" +
                          "\r\n" +
                          "------=_NextPart_000_000D_01C97C94.33D5E670.ALT\r\n" +
                          "Content-Type: text/plain;\r\n" +
                          "    charset=\"iso-8859-1\"\r\n" +
                          "Content-Transfer-Encoding: quoted-printable\r\n" +
                          "\r\n" +
                          "test\r\n" +
                          "------=_NextPart_000_000D_01C97C94.33D5E670.ALT\r\n" +
                          "Content-Type: text/html;\r\n" +
                          "    charset=\"iso-8859-1\"\r\n" +
                          "Content-Transfer-Encoding: quoted-printable\r\n" +
                          "\r\n" +
                          "<a>test</a>\r\n" +
                          "\r\n" +
                          "------=_NextPart_000_000D_01C97C94.33D5E670.ALT--\r\n" +
                          "\r\n" +
                          "------=_NextPart_000_000D_01C97C94.33D5E670\r\n" +
                          "Content-Type: application/octet-stream;\r\n" +
                          "    name=\"test.vbs\"\r\n" +
                          "Content-Transfer-Encoding: 7bit\r\n" +
                          "Content-Disposition: attachment;\r\n" +
                          "    filename=\"test.vbs\"\r\n" +
                          "\r\n" +
                          "s = \"abc\"\r\n" +
                          "\r\n" +
                          "msgbox mid(s,1,100000)\r\n" +
                          "------=_NextPart_000_000D_01C97C94.33D5E670--\r\n" +
                          "\r\n" +
                          "";


         // Add an account
         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "mimetest@test.com", "test");

         Rule oRule = oAccount.Rules.Add();
         oRule.Name = "Criteria test";
         oRule.Active = true;

         RuleCriteria oRuleCriteria = oRule.Criterias.Add();
         oRuleCriteria.UsePredefined = false;
         oRuleCriteria.HeaderField = "Subject";
         oRuleCriteria.MatchType = eRuleMatchType.eMTContains;
         oRuleCriteria.MatchValue = "rtest";
         oRuleCriteria.Save();

         // Add action
         RuleAction oRuleAction = oRule.Actions.Add();
         oRuleAction.Type = eRuleActionType.eRASetHeaderValue;
         oRuleAction.HeaderName = "SomeHeader";
         oRuleAction.Value = "SomeValue";
         oRuleAction.Save();

         // Save the rule in the database
         oRule.Save();

         var smtpClientSimulator = new SmtpClientSimulator();

         // Spam folder
         smtpClientSimulator.SendRaw("mimetest@test.com", "mimetest@test.com", content);

         string sContents = Pop3ClientSimulator.AssertGetFirstMessageText("mimetest@test.com", "test");

         Assert.IsTrue(sContents.IndexOf("SomeHeader: SomeValue") > 0);
         Assert.IsTrue(sContents.IndexOf("------=_NextPart_000_000D_01C97C94.33D5E670.ALT--") > 0);
      }

      private void AssertIsConnectionTerminatedException(IOException exception)
      {
         var inner = exception.InnerException as SocketException;
         Assert.IsNotNull(inner);
     

         Assert.AreEqual(10054, inner.ErrorCode);
      }
   }
}