using System.Collections.Generic;
using hMailServer;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;

namespace RegressionTests.SMTP.SRS
{
   /// <summary>
   /// Shared setup for the SRS tests: a domain we host, a domain we do not, and a server
   /// standing in for the one at the other end of a forward.
   /// </summary>
   public class SrsTestBase : TestFixtureBase
   {
      protected const string ExternalDomain = "external.test";
      protected const string ExternalSender = "sender@external.test";
      protected const string ForwardTarget = "recipient@external.test";

      protected string Secret => _settings.SRSSecret;

      protected void EnableSrs()
      {
         _settings.SRSEnabled = true;
      }

      /// <summary>
      /// A server accepting mail for the external domain, plus the route which sends it
      /// there. The route makes the domain reachable without leaving the machine; it does
      /// not make it one of ours, so messages sent there are still leaving the server as
      /// far as SRS is concerned.
      /// </summary>
      internal SmtpServerSimulator StartExternalServer(int numberOfConnections, params string[] recipients)
      {
         var port = TestSetup.GetNextFreePort();

         var server = new SmtpServerSimulator(numberOfConnections, port);

         for (var i = 0; i < numberOfConnections; i++)
         {
            var deliveryResults = new Dictionary<string, int>();

            foreach (var recipient in recipients)
               deliveryResults[recipient] = 250;

            server.AddRecipientResult(deliveryResults);
         }

         server.StartListen();

         AddRoute(ExternalDomain, port);

         return server;
      }

      protected Route AddRoute(string domainName, int port)
      {
         var route = _settings.Routes.Add();

         route.DomainName = domainName;
         route.TargetSMTPHost = "localhost";
         route.TargetSMTPPort = port;
         route.NumberOfTries = 1;
         route.MinutesBetweenTry = 5;
         route.TreatRecipientAsLocalDomain = false;
         route.TreatSenderAsLocalDomain = false;
         route.AllAddresses = true;
         route.Save();

         return route;
      }

      protected Account AddForwardingAccount(string address, string forwardAddress)
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, address, "test");

         account.ForwardEnabled = true;
         account.ForwardAddress = forwardAddress;
         account.ForwardKeepOriginal = true;
         account.Save();

         return account;
      }

      /// <summary>
      /// Forwards a message and hands back the envelope sender the receiving server saw.
      /// </summary>
      internal string GetSenderOfForwardedMessage(SmtpServerSimulator server, string from, string to)
      {
         SmtpClientSimulator.StaticSend(from, to, "Forwarded message", "This is the body");

         CustomAsserts.AssertRecipientsInDeliveryQueue(0);

         server.WaitForCompletion();

         return server.MailFrom;
      }
   }
}
