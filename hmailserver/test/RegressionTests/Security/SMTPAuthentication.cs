// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System.Collections.Generic;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;

namespace RegressionTests.Security
{
   [TestFixture]
   public class SMTPAuthentication : TestFixtureBase
   {
      [SetUp]
      public new void SetUp()
      {
         _settings.ClearLogonFailureList();
      }

      [Test]
      [Description("Local to local")]
      public void TestBlockingDeliveries()
      {
         var range =
            SingletonProvider<TestSetup>.Instance.GetApp().Settings.SecurityRanges.get_ItemByName("My computer");
         range.RequireSMTPAuthLocalToLocal = false;
         range.RequireSMTPAuthLocalToExternal = false;
         range.RequireSMTPAuthExternalToLocal = false;
         range.RequireSMTPAuthExternalToExternal = false;

         range.AllowDeliveryFromLocalToLocal = false;
         range.AllowDeliveryFromLocalToRemote = false;
         range.AllowDeliveryFromRemoteToLocal = false;
         range.AllowDeliveryFromRemoteToRemote = false;

         range.Save();

         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         var smtpClientSimulator = new SmtpClientSimulator();

         string result1 = "", result2 = "", result3 = "", result4 = "";

         CustomAsserts.Throws<DeliveryFailedException>(() =>
            smtpClientSimulator.Send(account1.Address, account1.Address, "Mail 1", "Mail 1", out result1));
         CustomAsserts.Throws<DeliveryFailedException>(() =>
            smtpClientSimulator.Send(account1.Address, "externaladdress@gmail.com", "Mail 1", "Mail 1", out result2));
         CustomAsserts.Throws<DeliveryFailedException>(() =>
            smtpClientSimulator.Send("externaladdress@gmail.com", account1.Address, "Mail 1", "Mail 1", out result3));
         CustomAsserts.Throws<DeliveryFailedException>(() => smtpClientSimulator.Send("externaladdress@gmail.com",
            "externaladdress@gmail.com", "Mail 1", "Mail 1",
            out result4));

         Assert.IsTrue(result1.Contains("550 Delivery is not allowed to this address."));
         Assert.IsTrue(result2.Contains("550 Delivery is not allowed to this address."));
         Assert.IsTrue(result3.Contains("550 Delivery is not allowed to this address."));
         Assert.IsTrue(result4.Contains("550 Delivery is not allowed to this address."));
      }

      [Test]
      [Description("Test require SMTP auth is not happening if we're trying to deliver from external to external.")]
      public void TestSMTPAuthExternalToExternal()
      {
         var range =
            SingletonProvider<TestSetup>.Instance.GetApp().Settings.SecurityRanges.get_ItemByName("My computer");
         range.RequireSMTPAuthExternalToExternal = true;
         range.Save();

         var smtpClientSimulator = new SmtpClientSimulator();
         var result = "";
         CustomAsserts.Throws<DeliveryFailedException>(() => smtpClientSimulator.Send("externaladdress@example.com",
            "someexternaladdress@example.com", "Mail 1",
            "Mail 1", out result));
         Assert.IsTrue(result.Contains("SMTP authentication is required."));

         range.RequireSMTPAuthExternalToExternal = false;
         range.AllowDeliveryFromRemoteToRemote = false;
         range.Save();

         CustomAsserts.Throws<DeliveryFailedException>(() => smtpClientSimulator.Send("externaladdress@example.com",
            "someexternaladdress@example.com", "Mail 1",
            "Mail 1", out result));
         Assert.IsTrue(result.Contains("550 Delivery is not allowed to this address."));
      }

      [Test]
      [Description("External to local")]
      public void TestSMTPAuthExternalToLocal()
      {
         var range =
            SingletonProvider<TestSetup>.Instance.GetApp().Settings.SecurityRanges.get_ItemByName("My computer");
         range.RequireSMTPAuthExternalToLocal = true;
         range.Save();

         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         var smtpClientSimulator = new SmtpClientSimulator();
         CustomAsserts.Throws<DeliveryFailedException>(() =>
            smtpClientSimulator.Send("someexternaladdress@example.com", account1.Address, "Mail 1", "Mail 1"));

         range.RequireSMTPAuthExternalToLocal = false;
         range.Save();

         smtpClientSimulator.Send("someexternaladdress@example.com", account1.Address, "Mail 1", "Mail 1");
         CustomAsserts.AssertRecipientsInDeliveryQueue(0);
      }

      [Test]
      [Description("External to route configured to be treated as local")]
      public void TestSMTPAuthExternalToRouteConfiguredAsLocal()
      {
         // First, make sure hMailServer requires auth.
         var range =
            SingletonProvider<TestSetup>.Instance.GetApp().Settings.SecurityRanges.get_ItemByName("My computer");
         range.RequireSMTPAuthExternalToExternal = true;
         range.RequireSMTPAuthLocalToExternal = true;
         range.Save();

         var smtpServerPort = TestSetup.GetNextFreePort();
         var route = TestSetup.AddRoutePointingAtLocalhost(1, smtpServerPort, false);
         route.TreatRecipientAsLocalDomain = true;
         route.TreatSenderAsLocalDomain = true;
         route.Save();

         // Set up the simulating server to listen.
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["dummy@dummy-example.com"] = 250;


         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            // Make sure we can't send to this route without using smtp auth.
            var smtpClientSimulator = new SmtpClientSimulator();
            smtpClientSimulator.Send("someexternaladdress@example.com", "dummy@dummy-example.com", "Mail 1", "Mail 1");

            server.WaitForCompletion();

            Assert.IsTrue(server.MessageData.Contains("Mail 1"), server.MessageData);
         }
      }

      [Test]
      [Description("Test require SMTP auth when sending from local address to external address.")]
      public void TestSMTPAuthLocalToExternal()
      {
         var range =
            SingletonProvider<TestSetup>.Instance.GetApp().Settings.SecurityRanges.get_ItemByName("My computer");
         range.RequireSMTPAuthLocalToExternal = true;
         range.Save();

         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         var smtpClientSimulator = new SmtpClientSimulator();
         var result = "";
         CustomAsserts.Throws<DeliveryFailedException>(() => smtpClientSimulator.Send(account1.Address,
            "someexternaladdress@example.com", "Mail 1", "Mail 1",
            out result));
         Assert.IsTrue(result.Contains("SMTP authentication is required"));
      }

      [Test]
      [Description("Local to local")]
      public void TestSMTPAuthLocalToLocal()
      {
         var range =
            SingletonProvider<TestSetup>.Instance.GetApp().Settings.SecurityRanges.get_ItemByName("My computer");
         range.RequireSMTPAuthLocalToLocal = true;
         range.Save();

         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         var smtpClientSimulator = new SmtpClientSimulator();
         var result = "";
         CustomAsserts.Throws<DeliveryFailedException>(() =>
            smtpClientSimulator.Send(account1.Address, account1.Address, "Mail 1", "Mail 1", out result));
         Assert.IsTrue(result.Contains("SMTP authentication is required."));

         range.RequireSMTPAuthLocalToLocal = false;
         range.Save();

         smtpClientSimulator.Send(account1.Address, account1.Address, "Mail 1", "Mail 1", out result);
         CustomAsserts.AssertRecipientsInDeliveryQueue(0);
      }

      [Test]
      [Description(
         "Test option TestSenderAsLocalDomain. Attempt to send a message from a route configured as external domain to a local account account. Should fail, since SMTP auth is required."
      )]
      public void TestSenderAsExternalDomainSendToLocalAccountFail()
      {
         var smtpServerPort = TestSetup.GetNextFreePort();
         var route = TestSetup.AddRoutePointingAtLocalhost(1, smtpServerPort, false);
         route.TreatSenderAsLocalDomain = false;
         route.Save();

         var range =
            SingletonProvider<TestSetup>.Instance.GetApp().Settings.SecurityRanges.get_ItemByName("My computer");
         range.RequireSMTPAuthExternalToLocal = true;
         range.Save();

         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sales@example.test", "test");

         var smtpClientSimulator = new SmtpClientSimulator();
         string result;
         CustomAsserts.Throws<DeliveryFailedException>(() =>
            smtpClientSimulator.Send("someone@dummy-example.com", account1.Address, "Mail 1", "Mail 1", out result));
      }

      [Test]
      [Description(
         "Test option TestSenderAsLocalDomain. Attempt to send a message from a route configured as external domain to a local account account. Should succeed."
      )]
      public void TestSenderAsExternalDomainSendToLocalAccountPass()
      {
         var smtpServerPort = TestSetup.GetNextFreePort();
         var route = TestSetup.AddRoutePointingAtLocalhost(1, smtpServerPort, false);
         route.TreatSenderAsLocalDomain = false;
         route.Save();

         var range =
            SingletonProvider<TestSetup>.Instance.GetApp().Settings.SecurityRanges.get_ItemByName("My computer");
         range.RequireSMTPAuthLocalToLocal = true;
         range.Save();

         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sales@example.test", "test");

         var smtpClientSimulator = new SmtpClientSimulator();
         string result;
         smtpClientSimulator.Send("someone@dummy-example.com", account1.Address, "Mail 1", "Mail 1", out result);

         var text = Pop3ClientSimulator.AssertGetFirstMessageText(account1.Address, "test");
         Assert.IsTrue(text.Contains("Mail 1"));
      }

      [Test]
      [Description(
         "Test option TestSenderAsLocalDomain. Attempt to send a message from a route configured as local domain to an external account. Should fail, since SMTP auth is required."
      )]
      public void TestSenderAsLocalDomainSendToExternal()
      {
         var smtpServerPort = TestSetup.GetNextFreePort();
         var route = TestSetup.AddRoutePointingAtLocalhost(1, smtpServerPort, false);
         route.TreatSenderAsLocalDomain = true;
         route.Save();

         var range =
            SingletonProvider<TestSetup>.Instance.GetApp().Settings.SecurityRanges.get_ItemByName("My computer");
         range.RequireSMTPAuthLocalToExternal = true;
         range.Save();

         var smtpClientSimulator = new SmtpClientSimulator();
         var result = "";
         CustomAsserts.Throws<DeliveryFailedException>(() =>
            smtpClientSimulator.Send("someone@dummy-example.com", "test@example.com", "Mail 1", "Mail 1", out result));
         Assert.IsTrue(result.Contains("530 SMTP authentication is required."));
      }

      [Test]
      [Description(
         "Test option TestSenderAsLocalDomain. Attempt to send a message from a route configured as local domain to a local account account. Should fail, since SMTP auth is required."
      )]
      public void TestSenderAsLocalDomainSendToLocalAccount()
      {
         var smtpServerPort = TestSetup.GetNextFreePort();
         var route = TestSetup.AddRoutePointingAtLocalhost(1, smtpServerPort, false);
         route.TreatSenderAsLocalDomain = true;
         route.Save();

         var range =
            SingletonProvider<TestSetup>.Instance.GetApp().Settings.SecurityRanges.get_ItemByName("My computer");
         range.RequireSMTPAuthLocalToLocal = true;
         range.Save();

         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sales@example.test", "test");

         var smtpClientSimulator = new SmtpClientSimulator();
         var result = "";
         CustomAsserts.Throws<DeliveryFailedException>(() =>
            smtpClientSimulator.Send("someone@dummy-example.com", account1.Address, "Mail 1", "Mail 1", out result));
         Assert.IsTrue(result.Contains("530 SMTP authentication is required."));
      }

      [Test]
      [Description(
         "Test option TestSenderAsLocalDomain. Attempt to send a message from a route configured as local domain to a local account account. Should succeed."
      )]
      public void TestSenderAsLocalDomainSendToLocalAccountPass()
      {
         var smtpServerPort = TestSetup.GetNextFreePort();
         var route = TestSetup.AddRoutePointingAtLocalhost(1, smtpServerPort, false);
         route.TreatSenderAsLocalDomain = true;
         route.Save();

         var range =
            SingletonProvider<TestSetup>.Instance.GetApp().Settings.SecurityRanges.get_ItemByName("My computer");
         range.RequireSMTPAuthLocalToLocal = false;
         range.Save();

         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sales@example.test", "test");

         var smtpClientSimulator = new SmtpClientSimulator();
         string result;
         smtpClientSimulator.Send("someone@dummy-example.com", account1.Address, "Mail 1", "Mail 1", out result);

         var text = Pop3ClientSimulator.AssertGetFirstMessageText(account1.Address, "test");
         Assert.IsTrue(text.Contains("Mail 1"));
      }


      [Test]
      [Description("Use case 3: Delivery from external user to route (backup MX case).")]
      public void TestUseCase3DeliveryFromExternalUserToPrimaryViaBackup()
      {
         var smtpServerPort = TestSetup.GetNextFreePort();
         var route = TestSetup.AddRoutePointingAtLocalhost(1, smtpServerPort, false);
         route.TreatRecipientAsLocalDomain = true;
         route.TreatSenderAsLocalDomain = false;
         route.Save();

         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@dummy-example.com"] = 250;

         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            var smtpClientSimulator = new SmtpClientSimulator();
            string result;
            smtpClientSimulator.Send("someone@example.com", "test@dummy-example.com", "Mail 1", "Mail 1", out result);

            server.WaitForCompletion();

            server.MessageData.Contains("Mail 1");
         }
      }

      [Test]
      [Description(
         "Use case 2: Delivery from primary MX to backup server. For example someone@customer to someone@hoster.")]
      public void TestUseCaseDeliveryFromPrimaryMXToBackupMX()
      {
         var smtpServerPort = TestSetup.GetNextFreePort();
         var route = TestSetup.AddRoutePointingAtLocalhost(1, smtpServerPort, false);
         route.TreatRecipientAsLocalDomain = true;
         route.TreatSenderAsLocalDomain = false;
         route.Save();

         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sales@example.test", "test");

         var smtpClientSimulator = new SmtpClientSimulator();
         string result;
         smtpClientSimulator.Send("someone@dummy-example.com", account1.Address, "Mail 1", "Mail 1", out result);

         var text = Pop3ClientSimulator.AssertGetFirstMessageText(account1.Address, "test");
         Assert.IsTrue(text.Contains("Mail 1"));
      }

      [Test]
      [Description(
         "Use case 1: Delivery from internal users to a route configured as local. SMTP-auth should not be required."
      )]
      public void TestUseCaseDeliveryToLocalRoute()
      {
         var smtpServerPort = TestSetup.GetNextFreePort();
         var route = TestSetup.AddRoutePointingAtLocalhost(1, smtpServerPort, false);
         route.TreatRecipientAsLocalDomain = true;
         route.TreatSenderAsLocalDomain = false;
         route.Save();

         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@dummy-example.com"] = 250;

         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            var smtpClientSimulator = new SmtpClientSimulator();
            string result;
            smtpClientSimulator.Send("someone@dummy-example.com", "test@dummy-example.com", "Mail 1", "Mail 1",
               out result);

            server.WaitForCompletion();

            server.MessageData.Contains("Mail 1");
         }
      }


      [Test]
      [Description(
         "Test option TreatRecipientAsLocalDomain. Attempt to send message from external account to route configured as external. Should fail."
      )]
      public void TreatRecipientAsExternalDomain()
      {
         var smtpServerPort = TestSetup.GetNextFreePort();
         var route = TestSetup.AddRoutePointingAtLocalhost(1, smtpServerPort, false);
         route.TreatRecipientAsLocalDomain = false;
         route.Save();

         var smtpClientSimulator = new SmtpClientSimulator();
         var result = "";
         CustomAsserts.Throws<DeliveryFailedException>(() =>
            smtpClientSimulator.Send("someone@example.com", "test@dummy-example.com", "Mail 1", "Mail 1", out result));
         Assert.IsTrue(result.Contains("530 SMTP authentication is required."));
      }


      [Test]
      [Description(
         "Test option TreatRecipientAsLocalDomain. Attempt to send message from external account to route configured as external. Should succeed, since it's permitted by IP range."
      )]
      public void TreatRecipientAsExternalDomainPermitted()
      {
         var smtpServerPort = TestSetup.GetNextFreePort();
         var route = TestSetup.AddRoutePointingAtLocalhost(1, smtpServerPort, false);
         route.TreatRecipientAsLocalDomain = false;
         route.Save();

         var range =
            SingletonProvider<TestSetup>.Instance.GetApp().Settings.SecurityRanges.get_ItemByName("My computer");
         range.AllowDeliveryFromRemoteToRemote = true;
         range.RequireSMTPAuthExternalToExternal = false;
         range.Save();

         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@dummy-example.com"] = 250;

         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            var smtpClientSimulator = new SmtpClientSimulator();
            string result;
            smtpClientSimulator.Send("someone@example.com", "test@dummy-example.com", "Mail 1", "Mail 1", out result);

            server.WaitForCompletion();

            server.MessageData.Contains("Mail 1");
         }
      }

      [Test]
      [Description(
         "Test option TreatRecipientAsLocalDomain. Attempt to send message from external account to route configured as local. Should succeed."
      )]
      public void TreatRecipientAsLocalDomain()
      {
         var smtpServerPort = TestSetup.GetNextFreePort();
         var route = TestSetup.AddRoutePointingAtLocalhost(1, smtpServerPort, false);
         route.TreatRecipientAsLocalDomain = true;
         route.Save();

         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@dummy-example.com"] = 250;

         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            var smtpClientSimulator = new SmtpClientSimulator();
            string result;
            smtpClientSimulator.Send("someone@example.com", "test@dummy-example.com", "Mail 1", "Mail 1", out result);

            server.WaitForCompletion();

            server.MessageData.Contains("Mail 1");
         }
      }
   }
}