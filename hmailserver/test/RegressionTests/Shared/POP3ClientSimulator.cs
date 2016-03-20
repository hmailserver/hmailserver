using System;
using System.Collections.Generic;
using System.Net;
using System.Text;
using System.Threading;
using NUnit.Framework;
using RegressionTests.Infrastructure;

namespace RegressionTests.Shared
{
   /// <summary>
   /// Summary description for POP3ClientSimulator.
   /// </summary>
   public class Pop3ClientSimulator
   {
      private readonly IPAddress _ipaddress;
      private readonly int _port = 110;
      private readonly TcpConnection _tcpConnection;

      public Pop3ClientSimulator() :
         this(false, 110)
      {
         
      }


      public Pop3ClientSimulator(bool useSSL, int port) :
         this(IPAddress.Parse("127.0.0.1"), useSSL, port)
      {
      }


      public Pop3ClientSimulator(IPAddress ipaddress, bool useSSL, int port)
      {
         _tcpConnection = new TcpConnection(useSSL);
         _port = port;
         _ipaddress = ipaddress;
      }

      public bool Connect()
      {
         return _tcpConnection.Connect(_ipaddress, _port);
      }

      public void Disconnect()
      {
         _tcpConnection.Disconnect();
      }

      public bool TestConnect(int iPort)
      {
         bool bRetVal = _tcpConnection.Connect(_ipaddress, iPort);
         _tcpConnection.Disconnect();
         return bRetVal;
      }

      public string GetWelcomeMessage()
      {
         _tcpConnection.Connect(_ipaddress, _port);
         string sData = _tcpConnection.Receive();

         _tcpConnection.Disconnect();
         return sData;
      }

      public bool ConnectAndLogon(string username, string password, out string errorMessage)
      {
         errorMessage = "";
         if (!Connect())
            return false;


         if (!ReceiveBanner(out errorMessage))
            return false;

         _tcpConnection.Send("USER " + username + "\r\n");

         errorMessage = _tcpConnection.ReadUntil("+OK Send your password");
         if (!errorMessage.StartsWith("+OK"))
            return false;

         _tcpConnection.Send("PASS " + password + "\r\n");
         errorMessage = _tcpConnection.ReadUntil(new List<string> { "+OK", "-ERR" });

         return errorMessage.StartsWith("+OK");
      }

      public bool ReceiveBanner(out string errorMessage)
      {
         errorMessage = string.Empty;

         // Receive welcome message.
         var message = _tcpConnection.ReadUntil("+OK");
         if (!message.StartsWith("+OK"))
         {
            errorMessage = message;
            return false;
         }

         return true;
      }

      public void Handshake()
      {
         _tcpConnection.HandshakeAsClient();
      }

      public bool ConnectAndLogon(string username, string password)
      {
         string errorMessage;

         return ConnectAndLogon(username, password, out errorMessage);
      }

      public string RETR(int index)
      {
         _tcpConnection.Send("RETR " + index.ToString() + "\r\n");

         var result = new StringBuilder();

         string eofCheck = "";

         while (eofCheck.IndexOf("\r\n.\r\n") < 0)
         {
            if (eofCheck.StartsWith("-ERR"))
            {
               _tcpConnection.Disconnect();
               throw new Exception(string.Format("Message with index {0} does not exist.", index));
            }

            string data = _tcpConnection.Receive();

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

         _tcpConnection.Send("LIST\r\n");

         while (sRetVal.IndexOf("\r\n.\r\n") < 0)
         {
            if (sRetVal.IndexOf("-ERR No such message") >= 0)
            {
               _tcpConnection.Disconnect();
               return "";
            }

            sRetVal += _tcpConnection.Receive();
         }

         return sRetVal;
      }


      public string LIST(int index)
      {
         string sRetVal;

         _tcpConnection.Send("LIST " + index.ToString() + "\r\n");

         sRetVal = _tcpConnection.Receive();

         return sRetVal;
      }

      public string User(string userName)
      {
         _tcpConnection.Send("USER " + userName + "\r\n");

         return _tcpConnection.Receive();
      }


      public string UIDL()
      {
         string sRetVal = "";

         _tcpConnection.Send("UIDL\r\n");

         while (sRetVal.IndexOf("\r\n.\r\n") < 0)
         {
            if (sRetVal.IndexOf("-ERR No such message") >= 0)
            {
               _tcpConnection.Disconnect();
               return "";
            }

            sRetVal += _tcpConnection.Receive();
         }

         return sRetVal;
      }

      public string UIDL(int index)
      {
         string sRetVal;

         _tcpConnection.Send("UIDL " + index.ToString() + "\r\n");

         sRetVal = _tcpConnection.Receive();

         return sRetVal;
      }

      public bool DELE(int index)
      {
         _tcpConnection.Send("DELE " + index.ToString() + "\r\n");
         string data = _tcpConnection.ReadUntil(new List<string> { "+OK msg deleted", "-ERR No such message" });
         return data.StartsWith("+OK msg deleted");
      }

      public bool HELP()
      {
         _tcpConnection.Send("HELP\r\n");
         _tcpConnection.ReadUntil(new List<string> { "+OK Normal POP3 commands allowed" });

         return true;
      }

      public bool STLS()
      {
         _tcpConnection.Send("STLS\r\n");
         _tcpConnection.ReadUntil(new List<string> { "+OK Begin TLS negotiation" });

         return true;
      }

      public string TOP(int index, int rows)
      {
         if (rows > 0)
            _tcpConnection.Send("TOP " + index.ToString() + " " + rows.ToString() + "\r\n");
         else
            _tcpConnection.Send("TOP " + index.ToString() + "\r\n");

         string sRetVal = _tcpConnection.Receive();
         while (sRetVal.IndexOf("\r\n.\r\n") < 0)
         {
            if (sRetVal.IndexOf("-ERR No such message") >= 0)
            {
               return sRetVal;
            }

            sRetVal += _tcpConnection.Receive();
         }

         return sRetVal;
      }


      public void QUIT()
      {
         _tcpConnection.Send("QUIT\r\n");
         _tcpConnection.ReadUntil("+OK");
      }

      public string CAPA()
      {
         _tcpConnection.Send("CAPA\r\n");
         return _tcpConnection.Receive();
      }

      public string GetFirstMessageText(string sUsername, string sPassword)
      {
         if (!ConnectAndLogon(sUsername, sPassword))
            throw new Exception("Unable to connect to server.");

         string sRetVal = RETR(1);
         DELE(1);
         QUIT();

         _tcpConnection.Disconnect();

         return sRetVal;
      }

      public int GetMessageCount(string sUsername, string sPassword)
      {
         if (!_tcpConnection.Connect(_port))
            throw new Exception(string.Format("Unable to connect to POP3 server on localhost on port {0}", _port));

         // Receive welcome message.
         string sData = _tcpConnection.Receive();

         _tcpConnection.Send("USER " + sUsername + "\r\n");
         sData = _tcpConnection.ReadUntil("+OK Send your password");

         _tcpConnection.Send("PASS " + sPassword + "\r\n");
         sData = _tcpConnection.ReadUntil(new List<string> { "+OK Mailbox locked and ready", "-ERR Invalid user name or password." });
         Assert.IsTrue(sData.Contains("+OK Mailbox locked and ready"), sData);

         _tcpConnection.Send("LIST\r\n");
         sData = _tcpConnection.ReadUntil("+OK");

         // Check EXISTS header.
         int iStartPos = 4;
         int iEndPos = sData.IndexOf(" ", iStartPos);
         int iLength = iEndPos - iStartPos;
         string sValue = sData.Substring(iStartPos, iLength);

         _tcpConnection.Send("QUIT\r\n");
         sData = _tcpConnection.ReadUntil("+OK POP3 server saying goodbye...");

         _tcpConnection.Disconnect();

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
            CustomAsserts.AssertRecipientsInDeliveryQueue(0);
         }

         int timeout = 100;
         int actualCount = 0;
         while (timeout > 0)
         {
            var pop3ClientSimulator = new Pop3ClientSimulator();

            actualCount = pop3ClientSimulator.GetMessageCount(accountName, accountPassword);
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
         var pop3 = new Pop3ClientSimulator();
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