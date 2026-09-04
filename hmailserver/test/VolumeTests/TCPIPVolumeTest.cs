// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;


namespace VolumeTests
{
   [TestFixture]
   public class TCPIPVolumeTest : TestFixtureBase
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

         const string connectionCreated = "TCP - 127.0.0.1 connected to 127.0.0.1:25.";
         const string connectionEnded = "Ending session ";

         // The server writes its log asynchronously. Asserting inside the retry would fail the
         // test on the first attempt, since NUnit records every failed assertion.
         RetryHelper.TryAction(() =>
            {
               string log = LogHandler.ReadCurrentDefaultLog();

               int created = Regex.Matches(log, connectionCreated).Count;
               int ended = Regex.Matches(log, connectionEnded).Count;

               if (created < count || ended < count)
                  throw new Exception(string.Format("The log contains {0} created and {1} ended sessions, expected {2}.",
                                                    created, ended, count));

            }, TimeSpan.FromSeconds(1), TimeSpan.FromSeconds(30));

         string finalLog = LogHandler.ReadCurrentDefaultLog();

         Assert.AreEqual(count, Regex.Matches(finalLog, connectionCreated).Count);
         Assert.AreEqual(count, Regex.Matches(finalLog, connectionEnded).Count);
      }
   }
}
