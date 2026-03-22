// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.Infrastructure
{
   [TestFixture]
   public class IPRanges : TestFixtureBase
   {
      [SetUp]
      public new void SetUp()
      {
         _ipRanges = SingletonProvider<TestSetup>.Instance.GetApp().Settings.SecurityRanges;
      }

      private SecurityRanges _ipRanges;

      private void AddIPRange()
      {
         var oRange = _ipRanges.Add();
         oRange.LowerIP = "127.0.0.1";
         oRange.UpperIP = "127.0.0.1";
         oRange.Name = "My computer";
         oRange.AllowDeliveryFromLocalToLocal = true;
         oRange.AllowDeliveryFromLocalToRemote = true;
         oRange.AllowDeliveryFromRemoteToLocal = true;
         oRange.AllowIMAPConnections = true;
         oRange.AllowPOP3Connections = true;
         oRange.AllowSMTPConnections = true;


         oRange.EnableSpamProtection = true;

         oRange.Save();
      }

      public void RemoveIPRanges()
      {
         while (_ipRanges.Count > 0)
            _ipRanges.Delete(0);
      }

      [Test]
      public void TestIPv6RangeCanBeSavedAndRetrieved()
      {
         // Verify that IPv6 addresses are parsed and stored correctly.
         // This exercises the make_address_v6 code path in IPAddress::TryParse.
         var range = _ipRanges.Add();
         range.LowerIP = "::1";
         range.UpperIP = "::1";
         range.Name = "IPv6 loopback";
         range.AllowSMTPConnections = true;
         range.Save();

         // Re-read from server to confirm the value round-tripped correctly.
         var saved = _ipRanges[_ipRanges.Count - 1];
         Assert.AreEqual("::1", saved.LowerIP);
         Assert.AreEqual("::1", saved.UpperIP);

         _ipRanges.Delete(_ipRanges.Count - 1);
      }

      [Test]
      public void TestConnections()
      {
         var oSocket = new TcpConnection();

         // Make sure an IP range exists.
         RemoveIPRanges();
         if (_ipRanges.Count == 0)
            AddIPRange();

         if (!oSocket.IsPortOpen(25))
            throw new Exception("ERROR: Cannot connect to port 25");
         if (!oSocket.IsPortOpen(110))
            throw new Exception("ERROR: Cannot connect to port 110");
         if (!oSocket.IsPortOpen(143))
            throw new Exception("ERROR: Cannot connect to port 143");

         RemoveIPRanges();

         // Now it shouldn't be possible to connect.

         if (oSocket.IsPortOpen(25))
            throw new Exception("ERROR: Cannot connect to port 25");
         if (oSocket.IsPortOpen(110))
            throw new Exception("ERROR: Cannot connect to port 110");
         if (oSocket.IsPortOpen(143))
            throw new Exception("ERROR: Cannot connect to port 143");

         AddIPRange();
         // Now it should be possible to connect again.
         if (!oSocket.IsPortOpen(25))
            throw new Exception("ERROR: Cannot connect to port 25");
         if (!oSocket.IsPortOpen(110))
            throw new Exception("ERROR: Cannot connect to port 110");
         if (!oSocket.IsPortOpen(143))
            throw new Exception("ERROR: Cannot connect to port 143");
      }
   }
}