// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System.Collections.Generic;
using System.Net;

namespace RegressionTests.Shared
{
   /// <summary>
   /// Summary description for SMTPSimulator.
   /// </summary>
   public class SMTPClientSimulator
   {
      private readonly IPAddress _ipaddress;
      private readonly int _port = 25;
      private readonly TcpConnection _tcpConnection;

      public SMTPClientSimulator() :
         this(false, 25)
      {
      }

      public SMTPClientSimulator(bool useSSL, int port) :
         this(useSSL, port, null)
      {
      }

      public SMTPClientSimulator(bool useSSL, int port, IPAddress ipaddress)
      {
         _tcpConnection = new TcpConnection(useSSL);
         _port = port;
         _ipaddress = ipaddress;
      }

      public bool IsConnected
      {
         get { return _tcpConnection.IsConnected; }
      }

      public bool TestConnect(int iPort)
      {
         bool bRetVal = _tcpConnection.Connect(_ipaddress, iPort);
         _tcpConnection.Disconnect();
         return bRetVal;
      }

      public void Connect()
      {
         _tcpConnection.Connect(_port);
      }

      public bool ConnectAndLogon(string base64Username, string base64Password, out string errorMessage)
      {
         Connect();

         errorMessage = Receive();
         if (!errorMessage.StartsWith("220"))
            return false;

         Send("EHLO test\r\n");
         errorMessage = Receive();
         if (!errorMessage.StartsWith("250"))
            return false;

         Send("AUTH LOGIN\r\n");
         errorMessage = Receive();
         if (!errorMessage.StartsWith("334"))
            return false;

         Send(base64Username + "\r\n");
         errorMessage = Receive();
         if (!errorMessage.StartsWith("334"))
            return false;

         Send(base64Password + "\r\n");
         errorMessage = Receive();
         if (!errorMessage.StartsWith("235"))
            return false;

         return true;
      }

      public void Disconnect()
      {
         _tcpConnection.Disconnect();
      }

      public void Send(string sData)
      {
         _tcpConnection.Send(sData);
      }

      public string Receive()
      {
         return _tcpConnection.Receive();
      }

      public string SendAndReceive(string sData)
      {
         Send(sData);
         return Receive();
      }

      public void Handshake()
      {
         _tcpConnection.Handshake();
      }

      public string GetWelcomeMessage()
      {
         _tcpConnection.Connect(_port);
         string sData = _tcpConnection.Receive();

         _tcpConnection.Disconnect();
         return sData;
      }

      public bool Send(string sFrom, List<string> lstRecipients, string sSubject, string sBody)
      {
         if (!_tcpConnection.Connect(_ipaddress, _port))
            return false;

         // Receive welcome message.
         string sData = _tcpConnection.Receive();

         _tcpConnection.Send("HELO 127.0.0.1\r\n");
         sData = _tcpConnection.Receive();

         // User
         _tcpConnection.Send("MAIL FROM:<" + sFrom + ">\r\n");
         sData = _tcpConnection.Receive();

         string sCommaSeparatedRecipients = "";
         foreach (string sRecipient in lstRecipients)
         {
            _tcpConnection.Send("RCPT TO:<" + sRecipient + ">\r\n");
            sData = _tcpConnection.Receive();

            if (!sData.StartsWith("250"))
               return false;

            if (sCommaSeparatedRecipients.Length > 0)
               sCommaSeparatedRecipients += ", ";

            sCommaSeparatedRecipients += sRecipient;
         }

         // Select inbox
         _tcpConnection.Send("DATA\r\n");
         sData = _tcpConnection.Receive();

         if (sCommaSeparatedRecipients.Length > 5000)
            sCommaSeparatedRecipients = sCommaSeparatedRecipients.Substring(0, 5000);

         _tcpConnection.Send("From: " + sFrom + "\r\n");
         _tcpConnection.Send("To: " + sCommaSeparatedRecipients + "\r\n");
         _tcpConnection.Send("Subject: " + sSubject + "\r\n");
         _tcpConnection.Send("Date: " + TestSetup.GetCurrentMIMEDateTime() + "\r\n");

         _tcpConnection.Send("\r\n");

         // Send body

         _tcpConnection.Send(sBody);

         _tcpConnection.Send("\r\n");
         _tcpConnection.Send(".\r\n");

         // Wait for OK.
         sData = _tcpConnection.Receive();
         if (sData.Substring(0, 3) != "250")
            return false;

         // Quit again
         _tcpConnection.Send("QUIT\r\n");
         sData = _tcpConnection.Receive();

         _tcpConnection.Disconnect();

         return true;
      }

      public bool Send(string sFrom, string sTo, string sSubject, string sBody)
      {
         string result = "";

         return Send(sFrom, sTo, sSubject, sBody, out result);
      }

      public bool Send(string sFrom, string sTo, string sSubject, string sBody, out string result)
      {
         _tcpConnection.Connect(_ipaddress, _port);

         // Receive welcome message.
         string sData = _tcpConnection.Receive();

         _tcpConnection.Send("HELO example.com\r\n");
         sData = _tcpConnection.Receive();

         // User
         _tcpConnection.Send("MAIL FROM:<" + sFrom + ">\r\n");
         sData = _tcpConnection.Receive();

         _tcpConnection.Send("RCPT TO:<" + sTo + ">\r\n");
         sData = _tcpConnection.Receive();
         if (sData.StartsWith("2") == false)
         {
            result = sData;
            return false;
         }

         // Select inbox
         _tcpConnection.Send("DATA\r\n");
         sData = _tcpConnection.Receive();

         _tcpConnection.Send("From: " + sFrom + "\r\n");
         _tcpConnection.Send("To: " + sTo + "\r\n");
         _tcpConnection.Send("Subject: " + sSubject + "\r\n");

         _tcpConnection.Send("\r\n");

         // Send body

         _tcpConnection.Send(sBody);

         _tcpConnection.Send("\r\n");
         _tcpConnection.Send(".\r\n");

         // Wait for OK.
         sData = _tcpConnection.Receive();
         if (sData.Substring(0, 3) != "250")
         {
            result = sData;
            return false;
         }

         // Quit again
         _tcpConnection.Send("QUIT\r\n");
         sData = _tcpConnection.Receive();

         _tcpConnection.Disconnect();

         result = "";
         return true;
      }

      public bool SendRaw(string sFrom, string sTo, string text)
      {
         if (!_tcpConnection.Connect(_port))
            return false;

         // Receive welcome message.
         string sData = _tcpConnection.Receive();

         _tcpConnection.Send("HELO 127.0.0.1\r\n");
         sData = _tcpConnection.Receive();

         // User
         _tcpConnection.Send("MAIL FROM:<" + sFrom + ">\r\n");
         sData = _tcpConnection.Receive();

         _tcpConnection.Send("RCPT TO:<" + sTo + ">\r\n");
         sData = _tcpConnection.Receive();
         if (sData.StartsWith("2") == false)
            return false;

         // Send the message.
         _tcpConnection.Send("DATA\r\n");
         sData = _tcpConnection.Receive();

         _tcpConnection.Send(text);

         _tcpConnection.Send("\r\n");
         _tcpConnection.Send(".\r\n");

         // Wait for OK.
         sData = _tcpConnection.Receive();

         bool success = sData.Substring(0, 3) == "250";
         if (!success)
            return false;

         // Quit again
         _tcpConnection.Send("QUIT\r\n");
         sData = _tcpConnection.Receive();

         _tcpConnection.Disconnect();

         return success;
      }

      public static bool StaticSend(string sFrom, List<string> lstRecipients, string sSubject, string sBody)
      {
         var oSimulator = new SMTPClientSimulator();
         return oSimulator.Send(sFrom, lstRecipients, sSubject, sBody);
      }

      public static bool StaticSendRaw(string sFrom, string recipient, string sBody)
      {
         var oSimulator = new SMTPClientSimulator();
         return oSimulator.SendRaw(sFrom, recipient, sBody);
      }

      public static bool StaticSend(string sFrom, string recipient, string sSubject, string sBody)
      {
         var messageRecipients = new List<string>();
         messageRecipients.Add(recipient);

         var oSimulator = new SMTPClientSimulator();
         return oSimulator.Send(sFrom, messageRecipients, sSubject, sBody);
      }
   }
}