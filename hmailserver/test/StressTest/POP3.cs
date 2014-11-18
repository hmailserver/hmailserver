using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using NUnit.Framework;
using RegressionTests.Shared;

namespace StressTest
{
   [TestFixture]
   public class POP3 : TestFixtureBase
   {
      [SetUp]
      public new void SetUp()
      {
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
      }
      
      /// <summary>
      /// Can we retrieve a 100MB message over POP3?
      /// </summary>
      [Test]
      public void TestPOP3DownloadOfLargeMessage()
      {
         Send100MBMessage();

         Pop3ClientSimulator.AssertMessageCount("test@test.com", "test", 1);

         TcpConnection socket = new TcpConnection();
         socket.Connect(110);

         // Receive welcome message.
         socket.Receive();

         socket.Send("USER " + "test@test.com" + "\r\n");
         socket.ReadUntil("+OK");

         socket.Send("PASS " + "test" + "\r\n");
         socket.ReadUntil("+OK");

         socket.Send("RETR 1\r\n");

         var endOfResponseSB = new StringBuilder();

         string endOfResponse = "";
         while (endOfResponse.IndexOf("\r\n.\r\n") < 0)
         {
            if (endOfResponse.IndexOf("-ERR no such message") >= 0)
            {
               socket.Disconnect();
               Assert.Fail("Nope");
            }

            endOfResponseSB.Append(socket.Receive());

            if (endOfResponseSB.Length > 100)
               endOfResponseSB.Remove(0, endOfResponseSB.Length - 100);

            endOfResponse = endOfResponseSB.ToString();
         }

         socket.Send("DELE 1\r\n");
         socket.ReadUntil("+OK");

         socket.Send("QUIT\r\n");
         socket.ReadUntil("+OK");

         socket.Disconnect();
      }

      public void Send100MBMessage()
      {
         long memoryUsage = Shared.GetCurrentMemoryUsage();

         _application.Settings.MaxMessageSize = 0;

         TcpConnection socket = new TcpConnection();
         socket.Connect(25);
         socket.Receive();

         socket.Send("HELO test.com\r\n");
         socket.Receive();

         // Build a large string...
         StringBuilder sb = new StringBuilder();
         sb.Append("A01");
         for (int i = 0; i < 10000; i++)
         {
            sb.Append("01234567890012345678900123456789001234567890012345678900123456789001234567890012345678900123456789001234567890\r\n");
         }
         int length = sb.Length;

         sb.Append(Environment.NewLine);

         socket.Send("MAIL FROM: test@test.com\r\n");
         socket.Receive();
         socket.Send("RCPT TO: test@test.com\r\n");
         socket.Receive();
         socket.Send("DATA\r\n");
         socket.Receive();

         for (int i = 1; i <= 100; i++)
         {
            Shared.AssertLowMemoryUsage(memoryUsage + 30);

            try
            {
               socket.Send(sb.ToString());
            }
            catch (Exception)
            {
               return;
            }

            if ((i % 10) == 0)
            {
               TestTracer.WriteTraceInfo("{0}/{1}", i, 1000);
            }
         }

         socket.Send("\r\n.\r\n");
         string result = socket.Receive();
         Assert.IsTrue(result.StartsWith("250"));

         socket.Send("QUIT\r\n");
         result = socket.Receive();
         socket.Disconnect();

         Pop3ClientSimulator.AssertMessageCount(_domain.Accounts[0].Address, "test", 1);
      }
   }
}
