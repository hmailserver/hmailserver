using System;
using System.Collections.Generic;
using System.Net;
using System.Text;
using System.Threading;
using NUnit.Framework;

namespace RegressionTests.Shared
{
   /// <summary>
   /// Summary description for POP3Simulator.
   /// </summary>
   public class POP3Simulator
   {
      private readonly IPAddress _ipaddress;
      private readonly int _port = 110;
      private readonly TcpSocket _socket;

      public POP3Simulator()
      {
         _socket = new TcpSocket();
      }

      public POP3Simulator(IPAddress ipaddress, bool useSSL, int port)
      {
         _socket = new TcpSocket(useSSL);
         _port = port;
         _ipaddress = ipaddress;
      }

      public POP3Simulator(bool useSSL, int port) :
         this(null, useSSL, port)
      {
      }

      public bool Connect(int iPort)
      {
         return _socket.Connect(_ipaddress, iPort);
      }

      public void Disconnect()
      {
         _socket.Disconnect();
      }

      public bool TestConnect(int iPort)
      {
         bool bRetVal = _socket.Connect(_ipaddress, iPort);
         _socket.Disconnect();
         return bRetVal;
      }

      public string GetWelcomeMessage()
      {
         _socket.Connect(_ipaddress, _port);
         string sData = _socket.Receive();

         _socket.Disconnect();
         return sData;
      }

      public bool ConnectAndLogon(string username, string password, out string errorMessage)
      {
         errorMessage = "";
         if (!_socket.Connect(_ipaddress, _port))
            return false;

         // Receive welcome message.
         errorMessage = _socket.ReadUntil("+OK");
         if (!errorMessage.StartsWith("+OK"))
            return false;

         _socket.Send("USER " + username + "\r\n");

         errorMessage = _socket.ReadUntil("+OK Send your password");
         if (!errorMessage.StartsWith("+OK"))
            return false;

         _socket.Send("PASS " + password + "\r\n");
         errorMessage = _socket.ReadUntil(new List<string> {"+OK", "-ERR"});

         return errorMessage.StartsWith("+OK");
      }

      public bool ConnectAndLogon(string username, string password)
      {
         string errorMessage;

         return ConnectAndLogon(username, password, out errorMessage);
      }

      public string RETR(int index)
      {
         _socket.Send("RETR " + index.ToString() + "\r\n");

         var result = new StringBuilder();

         string eofCheck = "";

         while (eofCheck.IndexOf("\r\n.\r\n") < 0)
         {
            if (eofCheck.IndexOf("-ERR no such message") >= 0)
            {
               _socket.Disconnect();
               return "";
            }

            string data = _socket.Receive();

            eofCheck += data;

            if (eofCheck.Length > 25)
            {
               // Only save the end of the string.
               eofCheck = eofCheck.Substring(eofCheck.Length - 25);
            }

            result.Append(data);
         }

         return result.ToString();
      }

      public string LIST()
      {
         string sRetVal = "";

         _socket.Send("LIST\r\n");

         while (sRetVal.IndexOf("\r\n.\r\n") < 0)
         {
            if (sRetVal.IndexOf("-ERR No such message") >= 0)
            {
               _socket.Disconnect();
               return "";
            }

            sRetVal += _socket.Receive();
         }

         return sRetVal;
      }


      public string LIST(int index)
      {
         string sRetVal;

         _socket.Send("LIST " + index.ToString() + "\r\n");

         sRetVal = _socket.Receive();

         return sRetVal;
      }

      public string UIDL()
      {
         string sRetVal = "";

         _socket.Send("UIDL\r\n");

         while (sRetVal.IndexOf("\r\n.\r\n") < 0)
         {
            if (sRetVal.IndexOf("-ERR No such message") >= 0)
            {
               _socket.Disconnect();
               return "";
            }

            sRetVal += _socket.Receive();
         }

         return sRetVal;
      }

      public string UIDL(int index)
      {
         string sRetVal;

         _socket.Send("UIDL " + index.ToString() + "\r\n");

         sRetVal = _socket.Receive();

         return sRetVal;
      }

      public bool DELE(int index)
      {
         _socket.Send("DELE " + index.ToString() + "\r\n");
         string data = _socket.ReadUntil(new List<string> {"+OK msg deleted", "-ERR No such message"});
         return data.StartsWith("+OK msg deleted");
      }

      public string TOP(int index, int rows)
      {
         if (rows > 0)
            _socket.Send("TOP " + index.ToString() + " " + rows.ToString() + "\r\n");
         else
            _socket.Send("TOP " + index.ToString() + "\r\n");

         string sRetVal = _socket.Receive();
         while (sRetVal.IndexOf("\r\n.\r\n") < 0)
         {
            if (sRetVal.IndexOf("-ERR No such message") >= 0)
            {
               return sRetVal;
            }

            sRetVal += _socket.Receive();
         }

         return sRetVal;
      }


      public void QUIT()
      {
         _socket.Send("QUIT\r\n");
         _socket.ReadUntil("+OK");
      }

      public string GetFirstMessageText(string sUsername, string sPassword)
      {
         ConnectAndLogon(sUsername, sPassword);

         string sRetVal = RETR(1);
         DELE(1);
         QUIT();

         _socket.Disconnect();

         return sRetVal;
      }

      public int GetMessageCount(string sUsername, string sPassword)
      {
         _socket.Connect(_port);

         // Receive welcome message.
         string sData = _socket.Receive();

         _socket.Send("USER " + sUsername + "\r\n");
         sData = _socket.ReadUntil("+OK Send your password");

         _socket.Send("PASS " + sPassword + "\r\n");
         sData =
            _socket.ReadUntil(new List<string>
                                 {"+OK Mailbox locked and ready", "-ERR Invalid user name or password."});
         Assert.IsTrue(sData.Contains("+OK Mailbox locked and ready"), sData);

         _socket.Send("LIST\r\n");
         sData = _socket.ReadUntil("+OK");

         // Check EXISTS header.
         int iStartPos = 4;
         int iEndPos = sData.IndexOf(" ", iStartPos);
         int iLength = iEndPos - iStartPos;
         string sValue = sData.Substring(iStartPos, iLength);

         _socket.Send("QUIT\r\n");
         sData = _socket.ReadUntil("+OK POP3 server saying goodbye...");

         _socket.Disconnect();

         return Convert.ToInt32(sValue);
      }

      public static void WaitForFirstMessage(string accountName, string accountPassword)
      {
         AssertGetFirstMessageText(accountName, accountPassword);
      }

      public static void AssertMessageCount(string accountName, string accountPassword, int expectedCount)
      {
         if (expectedCount == 0)
         {
            // just in case.
            TestSetup.AssertRecipientsInDeliveryQueue(0);
         }

         int timeout = 100;
         int actualCount = 0;
         while (timeout > 0)
         {
            var oPOP3 = new POP3Simulator();

            actualCount = oPOP3.GetMessageCount(accountName, accountPassword);
            if (actualCount == expectedCount)
               return;

            if (actualCount > expectedCount)
               Assert.Fail(
                  string.Format(
                     "Actual count exceeds expected count. Account name: {2}, Actual: {0}, Expected: {1}.",
                     actualCount, expectedCount, accountName));

            timeout--;
            Thread.Sleep(50);
         }

         Assert.Fail(string.Format("Wrong number of messages in inbox for {0}. Actual: {1}, Expected: {2}",
                                   accountName, actualCount, expectedCount));
      }

      public static string AssertGetFirstMessageText(string accountName, string accountPassword)
      {
         // Wait for the message to appear.
         var pop3 = new POP3Simulator();
         for (int i = 0; i < 5000; i++)
         {
            if (pop3.GetMessageCount(accountName, accountPassword) > 0)
               break;

            Thread.Sleep(20);
         }

         // Download it.
         string text = pop3.GetFirstMessageText(accountName, accountPassword);

         if (text.Length == 0)
            Assert.Fail("Message was found but contents could not be received");

         return text;
      }
   }
}