// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Threading;
using NUnit.Framework;
using RegressionTests.Shared;
using RegressionTests.SSL;

namespace RegressionTests.SMTP
{
   [TestFixture]
   public class ReceivedHeaders : TestFixtureBase
   {
      private hMailServer.Account _account;
      [TestFixtureSetUp]
      public new void TestFixtureSetUp()
      {
         SslSetup.SetupSSLPorts(_application);

         Thread.Sleep(1000);
      }

      [SetUp]
      public new void SetUp()
      {
         _account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
      }


      [Test]
      [Description("Header should contain ESMTPA if authenticated.")]
      public void TestESMTPAInHeader()
      {
         string errorMessage;

         var client = new SMTPClientSimulator();
         client.Send(false, _account.Address, "test", _account.Address, _account.Address, "Test", "Test", out errorMessage);

         var message = POP3ClientSimulator.AssertGetFirstMessageText(_account.Address, "test");

         CustomAssert.IsTrue(message.Contains("ESMTPA\r\n"));
      }

      [Test]
      [Description("Header should contain ESMTPS if STARTTLS is used.")]
      public void TestESMTPSInHeader()
      {
         var smtpClientSimulator = new SMTPClientSimulator(false, 25002);

         string errorMessage;
         smtpClientSimulator.Send(true, string.Empty, string.Empty, _account.Address, _account.Address, "Test", "test", out errorMessage);

         var message = POP3ClientSimulator.AssertGetFirstMessageText(_account.Address, "test");
         CustomAssert.IsTrue(message.Contains("ESMTPS\r\n"));
      }

      [Test]
      [Description("Header should contain ESMTPSA if STARTTLS is used and user is authenticated.")]
      public void TestESMTPSAInHeader()
      {
         try
         {
            var smtpClientSimulator = new SMTPClientSimulator(false, 25002);

            string errorMessage;
            smtpClientSimulator.Send(true, _account.Address, "test", _account.Address, _account.Address, "Test", "test",
               out errorMessage);

            var message = POP3ClientSimulator.AssertGetFirstMessageText(_account.Address, "test");
            CustomAssert.IsTrue(message.Contains("ESMTPSA\r\n"));
         }
         catch (Exception e)
         {
            CustomAssert.Fail(e.ToString());
         }
      }

   }
}