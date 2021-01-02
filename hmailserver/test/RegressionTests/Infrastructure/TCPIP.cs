// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using NUnit.Framework;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.Infrastructure
{
   [TestFixture]
   public class TCPIP : TestFixtureBase
   {
      [Test]
      [Category("TCP/IP implementation")]
      [Description("Ensure that basic resolution of existing domain names work.")]
      public void TestMXQueryExistingDomain()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();


         string sQuery = application.Utilities.GetMailServer("martin@hmailserver.com");
         if (sQuery != TestSetup.GethMailServerCOMIPaddress())
            throw new Exception("ERROR - DNX query failed: " + sQuery);
      }

      [Test]
      [Category("TCP/IP implementation")]
      [Description("Ensure that basic resolution of non-existing domain names work.")]
      public void TestMXQueryNonExistentDomain()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();

         string query = application.Utilities.GetMailServer("martin@23sdfakm52lvcxbmvxcbmdtapvxcpaasdf.com");
         Assert.IsTrue(query.Length == 0);
      }

      [Test]
      [Category("TCP/IP implementation")]
      [Description("Ensure that it's possible to re-configure which ports hMailServer should listen on")]
      public void TestPortOpening()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();

         application.Settings.TCPIPPorts.SetDefault();

         var pSMTPSimulator = new TcpConnection();
         var pPOP3Simulator = new Pop3ClientSimulator();
         var pIMAPSimulator = new ImapClientSimulator();

         application.Stop();

         TCPIPPorts ports = application.Settings.TCPIPPorts;
         for (int i = 0; i < ports.Count; i++)
         {
            TCPIPPort testPort = ports[i];
            if (testPort.Protocol == eSessionType.eSTIMAP)
               testPort.PortNumber = 14300;
            else if (testPort.Protocol == eSessionType.eSTSMTP)
               testPort.PortNumber = 11000;
            else if (testPort.Protocol == eSessionType.eSTPOP3)
               testPort.PortNumber = 2500;

            testPort.Save();
         }

         application.Start();

         Assert.IsTrue(pSMTPSimulator.TestConnect(2500));
         Assert.IsTrue(pSMTPSimulator.TestConnect(11000));
         Assert.IsTrue(pSMTPSimulator.TestConnect(14300));

         application.Stop();

         TCPIPPort port = application.Settings.TCPIPPorts.Add();
         port.Protocol = eSessionType.eSTSMTP;
         port.PortNumber = 25000;
         port.Save();

         application.Start();

         // Try to connect to the new port
         Assert.IsTrue(pSMTPSimulator.TestConnect(25000));

         application.Stop();

         // Delete the port again
         application.Settings.TCPIPPorts.DeleteByDBID(port.ID);

         // Change back the ports
         for (int i = 0; i < ports.Count; i++)
         {
            TCPIPPort testPort = ports[i];
            if (testPort.Protocol == eSessionType.eSTIMAP)
               testPort.PortNumber = 143;
            else if (testPort.Protocol == eSessionType.eSTSMTP)
               testPort.PortNumber = 25;
            else if (testPort.Protocol == eSessionType.eSTPOP3)
               testPort.PortNumber = 110;

            testPort.Save();
         }

         application.Start();

         Assert.IsTrue(pSMTPSimulator.TestConnect(25));
         Assert.IsTrue(pPOP3Simulator.TestConnect(110));
         Assert.IsTrue(pPOP3Simulator.TestConnect(143));
      }
   }
}