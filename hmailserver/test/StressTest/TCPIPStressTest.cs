// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;
using RetryHelper = hMailServer.Test.Infrastructure.RetryHelper;


namespace StressTest
{
   [TestFixture]
   public class TCPIPStressTest : TestFixtureBase
   {
      [Test]
      public void TestManyTCPIPConnections()
      {
         LogHandler.DeleteCurrentDefaultLog();

         const int count = 1000;

         List<TcpConnection> sockets = new List<TcpConnection>();

         for (int i = 1; i <= count; i++)
         {
            TcpConnection socket = new TcpConnection();
            Assert.IsTrue(socket.Connect(25));

            if ((i % 10) == 0)
            {
               TestTracer.WriteTraceInfo("{0}/{1}", i, 1000);
            }

            sockets.Add(socket);
         }

         foreach (TcpConnection socket in sockets)
         {
            socket.Disconnect();
         }

         RetryHelper.TryAction(() =>
            {
               string log = LogHandler.ReadCurrentDefaultLog();

               string connectionCreated = "TCP - 127.0.0.1 connected to 127.0.0.1:25.";
               string connectionEnded = "Ending session ";

               var created = Regex.Matches(log, connectionCreated);
               var ended = Regex.Matches(log, connectionEnded);

               Assert.AreEqual(count, created.Count);
               Assert.AreEqual(count, ended.Count);

            }, TimeSpan.FromSeconds(1),TimeSpan.FromSeconds(30));
      }
   }
}
