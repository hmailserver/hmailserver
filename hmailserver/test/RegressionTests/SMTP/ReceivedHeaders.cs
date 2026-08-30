// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Threading;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Shared;
using RegressionTests.SSL;

namespace RegressionTests.SMTP
{
   [TestFixture]
   public class ReceivedHeaders : TestFixtureBase
   {
      [OneTimeSetUp]
      public new void TestFixtureSetUp()
      {
         SslSetup.SetupSSLPorts(_application);
      }

      [SetUp]
      public new void SetUp()
      {
         _account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
      }

      private Account _account;


      [Test]
      [Description("Header should contain ESMTPA if authenticated.")]
      public void TestESMTPAInHeader()
      {
         string errorMessage;

         var client = new SmtpClientSimulator();
         client.Send(false, _account.Address, "test", _account.Address, _account.Address, "Test", "Test",
            out errorMessage);

         var message = Pop3ClientSimulator.AssertGetFirstMessageText(_account.Address, "test");

         Assert.IsTrue(message.Contains(" with ESMTPA"));
      }

      [Test]
      [Description("Header should contain ESMTPS if STARTTLS is used.")]
      public void TestESMTPSInHeader()
      {
         var smtpClientSimulator = new SmtpClientSimulator(false, 25002);

         string errorMessage;
         smtpClientSimulator.Send(true, string.Empty, string.Empty, _account.Address, _account.Address, "Test", "test",
            out errorMessage);

         var message = Pop3ClientSimulator.AssertGetFirstMessageText(_account.Address, "test");
         Assert.IsTrue(message.Contains(" with ESMTPS"));
      }

      [Test]
      [Description("Header should contain ESMTPSA if STARTTLS is used and user is authenticated.")]
      public void TestESMTPSAInHeader()
      {
         try
         {
            var smtpClientSimulator = new SmtpClientSimulator(false, 25002);

            string errorMessage;
            smtpClientSimulator.Send(true, _account.Address, "test", _account.Address, _account.Address, "Test", "test",
               out errorMessage);

            var message = Pop3ClientSimulator.AssertGetFirstMessageText(_account.Address, "test");
            Assert.IsTrue(message.Contains(" with ESMTPSA"));
         }
         catch (Exception e)
         {
            Assert.Fail(e.ToString());
         }
      }

      [Test]
      [Description("Received header should include cipher information if SSL is used.")]
      public void TestCipherInfoInReceivedHeader()
      {
         try
         {
            var smtpClientSimulator = new SmtpClientSimulator(false, 25002);

            string errorMessage;
            smtpClientSimulator.Send(true, _account.Address, "test", _account.Address, _account.Address, "Test", "test",
               out errorMessage);

            var message = Pop3ClientSimulator.AssertGetFirstMessageText(_account.Address, "test");
            Assert.IsTrue(message.Contains("version=TLS"));
            Assert.IsTrue(message.Contains("cipher="));
            Assert.IsTrue(message.Contains("bits="));
         }
         catch (Exception e)
         {
            Assert.Fail(e.ToString());
         }
      }

      [Test]
      [Description("Received header should NOT include cipher information if SSL is NOT used.")]
      public void TestMissingCipherInfoInReceivedHeader()
      {
         try
         {
            var smtpClientSimulator = new SmtpClientSimulator(false, 25);

            string errorMessage;
            smtpClientSimulator.Send(false, _account.Address, "test", _account.Address, _account.Address, "Test",
               "test",
               out errorMessage);

            var message = Pop3ClientSimulator.AssertGetFirstMessageText(_account.Address, "test");
            Assert.IsFalse(message.Contains("cipher\r\n"));
         }
         catch (Exception e)
         {
            Assert.Fail(e.ToString());
         }
      }
   }
}