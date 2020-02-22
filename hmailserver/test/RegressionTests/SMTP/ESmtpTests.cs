using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.SMTP
{
   [TestFixture]
   public class ESmtpTests : TestFixtureBase
   {
      [Test]
      public void UnsupportedParameterInMailFrom()
      {
         var smtpConn = new SmtpClientSimulator();
         smtpConn.Connect();

         smtpConn.Receive();

         smtpConn.SendAndReceive("EHLO example.com\r\n");

         var response = smtpConn.SendAndReceive("MAIL FROM: example@example.com A=B\r\n");
         Assert.AreEqual("550 Unsupported ESMTP extension: A=B\r\n", response);
      }

      [Test]
      public void UnsupportedParameterInRcptTo()
      {
         var smtpConn = new SmtpClientSimulator();
         smtpConn.Connect();
         
         smtpConn.Receive();

         var A = smtpConn.SendAndReceive("EHLO example.com\r\n");
         var b = smtpConn.SendAndReceive("MAIL FROM: example@example.com\r\n");

         var response = smtpConn.SendAndReceive("RCPT TO: example@example.com A=B\r\n");
         Assert.AreEqual("550 Unsupported ESMTP extension: A=B\r\n", response);
      }
   }
}
