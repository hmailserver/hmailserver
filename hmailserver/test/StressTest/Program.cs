// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using NUnit.Framework;
using RegressionTests;
using RegressionTests.Shared;

namespace StressTest
{
    [TestFixture]
    public class StressTests : TestFixtureBase
    {
        [SetUp]
        public new void SetUp()
        {
            SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
        }

        [Test]
        public void TestConnection()
        {
            for (int i = 0; i < 2500; i++)
            {
                TcpSocket socket = new TcpSocket();

                Assert.IsTrue(socket.CanConnect(25), i.ToString());
            }
        }

        [Test]
        public void TestLongIMAPCommand()
        {
            IMAPSimulator sim = ConnectAndLogon();

            // build a large string.
            StringBuilder sb = new StringBuilder();
            sb.Append("A01");
            for (int i = 0; i < 1000000; i++)
            {
                sb.Append("01234567890");
            }
            int length = sb.Length;

            sb.Append(Environment.NewLine);

            string result = sim.Send(sb.ToString());
            Assert.IsTrue(result.StartsWith("* BYE Excessive amount of data sent to server."));

            sim.Disconnect();
        }

        /// <summary>
        /// What happens if we connect to port 25 and send a 1000000000 character string with no new line.
        /// </summary>
        [Test]
        public void TestLongSMTPCommand()
        {
            TcpSocket socket = new TcpSocket();
            socket.Connect(25);

            // Build a large string...
            StringBuilder sb = new StringBuilder();
            sb.Append("A01");
            for (int i = 0; i < 1000000; i++)
            {
                sb.Append("01234567890");
            }
            int length = sb.Length;

            sb.Append(Environment.NewLine);

            for (int i = 0; i < 100; i++)
            {
                try
                {
                    socket.Send(sb.ToString());
                }
                catch (Exception)
                {
                    return;
                }
            }

            socket.Send("\r\n");
            string result = socket.Receive();

            socket.Disconnect();
        }

        /// <summary>
        /// What happens if we start a SMTP data session and send 100MB with newline.
        /// The memory usage should be below 20MB.
        /// </summary>
        [Test]
        public void TestLongSMTPDataSessionIncludingNewline()
        {
           long memoryUsage = Shared.GetCurrentMemoryUsage();

           _application.Settings.MaxMessageSize = 0;

            TcpSocket socket = new TcpSocket();
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

            for (int i = 0; i < 100; i++)
            {
                Shared.AssertLowMemoryUsage(memoryUsage+30);

                try
                {
                    socket.Send(sb.ToString());
                }
                catch (Exception)
                {
                    return;
                }
            }

            socket.Send("\r\n.\r\n");
            string result = socket.Receive();
            Assert.IsTrue(result.StartsWith("250"));

            socket.Send("QUIT\r\n");
            result = socket.Receive();
            socket.Disconnect();

            POP3Simulator.AssertMessageCount(_domain.Accounts[0].Address, "test", 1);
        }

        /// <summary>
        /// Can we retrieve a 100MB message over POP3?
        /// </summary>
        [Test]
        public void TestPOP3DownloadOfLargeMessage()
        {
            TestLongSMTPDataSessionIncludingNewline();

            POP3Simulator.AssertMessageCount("test@test.com", "test", 1);

            TcpSocket socket = new TcpSocket();
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

        /// <summary>
        /// What happens if we start a SMTP data session and send 1GB with newline...
        /// </summary>
        [Test]
        public void TestLongSMTPDataSessionWithoutNewline()
        {
           long memoryUsage = Shared.GetCurrentMemoryUsage();

            TcpSocket socket = new TcpSocket();
            socket.Connect(25);
            socket.Receive();

            socket.Send("HELO test.com\r\n");
            socket.Receive();

            // Build a large string...
            StringBuilder sb = new StringBuilder();
            sb.Append("A01");
            for (int i = 0; i < 10000; i++)
            {
                sb.Append("01234567890012345678900123456789001234567890012345678900123456789001234567890012345678900123456789001234567890");
            }
            int length = sb.Length;

            socket.Send("MAIL FROM: test@test.com\r\n");
            socket.Receive();
            socket.Send("RCPT TO: test@test.com\r\n");
            socket.Receive();
            socket.Send("DATA\r\n");
            socket.Receive();

            for (int i = 0; i < 1000; i++)
            {
                Shared.AssertLowMemoryUsage(memoryUsage+30);

                try
                {
                    socket.Send(sb.ToString());
                }
                catch (Exception)
                {
                    return;
                }
            }

            socket.Send("\r\n.\r\n");
            string result = socket.Receive();

            Assert.IsTrue(result.StartsWith("554 Too long line was received. Transmission aborted."));

            socket.Send("QUIT\r\n");
            result = socket.Receive();


            socket.Disconnect();
        }

        public void SendExploitFile(string file, bool sendAllAtOnce)
        {
            IMAPSimulator sim = ConnectAndLogon();

            string contents = File.ReadAllText(file);

            if (sendAllAtOnce)
            {
                sim.Send(contents);
            }
            else
            {

                string[] commands = Microsoft.VisualBasic.Strings.Split(contents, Environment.NewLine, 1000000, Microsoft.VisualBasic.CompareMethod.Text);
                foreach (string command in commands)
                {
                    try
                    {
                        sim.SendSingleCommand(command);
                    }
                    catch (Exception e)
                    {
                        Console.WriteLine(e.Message);
                        sim = ConnectAndLogon();
                    }
                }
            }

            sim.Disconnect();
        }

        private static IMAPSimulator ConnectAndLogon()
        {
            IMAPSimulator sim = new IMAPSimulator();
            sim.ConnectAndLogon("test@test.com", "test");
            return sim;
        }

    }
}
