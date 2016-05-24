using System;
using System.Collections.Generic;
using System.Text;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.SMTP
{
   [TestFixture]
   class RcptToParsing : TestFixtureBase
   {
      [Test]
      public void RcptToValidAddressShouldSucceed()
      {
         AssertValidMailRcptToCommand("RCPT TO: example@example.com");
      }

      [Test]
      public void RcptToDomainWithDotShouldSucceed()
      {
         AssertValidMailRcptToCommand("RCPT TO: example@exa.mple.com");
      }

      [Test]
      public void RcptToMailboxWithSpecialCharacters()
      {
         AssertValidMailRcptToCommand("RCPT TO: !#$%&'*+-/=?^_`.{|}~@example.com");
      }

      [Test]
      public void RcptToExample1ShouldSucceed()
      {
         AssertValidMailRcptToCommand("RCPT TO: user+mailbox@example.com");
      }

      [Test]
      public void RcptToExample2ShouldSucceed()
      {
         AssertValidMailRcptToCommand("RCPT TO: customer/department=shipping@example.com");
      }

      [Test]
      public void RcptToExample3ShouldSucceed()
      {
         AssertValidMailRcptToCommand("RCPT TO: $A12345@example.com");
      }

      [Test]
      public void RcptToExample4ShouldSucceed()
      {
         AssertValidMailRcptToCommand("RCPT TO: !def!xyz%abc@example.com");
      }

      [Test]
      public void RcptToExample5ShouldSucceed()
      {
         AssertValidMailRcptToCommand("RCPT TO: _somename@example.com");
      }

      [Test]
      public void RcptToDomainWithSpecialCharactersShouldFail()
      {
         AssertInvalidRcptToCommand("RCPT TO: example@!#$%&'*+-/=?^_`.{|}~.com", "550 A valid address is required.");
      }

      [Test]
      public void RcptToEmptyAddressShouldFail()
      {
         AssertInvalidRcptToCommand("RCPT TO: <>", "550 A valid address is required.");
      }
      
      [Test]
      public void RcptToQuotedAddressWithSpaceShouldSucceed()
      {
         AssertValidMailRcptToCommand("RCPT TO: <\"John Smith\"@example.com>");
      }

      [Test]
      public void RcptToQuotedAddressWithSpaceWithoutGtLtShouldSucceed()
      {
         AssertValidMailRcptToCommand("RCPT TO: \"John Smith\"@example.com");
      }

      [Test]
      public void RcptToQuotedAddressWithoutSpaceShouldFail()
      {
         AssertInvalidRcptToCommand("RCPT TO: <John Smith@example.com>", "550 A valid address is required.");
      }

      [Test]
      public void RcptToSingleQuoteShouldFail()
      {
         AssertInvalidRcptToCommand("RCPT TO: \"", "550 A valid address is required.");
      }

      [Test]
      public void RcptToWithForwardSlashShouldSucceed()
      {
         AssertValidMailRcptToCommand("RCPT TO: <example/example@example.com>");
      }

      [Test]
      public void RcptToWithBackwardSlashShouldFail()
      {
         AssertInvalidRcptToCommand("RCPT TO: <example\\example@example.com>", "550 A valid address is required.");
      }


      [Test]
      public void RcptToWithSingleGreaterThanShouldFail()
      {
         AssertInvalidRcptToCommand("RCPT TO: <example@example.com", "550 Invalid syntax. Syntax should be RCPT TO:<mailbox@domain>[crlf]");
      }

      [Test]
      public void RcptToWithAdditionalParametersShouldFail()
      {
         AssertInvalidRcptToCommand("RCPT TO: <example@example.com> KEY=VALUE", "550 Invalid syntax. Syntax should be RCPT TO:<mailbox@domain>[crlf]");
      }

      [Test]
      public void RcptToWithDomainStartingWithDashShouldFail()
      {
         AssertInvalidRcptToCommand("RCPT TO: <user@-startswithdash.com>", "550 A valid address is required.");
      }

      [Test]
      public void RcptToWithDomainStartingWithDashWithoutTldShouldFail()
      {
         AssertInvalidRcptToCommand("RCPT TO: <user@-startswithdash>", "550 A valid address is required.");
      }

      [Test]
      public void RcptToWithTopDomainStartingWithDashShouldFail()
      {
         AssertInvalidRcptToCommand("RCPT TO: <user@domain.-startswithdash>", "550 A valid address is required.");
      }

      [Test]
      public void RcptToWithLastCharacterDashShouldFail()
      {
         AssertInvalidRcptToCommand("RCPT TO: <user@domain.lastcharacterdash->", "550 A valid address is required.");
      }

      [Test]
      public void RcptToWithDomainStartingWithDotShouldFail()
      {
         AssertInvalidRcptToCommand("RCPT TO: <user@.startswithdot.com>", "550 A valid address is required.");
      }

      [Test]
      public void RcptToWithDomainEndingWithDotShouldFail()
      {
         AssertInvalidRcptToCommand("RCPT TO: <user@endswithdot.>", "550 A valid address is required.");
      }

      [Test]
      public void RcptToWithDomainWithDoubleDotShouldFail()
      {
         AssertInvalidRcptToCommand("RCPT TO: <user@double..dot>", "550 A valid address is required.");
      }

      [Test]
      public void RcptToWithDoubleAtShouldFail()
      {
         AssertInvalidRcptToCommand("RCPT TO: <user@@doubleAT.com>", "550 A valid address is required.");
      }


      private void AssertInvalidRcptToCommand(string command, string expectedResponse)
      {
         var smtpClientSimulator = new TcpConnection();
         smtpClientSimulator.Connect(25);
         Assert.IsTrue(smtpClientSimulator.Receive().StartsWith("220"));
         smtpClientSimulator.Send("HELO test\r\n");
         Assert.IsTrue(smtpClientSimulator.Receive().StartsWith("250"));
         smtpClientSimulator.Send("MAIL FROM: <test@test.com>\r\n");
         Assert.IsTrue(smtpClientSimulator.Receive().StartsWith("250"));

         string result = smtpClientSimulator.SendAndReceive(command+ "\r\n");

         smtpClientSimulator.Disconnect();

         Assert.AreEqual(expectedResponse + "\r\n", result);
      }

      private void AssertValidMailRcptToCommand(string comamnd)
      {
         var smtpClientSimulator = new TcpConnection();
         smtpClientSimulator.Connect(25);
         Assert.IsTrue(smtpClientSimulator.Receive().StartsWith("220"));
         smtpClientSimulator.Send("HELO test\r\n");
         Assert.IsTrue(smtpClientSimulator.Receive().StartsWith("250"));
         smtpClientSimulator.Send("MAIL FROM: <test@test.com>\r\n");
         Assert.IsTrue(smtpClientSimulator.Receive().StartsWith("250"));

         string result = smtpClientSimulator.SendAndReceive(comamnd + "\r\n");

         smtpClientSimulator.Disconnect();

         Assert.AreEqual("250 OK\r\n", result);
      }

   }
}
