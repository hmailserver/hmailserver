// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using NUnit.Framework;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.Infrastructure
{
   [TestFixture]
   public class IPRanges : TestFixtureBase
   {
      #region Setup/Teardown

      [SetUp]
      public new void SetUp()
      {
         _ipRanges = SingletonProvider<TestSetup>.Instance.GetApp().Settings.SecurityRanges;
      }

      #endregion

      private SecurityRanges _ipRanges;

      private void AddIPRange()
      {
         SecurityRange oRange = _ipRanges.Add();
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