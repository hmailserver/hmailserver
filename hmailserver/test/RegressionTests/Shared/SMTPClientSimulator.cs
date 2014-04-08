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
      private readonly TcpSocket _socket;

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
         _socket = new TcpSocket(useSSL);
         _port = port;
         _ipaddress = ipaddress;
      }

      public bool IsConnected
      {
         get { return _socket.Connected; }
      }

      public bool TestConnect(int iPort)
      {
         bool bRetVal = _socket.Connect(_ipaddress, iPort);
         _socket.Disconnect();
         return bRetVal;
      }

      public void Connect(int iPort)
      {
         _socket.Connect(iPort);
      }

      public bool ConnectAndLogon(int port, string base64Username, string base64Password, out string errorMessage)
      {
         Connect(port);

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
         _socket.Disconnect();
      }

      public void Send(string sData)
      {
         _socket.Send(sData);
      }

      public string Receive()
      {
         return _socket.Receive();
      }

      public string SendAndReceive(string sData)
      {
         Send(sData);
         return Receive();
      }

      public string GetWelcomeMessage()
      {
         _socket.Connect(_port);
         string sData = _socket.Receive();

         _socket.Disconnect();
         return sData;
      }

      public bool Send(string sFrom, List<string> lstRecipients, string sSubject, string sBody)
      {
         if (!_socket.Connect(_ipaddress, _port))
            return false;

         // Receive welcome message.
         string sData = _socket.Receive();

         _socket.Send("HELO 127.0.0.1\r\n");
         sData = _socket.Receive();

         // User
         _socket.Send("MAIL FROM:<" + sFrom + ">\r\n");
         sData = _socket.Receive();

         string sCommaSeparatedRecipients = "";
         foreach (string sRecipient in lstRecipients)
         {
            _socket.Send("RCPT TO:<" + sRecipient + ">\r\n");
            sData = _socket.Receive();

            if (!sData.StartsWith("250"))
               return false;

            if (sCommaSeparatedRecipients.Length > 0)
               sCommaSeparatedRecipients += ", ";

            sCommaSeparatedRecipients += sRecipient;
         }

         // Select inbox
         _socket.Send("DATA\r\n");
         sData = _socket.Receive();

         if (sCommaSeparatedRecipients.Length > 5000)
            sCommaSeparatedRecipients = sCommaSeparatedRecipients.Substring(0, 5000);

         _socket.Send("From: " + sFrom + "\r\n");
         _socket.Send("To: " + sCommaSeparatedRecipients + "\r\n");
         _socket.Send("Subject: " + sSubject + "\r\n");
         _socket.Send("Date: " + TestSetup.GetCurrentMIMEDateTime() + "\r\n");

         _socket.Send("\r\n");

         // Send body

         _socket.Send(sBody);

         _socket.Send("\r\n");
         _socket.Send(".\r\n");

         // Wait for OK.
         sData = _socket.Receive();
         if (sData.Substring(0, 3) != "250")
            return false;

         // Quit again
         _socket.Send("QUIT\r\n");
         sData = _socket.Receive();

         _socket.Disconnect();

         return true;
      }

      public bool Send(string sFrom, string sTo, string sSubject, string sBody)
      {
         string result = "";

         return Send(sFrom, sTo, sSubject, sBody, out result);
      }

      public bool Send(string sFrom, string sTo, string sSubject, string sBody, out string result)
      {
         _socket.Connect(_ipaddress, _port);

         // Receive welcome message.
         string sData = _socket.Receive();

         _socket.Send("HELO example.com\r\n");
         sData = _socket.Receive();

         // User
         _socket.Send("MAIL FROM:<" + sFrom + ">\r\n");
         sData = _socket.Receive();

         _socket.Send("RCPT TO:<" + sTo + ">\r\n");
         sData = _socket.Receive();
         if (sData.StartsWith("2") == false)
         {
            result = sData;
            return false;
         }

         // Select inbox
         _socket.Send("DATA\r\n");
         sData = _socket.Receive();

         _socket.Send("From: " + sFrom + "\r\n");
         _socket.Send("To: " + sTo + "\r\n");
         _socket.Send("Subject: " + sSubject + "\r\n");

         _socket.Send("\r\n");

         // Send body

         _socket.Send(sBody);

         _socket.Send("\r\n");
         _socket.Send(".\r\n");

         // Wait for OK.
         sData = _socket.Receive();
         if (sData.Substring(0, 3) != "250")
         {
            result = sData;
            return false;
         }

         // Quit again
         _socket.Send("QUIT\r\n");
         sData = _socket.Receive();

         _socket.Disconnect();

         result = "";
         return true;
      }

      public bool SendRaw(string sFrom, string sTo, string text)
      {
         if (!_socket.Connect(_port))
            return false;

         // Receive welcome message.
         string sData = _socket.Receive();

         _socket.Send("HELO 127.0.0.1\r\n");
         sData = _socket.Receive();

         // User
         _socket.Send("MAIL FROM:<" + sFrom + ">\r\n");
         sData = _socket.Receive();

         _socket.Send("RCPT TO:<" + sTo + ">\r\n");
         sData = _socket.Receive();
         if (sData.StartsWith("2") == false)
            return false;

         // Send the message.
         _socket.Send("DATA\r\n");
         sData = _socket.Receive();

         _socket.Send(text);

         _socket.Send("\r\n");
         _socket.Send(".\r\n");

         // Wait for OK.
         sData = _socket.Receive();

         bool success = sData.Substring(0, 3) == "250";
         if (!success)
            return false;

         // Quit again
         _socket.Send("QUIT\r\n");
         sData = _socket.Receive();

         _socket.Disconnect();

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