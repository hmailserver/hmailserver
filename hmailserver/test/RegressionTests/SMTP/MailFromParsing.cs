using System;
using System.Collections.Generic;
using System.Text;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.SMTP
{
   [TestFixture]
   class MailFromParsing : TestFixtureBase
   {
      [Test]
      public void MailFromValidAddressShouldSucceed()
      {
         AssertValidMailFromCommand("MAIL FROM: example@example.com");
      }

      [Test]
      public void MailFromMailboxWithSpecialCharacters()
      {
         AssertValidMailFromCommand("MAIL FROM: !#$%&'*+-/=?^_`.{|}~@example.com");
      }

      [Test]
      public void MailFromDomainWithSpecialCharactersShouldFail()
      {
         AssertInvalidMailFromCommand("MAIL FROM: example@!#$%&'*+-/=?^_`.{|}~.com", "550 The address is not valid.");
      }


      [Test]
      public void MailFromExample1ShouldSucceed()
      {
         AssertValidMailFromCommand("MAIL FROM: user+mailbox@example.com");
      }

      [Test]
      public void MailFromExample2ShouldSucceed()
      {
         AssertValidMailFromCommand("MAIL FROM: customer/department=shipping@example.com");
      }

      [Test]
      public void MailFromExample3ShouldSucceed()
      {
         AssertValidMailFromCommand("MAIL FROM: $A12345@example.com");
      }

      [Test]
      public void MailFromExample4ShouldSucceed()
      {
         AssertValidMailFromCommand("MAIL FROM: !def!xyz%abc@example.com");
      }

      [Test]
      public void MailFromExample5ShouldSucceed()
      {
         AssertValidMailFromCommand("MAIL FROM: _somename@example.com");
      }


      [Test]
      public void MailFromDomainWithDotShouldSucceed()
      {
         AssertValidMailFromCommand("MAIL FROM: example@exa.mple.com");
      }

      [Test]
      public void MailFromEmptyAddressShouldSucceed()
      {
         AssertValidMailFromCommand("MAIL FROM: <>");
      }

      [Test]
      public void MailFromEmptyAddressWithParameterShouldSucceed()
      {
         AssertValidMailFromCommand("MAIL FROM: <> AUTH=<>");
      }

      [Test]
      public void MailFromUnquotedAddressWithSpaceShouldFail()
      {
         AssertInvalidMailFromCommand("MAIL FROM: <John Smith@example.com>", "550 Invalid syntax. Syntax should be MAIL FROM:<mailbox@domain>[crlf]");
      }

      [Test]
      public void MailFromWithForwardSlashShouldSucceed()
      {
         AssertValidMailFromCommand("MAIL FROM: <example/example@example.com>");
      }

      [Test]
      public void MailFromWithBackwardSlashShouldFail()
      {
         AssertInvalidMailFromCommand("MAIL FROM: <example\\example@example.com>", "550 The address is not valid.");
      }

      [Test]
      public void MailFromQuotedAddressWithSpaceWithoutGtLtShouldSucceed()
      {
         AssertValidMailFromCommand("MAIL FROM: \"John Smith\"@example.com");
      }

      [Test]
      public void MailFromQuotedAddressWithSpaceWithExtensionsWithoutGtLtShouldSucceed()
      {
         AssertValidMailFromCommand("MAIL FROM: \"John Smith\"@example.com AUTH=<>");
      }

      [Test]
      public void MailFromQuotedAddressWithSpaceShouldSucceed()
      {
         AssertValidMailFromCommand("MAIL FROM: <\"John Smith\"@example.com>");
      }

      [Test]
      public void MailFromQuotedAddressWithSpaceAndWithParametersShouldSucceed()
      {
         AssertValidMailFromCommand("MAIL FROM: <\"John Smith\"@example.com> AUTH=<>");
      }

      [Test]
      public void MailFromSingleQuoteShouldFail()
      {
         AssertInvalidMailFromCommand("MAIL FROM: \"", "550 Invalid syntax. Syntax should be MAIL FROM:<mailbox@domain>[crlf]");
      }

      [Test]
      public void MailWithSingleGreaterThanShouldFail()
      {
         AssertInvalidMailFromCommand("MAIL FROM: <example@example.com", "550 Invalid syntax. Syntax should be MAIL FROM:<mailbox@domain>[crlf]");
      }

      [Test]
      public void MailWithSingleGreaterThanAndParametersShouldFail()
      {
         AssertInvalidMailFromCommand("MAIL FROM: <example@example.com AUTH=<>", "550 Invalid syntax. Syntax should be MAIL FROM:<mailbox@domain>[crlf]");
      }

      [Test]
      public void MailWithSingleGreaterThanAndQuotedFromAndParametersShouldFail()
      {
         AssertInvalidMailFromCommand("MAIL FROM: <\"John Smith\"@example.com AUTH=<>", "550 Invalid syntax. Syntax should be MAIL FROM:<mailbox@domain>[crlf]");
      }


      private void AssertInvalidMailFromCommand(string command, string expectedResponse)
      {
         var smtpClientSimulator = new TcpConnection();
         smtpClientSimulator.Connect(25);
         Assert.IsTrue(smtpClientSimulator.Receive().StartsWith("220"));
         smtpClientSimulator.Send("HELO test\r\n");
         Assert.IsTrue(smtpClientSimulator.Receive().StartsWith("250"));

         string result = smtpClientSimulator.SendAndReceive(command+ "\r\n");


         smtpClientSimulator.Disconnect();

         Assert.AreEqual(expectedResponse + "\r\n", result);
      }

      private void AssertValidMailFromCommand(string comamnd)
      {
         var smtpClientSimulator = new TcpConnection();
         smtpClientSimulator.Connect(25);
         Assert.IsTrue(smtpClientSimulator.Receive().StartsWith("220"));
         smtpClientSimulator.Send("HELO test\r\n");
         Assert.IsTrue(smtpClientSimulator.Receive().StartsWith("250"));

         string result = smtpClientSimulator.SendAndReceive(comamnd + "\r\n");

         smtpClientSimulator.Disconnect();

         Assert.AreEqual("250 OK\r\n", result);
      }

   }
}
