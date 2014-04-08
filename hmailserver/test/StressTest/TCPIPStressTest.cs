// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using NUnit.Framework;
using RegressionTests;
using System.Threading;
using RegressionTests.Shared;


namespace StressTest
{
   [TestFixture]
   public class TCPIPStressTest : TestFixtureBase
   {
      [Test]
      public void TestManyTCPIPConnections()
      {
         TestSetup.DeleteCurrentDefaultLog();

         const int count = 1000;

         List<TcpSocket> sockets = new List<TcpSocket>();
         for (int i = 0; i < count; i++)
         {
            TcpSocket socket = new TcpSocket();
            Assert.IsTrue(socket.Connect(25));

            sockets.Add(socket);
         }

         foreach (TcpSocket socket in sockets)
         {
            socket.Disconnect();
         }

         string log = TestSetup.ReadCurrentDefaultLog();
      }
   }
}
