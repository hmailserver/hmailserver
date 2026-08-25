// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Diagnostics;
using System.Threading;
using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.Stress
{
   [TestFixture]
   [Explicit]
   [Category("Stress")]
   public class AbruptDisconnectTests : TestFixtureBase
   {
      [Test]
      [Category("TCP/IP implementation")]
      [Description("Issue 568. hMailServer entered an endless read loop, consuming all CPU, when the peer closed the connection while a message was being transferred.")]
      public void ClosingConnectionDuringDataTransferShouldNotCauseEndlessReadLoop()
      {
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         var socket = new TcpConnection();
         Assert.IsTrue(socket.Connect(25));
         Assert.IsTrue(socket.Receive().StartsWith("220"));

         Assert.IsTrue(socket.SendAndReceive("HELO example.com\r\n").StartsWith("250"));
         Assert.IsTrue(socket.SendAndReceive("MAIL FROM: <sender@example.com>\r\n").StartsWith("250"));
         Assert.IsTrue(socket.SendAndReceive("RCPT TO: <test@example.test>\r\n").StartsWith("250"));
         Assert.IsTrue(socket.SendAndReceive("DATA\r\n").StartsWith("354"));

         // Send a partial message, so that the server is waiting for more data, and then
         // close the connection without sending the terminating dot.
         socket.Send("Subject: Test\r\n\r\nPartial message body");

         var cpuTimeBefore = GetServerCpuTime();

         socket.Disconnect();

         Thread.Sleep(TimeSpan.FromSeconds(5));

         var cpuTimeUsed = GetServerCpuTime() - cpuTimeBefore;

         Assert.Less(cpuTimeUsed.TotalSeconds, 1,
            string.Format("hMailServer used {0} seconds of CPU time after the client disconnected, which indicates an endless read loop.",
               cpuTimeUsed.TotalSeconds));
      }

      [Test]
      [Category("TCP/IP implementation")]
      [Description("Issue 568. A client which sends the entire message before the server has asked for it, and then stops sending, should still have its message delivered.")]
      public void PipelinedMessageFollowedByImmediateCloseShouldBeDelivered()
      {
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         var socket = new TcpConnection();
         Assert.IsTrue(socket.Connect(25));
         Assert.IsTrue(socket.Receive().StartsWith("220"));

         Assert.IsTrue(socket.SendAndReceive("HELO example.com\r\n").StartsWith("250"));
         Assert.IsTrue(socket.SendAndReceive("MAIL FROM: <sender@example.com>\r\n").StartsWith("250"));
         Assert.IsTrue(socket.SendAndReceive("RCPT TO: <test@example.test>\r\n").StartsWith("250"));

         // Send DATA and the entire message in a single write. The server parses the DATA command
         // line but leaves the message in its receive buffer, and the client stops sending before
         // the server reads the message from the socket. The buffered message must not be lost.
         socket.Send("DATA\r\nSubject: Pipelined\r\n\r\nMessage body\r\n.\r\n");

         // Only shut down the sending half. Closing the entire connection would make the client
         // reset it when the server sends its DATA response, and the server would then fail the
         // read with a connection error instead of reaching the end of the stream.
         socket.ShutdownSend();

         Pop3ClientSimulator.AssertMessageCount("test@example.test", "test", 1);

         socket.Disconnect();
      }

      private static TimeSpan GetServerCpuTime()
      {
         var processes = Process.GetProcessesByName("hmailserver");
         if (processes.Length != 1)
            throw new Exception(string.Format("Expected a single hMailServer.exe process, found {0}", processes.Length));

         return processes[0].TotalProcessorTime;
      }
   }
}
