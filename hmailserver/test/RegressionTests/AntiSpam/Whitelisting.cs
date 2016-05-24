// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Net;
using System.Collections.Generic;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.AntiSpam
{
   [TestFixture]
   internal class WhiteListing : TestFixtureBase
   {
      #region Setup/Teardown

      private const string SurblTestPointBody =
         "This is a test message with a SURBL url: -> http://surbl-org-permanent-test-point.com/ <-";

      [SetUp]
      public new void SetUp()
      {
         _antiSpam = _settings.AntiSpam;

         // We should always mark messages as spam.
         _antiSpam.SpamDeleteThreshold = 5;
         _antiSpam.SpamMarkThreshold = 2;

         _antiSpam.AddHeaderReason = true;
         _antiSpam.AddHeaderSpam = true;
         _antiSpam.PrependSubject = true;
         _antiSpam.PrependSubjectText = "ThisIsSpam";

         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "whitelist@test.com", "test");
      }

      #endregion

      private hMailServer.AntiSpam _antiSpam;

      [Test]
      public void TestEnabled()
      {
         WhiteListAddresses obAddresses = _antiSpam.WhiteListAddresses;
         WhiteListAddress obAddress = obAddresses.Add();

         obAddress.EmailAddress = "whitelist@microsoft.com";
         obAddress.LowerIPAddress = "0.0.0.0";
         obAddress.UpperIPAddress = "255.255.255.255";
         obAddress.Description = "Test";
         obAddress.Save();

         // Enable SURBL.
         var oSURBLServer = _antiSpam.SURBLServers[0];
         oSURBLServer.Active = true;
         oSURBLServer.Score = 5;
         oSURBLServer.Save();

         // Send a messages to this account.
         SmtpClientSimulator.StaticSend("whitelist@microsoft.com", "whitelist@test.com", "SURBL-Match",
                                                      "This is a test message with a SURBL url: -> http://surbl-org-permanent-test-point.com/ <-");

         obAddresses.DeleteByDBID(obAddress.ID);

         // Check that it's detected as spam again.
         CustomAsserts.Throws<DeliveryFailedException>(() => SmtpClientSimulator.StaticSend("whitelist@microsoft.com", "whitelist@test.com", "SURBL-Match",
                                                       "This is a test message with a SURBL url: -> http://surbl-org-permanent-test-point.com/ <-"));

         Pop3ClientSimulator.AssertMessageCount("whitelist@test.com", "test", 1);
      }

      [Test]
      public void TestFormatVariations()
      {
         WhiteListAddresses obAddresses = _antiSpam.WhiteListAddresses;

         WhiteListAddress obAddress = obAddresses.Add();

         string address = @"A%B/C\D_@microsoft.com";
         obAddress.EmailAddress = address;
         Assert.AreEqual(address, obAddress.EmailAddress);

         address = @"\%%%__\_@microsoft.com";
         obAddress.EmailAddress = address;
         Assert.AreEqual(address, obAddress.EmailAddress);
      }

      [Test]
      public void TestHelo()
      {
         WhiteListAddresses obAddresses = _antiSpam.WhiteListAddresses;
         WhiteListAddress obAddress = obAddresses.Add();

         obAddress.EmailAddress = "whitelist@microsoft.com";
         obAddress.LowerIPAddress = "0.0.0.0";
         obAddress.UpperIPAddress = "255.255.255.255";
         obAddress.Description = "Test";
         obAddress.Save();

         // Test that we can send spam now.
         // Create a test account
         // Fetch the default domain

         _antiSpam.CheckHostInHelo = true;
         _antiSpam.CheckHostInHeloScore = 125;

         // Enable SURBL.
         var oSURBLServer = _antiSpam.SURBLServers[0];
         oSURBLServer.Active = true;
         oSURBLServer.Score = 5;
         oSURBLServer.Save();

         // Send a messages to this account.
         var smtpClientSimulator = new SmtpClientSimulator();

         smtpClientSimulator.Send("whitelist@microsoft.com", "whitelist@test.com", "SURBL-Match",
                                  "This is a test message with a SURBL url: -> http://surbl-org-permanent-test-point.com/ <-");

         obAddresses.DeleteByDBID(obAddress.ID);

         // Check that it's deteceted as spam again.
         CustomAsserts.Throws<DeliveryFailedException>(() => smtpClientSimulator.Send("whitelist@microsoft.com", "whitelist@test.com", "SURBL-Match",
                                   "This is a test message with a SURBL url: -> http://surbl-org-permanent-test-point.com/ <-"));

         Pop3ClientSimulator.AssertMessageCount("whitelist@test.com", "test", 1);
      }


      [Test]
      public void TestWildcardEscapedCharacters()
      {
         WhiteListAddresses obAddresses = _antiSpam.WhiteListAddresses;

         WhiteListAddress obAddress = obAddresses.Add();
         obAddress.EmailAddress = "white%li_st@microsoft.com";
         obAddress.LowerIPAddress = "0.0.0.0";
         obAddress.UpperIPAddress = "255.255.255.255";
         obAddress.Description = "Test";
         obAddress.Save();

         // Enable SURBL.
         SURBLServer oSURBLServer = _antiSpam.SURBLServers[0];
         oSURBLServer.Active = true;
         oSURBLServer.Score = 5;
         oSURBLServer.Save();

         // Send a messages to this account.

         SmtpClientSimulator.StaticSend("white%li_st@microsoft.com", "whitelist@test.com",
                                                      "SURBL-Match",
                                                      "This is a test message with a SURBL url: -> http://surbl-org-permanent-test-point.com/ <-");
         CustomAsserts.Throws<DeliveryFailedException>(() => SmtpClientSimulator.StaticSend("whiteAlist@micro_soft.com", "whitelist@test.com",
                                                       "SURBL-Match",
                                                       "This is a test message with a SURBL url: -> http://surbl-org-permanent-test-point.com/ <-"));

         CustomAsserts.Throws<DeliveryFailedException>(() => SmtpClientSimulator.StaticSend("whiteAlist@microEsoft.com", "whitelist@test.com",
                                                       "SURBL-Match",
                                                       "This is a test message with a SURBL url: -> http://surbl-org-permanent-test-point.com/ <-"));

         Pop3ClientSimulator.AssertMessageCount("whitelist@test.com", "test", 1);
      }

      [Test]
      public void TestWildcardQuestionMark()
      {
         WhiteListAddresses obAddresses = _antiSpam.WhiteListAddresses;

         WhiteListAddress obAddress = obAddresses.Add();
         obAddress.EmailAddress = "whitelist@?icrosoft.com";
         obAddress.LowerIPAddress = "0.0.0.0";
         obAddress.UpperIPAddress = "255.255.255.255";
         obAddress.Description = "Test";
         obAddress.Save();


         // Enable SURBL.
         SURBLServer oSURBLServer = _antiSpam.SURBLServers[0];
         oSURBLServer.Active = true;
         oSURBLServer.Score = 5;
         oSURBLServer.Save();

         // Send a messages to this account.
         SmtpClientSimulator.StaticSend("whitelist@microsoft.com", "whitelist@test.com", "SURBL-Match",
                                                      "This is a test message with a SURBL url: -> http://surbl-org-permanent-test-point.com/ <-");
         CustomAsserts.Throws<DeliveryFailedException>(() => SmtpClientSimulator.StaticSend("whitelist@icrosoft.com", "whitelist@test.com", "SURBL-Match",
                                                       "This is a test message with a SURBL url: -> http://surbl-org-permanent-test-point.com/ <-"));
         SmtpClientSimulator.StaticSend("whitelist@microsoft.com", "whitelist@test.com", "SURBL-Match",
                                                      "This is a test message with a SURBL url: -> http://surbl-org-permanent-test-point.com/ <-");
         CustomAsserts.Throws<DeliveryFailedException>(() => SmtpClientSimulator.StaticSend("whitelist@icrosoft.com", "whitelist@test.com", "SURBL-Match",
                                                       "This is a test message with a SURBL url: -> http://surbl-org-permanent-test-point.com/ <-"));

         Pop3ClientSimulator.AssertMessageCount("whitelist@test.com", "test", 2);
      }


      [Test]
      public void TestWildcardSingleQuote()
      {
         WhiteListAddresses obAddresses = _antiSpam.WhiteListAddresses;

         WhiteListAddress obAddress = obAddresses.Add();
         obAddress.EmailAddress = "white'list@example.com";
         obAddress.LowerIPAddress = "0.0.0.0";
         obAddress.UpperIPAddress = "255.255.255.255";
         obAddress.Description = "Test";
         obAddress.Save();

         // Enable SURBL.
         SURBLServer oSURBLServer = _antiSpam.SURBLServers[0];
         oSURBLServer.Active = true;
         oSURBLServer.Score = 5;
         oSURBLServer.Save();

         // Send a messages to this account.
         SmtpClientSimulator.StaticSend("white'list@example.com", "whitelist@test.com",
                                                      "SURBL-Match",
                                                      "This is a test message with a SURBL url: -> http://surbl-org-permanent-test-point.com/ <-");
         CustomAsserts.Throws<DeliveryFailedException>(() => SmtpClientSimulator.StaticSend("whitelist@micro_soft.com", "whitelist@test.com",
                                                       "SURBL-Match",
                                                       "This is a test message with a SURBL url: -> http://surbl-org-permanent-test-point.com/ <-"));

         Pop3ClientSimulator.AssertMessageCount("whitelist@test.com", "test", 1);
      }

      [Test]
      public void TestWildcardStar()
      {
         WhiteListAddresses obAddresses = _antiSpam.WhiteListAddresses;

         WhiteListAddress obAddress = obAddresses.Add();
         obAddress.EmailAddress = "white*@microsoft.com";
         obAddress.LowerIPAddress = "0.0.0.0";
         obAddress.UpperIPAddress = "255.255.255.255";
         obAddress.Description = "Test";
         obAddress.Save();

         // Enable SURBL.
         SURBLServer oSURBLServer = _antiSpam.SURBLServers[0];
         oSURBLServer.Active = true;
         oSURBLServer.Score = 5;
         oSURBLServer.Save();

         // Send a messages to this account.
         SmtpClientSimulator.StaticSend("whitelist@microsoft.com", "whitelist@test.com", "SURBL-Match",
                                                      "This is a test message with a SURBL url: -> http://surbl-org-permanent-test-point.com/ <-");
         CustomAsserts.Throws<DeliveryFailedException>(() => SmtpClientSimulator.StaticSend("blacklist@microsoft.com", "whitelist@test.com", "SURBL-Match",
                                                       "This is a test message with a SURBL url: -> http://surbl-org-permanent-test-point.com/ <-"));
         SmtpClientSimulator.StaticSend("whitesomething@microsoft.com", "whitelist@test.com",
                                                      "SURBL-Match",
                                                      "This is a test message with a SURBL url: -> http://surbl-org-permanent-test-point.com/ <-");
         CustomAsserts.Throws<DeliveryFailedException>(() => SmtpClientSimulator.StaticSend("blacksomething@microsoft.com", "whitelist@test.com",
                                                       "SURBL-Match",
                                                       "This is a test message with a SURBL url: -> http://surbl-org-permanent-test-point.com/ <-"));

         Pop3ClientSimulator.AssertMessageCount("whitelist@test.com", "test", 2);
      }

      [Test]
      public void TestWhitelistSpecificIPv6Address()
      {
         var addresses = GetAllLocalAddresses(System.Net.Sockets.AddressFamily.InterNetworkV6);

         if (addresses.Length == 0)
         {
            Assert.Inconclusive("IPv6 not available.");
         }

         var firstAddress = addresses[0];

         // Enable this port
         var tcpIpPort = _application.Settings.TCPIPPorts.Add();
         tcpIpPort.Address = firstAddress;
         tcpIpPort.Protocol = eSessionType.eSTSMTP;
         tcpIpPort.PortNumber = 25;
         tcpIpPort.Save();

         // Add an IP range for ALL ipv6 source port.
         foreach (var address in addresses)
         {
            var ipRange = _application.Settings.SecurityRanges.Add();
            ipRange.Name = "IPv6Range" + address;
            ipRange.AllowDeliveryFromLocalToLocal = true;
            ipRange.AllowDeliveryFromLocalToRemote = true;
            ipRange.AllowDeliveryFromRemoteToLocal = true;
            ipRange.AllowDeliveryFromRemoteToRemote = true;
            ipRange.AllowSMTPConnections = true;
            ipRange.RequireAuthForDeliveryToLocal = false;
            ipRange.EnableSpamProtection = true;
            ipRange.LowerIP = address;
            ipRange.UpperIP = address;
            ipRange.Save();
         }

         _application.Stop();
         _application.Start();

         // Enable SURBL.
         SURBLServer oSURBLServer = _antiSpam.SURBLServers[0];
         oSURBLServer.Active = true;
         oSURBLServer.Score = 5;
         oSURBLServer.Save();

         // Make sure we are now blacklisted.
         var smtpClient = new SmtpClientSimulator(false, 25, IPAddress.Parse(firstAddress));
         CustomAsserts.Throws<DeliveryFailedException>(() => smtpClient.Send("user@example.com", "whitelist@test.com", "Hello", SurblTestPointBody));

         // White list all IPv6 addresses
         foreach (var address in addresses)
         {
            var obAddress = _antiSpam.WhiteListAddresses.Add();
            obAddress.EmailAddress = "*";
            obAddress.LowerIPAddress = address;
            obAddress.UpperIPAddress = address;
            obAddress.Description = "Test";
            obAddress.Save();
         }

         // Make sure we can now send again.
         smtpClient.Send("user@example.com", "whitelist@test.com", "Hello", SurblTestPointBody);

         Pop3ClientSimulator.AssertMessageCount("whitelist@test.com", "test", 1);
      }

      [Test]
      public void TestWhitelistOutOfRangeIPv6Address()
      {
         var addresses = GetAllLocalAddresses(System.Net.Sockets.AddressFamily.InterNetworkV6);

         if (addresses.Length == 0)
         {
            Assert.Inconclusive("IPv6 not available.");
         }

         var firstAddress = addresses[0];

         // Enable this port
         var tcpIpPort = _application.Settings.TCPIPPorts.Add();
         tcpIpPort.Address = firstAddress;
         tcpIpPort.Protocol = eSessionType.eSTSMTP;
         tcpIpPort.PortNumber = 25;
         tcpIpPort.Save();

         // Add an IP range for ALL ipv6 source port.
         foreach (var address in addresses)
         {
            var ipRange = _application.Settings.SecurityRanges.Add();
            ipRange.Name = "IPv6Range" + address;
            ipRange.AllowDeliveryFromLocalToLocal = true;
            ipRange.AllowDeliveryFromLocalToRemote = true;
            ipRange.AllowDeliveryFromRemoteToLocal = true;
            ipRange.AllowDeliveryFromRemoteToRemote = true;
            ipRange.AllowSMTPConnections = true;
            ipRange.RequireAuthForDeliveryToLocal = false;
            ipRange.EnableSpamProtection = true;
            ipRange.LowerIP = address;
            ipRange.UpperIP = address;
            ipRange.Save();
         }

         _application.Stop();
         _application.Start();

         // Enable SURBL.
         SURBLServer oSURBLServer = _antiSpam.SURBLServers[0];
         oSURBLServer.Active = true;
         oSURBLServer.Score = 5;
         oSURBLServer.Save();

         // Make sure we are now blacklisted.
         var smtpClient = new SmtpClientSimulator(false, 25, IPAddress.Parse(firstAddress));
         CustomAsserts.Throws<DeliveryFailedException>(() => smtpClient.Send("user@example.com", "whitelist@test.com", "Hello", SurblTestPointBody));

         // White list all IPv6 addresses
         var obAddress = _antiSpam.WhiteListAddresses.Add();
         obAddress.EmailAddress = "*";
         obAddress.LowerIPAddress = "1111::1110";
         obAddress.UpperIPAddress = "1111::1111";
         obAddress.Description = "Test";
         obAddress.Save();

         // Make sure we can now send again.
         CustomAsserts.Throws<DeliveryFailedException>(() => smtpClient.Send("user@example.com", "whitelist@test.com", "Hello", SurblTestPointBody));
      }

      [Test]
      public void TestWhitelistSpecificIpV4Address()
      {
         var addresses = GetAllLocalAddresses(System.Net.Sockets.AddressFamily.InterNetwork);
         var firstAddress = addresses[0];

         // Enable SURBL.
         var oSURBLServer = _antiSpam.SURBLServers[0];
         oSURBLServer.Active = true;
         oSURBLServer.Score = 5;
         oSURBLServer.Save();

         // Make sure we are now blacklisted.
         var smtpClient = new SmtpClientSimulator(false, 25, IPAddress.Parse(firstAddress));
         CustomAsserts.Throws<DeliveryFailedException>(() => smtpClient.Send("user@example.com", "whitelist@test.com", "Hello", SurblTestPointBody));

         // White list all IPv4 addresses
         foreach (var address in addresses)
         {
            var obAddress = _antiSpam.WhiteListAddresses.Add();
            obAddress.EmailAddress = "*";
            obAddress.LowerIPAddress = address;
            obAddress.UpperIPAddress = address;
            obAddress.Description = "Test";
            obAddress.Save();
         }

         // Make sure we can now send again.
         smtpClient.Send("user@example.com", "whitelist@test.com", "Hello", SurblTestPointBody);

         Pop3ClientSimulator.AssertMessageCount("whitelist@test.com", "test", 1);
      }

      [Test]
      public void TestWhitelistOutOfRangeAddress()
      {
         // Enable SURBL.
         var oSURBLServer = _antiSpam.SURBLServers[0];
         oSURBLServer.Active = true;
         oSURBLServer.Score = 5;
         oSURBLServer.Save();

         // Make sure we are now blacklisted.
         var smtpClient = new SmtpClientSimulator(false, 25);
         CustomAsserts.Throws<DeliveryFailedException>(() => smtpClient.Send("user@example.com", "whitelist@test.com", "Hello", SurblTestPointBody));

         // White list all IPv4 addresses
         var obAddress = _antiSpam.WhiteListAddresses.Add();
         obAddress.EmailAddress = "*";
         obAddress.LowerIPAddress = "1.1.1.1";
         obAddress.UpperIPAddress = "1.1.1.5";
         obAddress.Description = "Test";
         obAddress.Save();

         // Make sure we are still blacklisted.
         CustomAsserts.Throws<DeliveryFailedException>(() => smtpClient.Send("user@example.com", "whitelist@test.com", "Hello", SurblTestPointBody));
      }

      [Test]
      public void TestWhitelistAllIPv6Addresses()
      {
         var addresses = GetAllLocalAddresses(System.Net.Sockets.AddressFamily.InterNetworkV6);

         if (addresses.Length == 0)
         {
            Assert.Inconclusive("IPv6 not available.");
         }

         var firstAddress = addresses[0];

         // Enable this port
         var tcpIpPort = _application.Settings.TCPIPPorts.Add();
         tcpIpPort.Address = firstAddress;
         tcpIpPort.Protocol = eSessionType.eSTSMTP;
         tcpIpPort.PortNumber = 25;
         tcpIpPort.Save();

         // Add an IP range for ALL ipv6 source port.
         foreach (var address in addresses)
         {
            var ipRange = _application.Settings.SecurityRanges.Add();
            ipRange.Name = "IPv6Range" + address;
            ipRange.AllowDeliveryFromLocalToLocal = true;
            ipRange.AllowDeliveryFromLocalToRemote = true;
            ipRange.AllowDeliveryFromRemoteToLocal = true;
            ipRange.AllowDeliveryFromRemoteToRemote = true;
            ipRange.AllowSMTPConnections = true;
            ipRange.RequireAuthForDeliveryToLocal = false;
            ipRange.EnableSpamProtection = true;
            ipRange.LowerIP = address;
            ipRange.UpperIP = address;
            ipRange.Save();
         }

         _application.Stop();
         _application.Start();

         // Enable SURBL.
         SURBLServer oSURBLServer = _antiSpam.SURBLServers[0];
         oSURBLServer.Active = true;
         oSURBLServer.Score = 5;
         oSURBLServer.Save();

         // Make sure we are now blacklisted.
         var smtpClient = new SmtpClientSimulator(false, 25, IPAddress.Parse(firstAddress));
         CustomAsserts.Throws<DeliveryFailedException>(() => smtpClient.Send("user@example.com", "whitelist@test.com", "Hello", SurblTestPointBody));

         // White list all IPv6 addresses
         var obAddress = _antiSpam.WhiteListAddresses.Add();
         obAddress.EmailAddress = "*";
         obAddress.LowerIPAddress = "::";
         obAddress.UpperIPAddress = "ffff::ffff";
         obAddress.Description = "Test";
         obAddress.Save();

         // Make sure we can now send again.
         smtpClient.Send("user@example.com", "whitelist@test.com", "Hello", SurblTestPointBody);

         Pop3ClientSimulator.AssertMessageCount("whitelist@test.com", "test", 1);
      }


      private string[] GetAllLocalAddresses(System.Net.Sockets.AddressFamily family)
      {
         var result = new List<String>();

         string strHostName = Dns.GetHostName(); ;
         var ipEntry = Dns.GetHostEntry(strHostName);

         foreach (var address in ipEntry.AddressList)
         {
            if (address.AddressFamily == family)
            {
               string addr = address.ToString();

               if (addr.Contains("%"))
                  result.Add(addr.Substring(0, addr.IndexOf("%", StringComparison.InvariantCulture)));
               else
                  result.Add(addr);
            }
         }

         return result.ToArray();

      }
   }
}