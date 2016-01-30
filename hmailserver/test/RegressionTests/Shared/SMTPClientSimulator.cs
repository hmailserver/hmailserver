// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Net;
using System.Security.Authentication;
using System.Text;
using NUnit.Framework;

namespace RegressionTests.Shared
{
   /// <summary>
   /// Summary description for SMTPSimulator.
   /// </summary>
   public class SmtpClientSimulator
   {
      private readonly IPAddress _ipaddress;
      private readonly int _port = 25;
      private readonly TcpConnection _tcpConnection;

      public SmtpClientSimulator() :
         this(false, 25)
      {
      }

      public SmtpClientSimulator(bool useSSL, int port) :
         this(useSSL, port, IPAddress.Parse("127.0.0.1"))
      {

      }

      public SmtpClientSimulator(bool useSSL, int port, IPAddress ipaddress) :
         this(useSSL, SslProtocols.Default, port, ipaddress)
      {

      }

      public SmtpClientSimulator(bool useSSL, SslProtocols sslProtocols, int port, IPAddress ipaddress)
      {
         _tcpConnection = new TcpConnection(useSSL, sslProtocols);
         _port = port;
         _ipaddress = ipaddress;
      }


      public void ConnectAndLogon(string base64Username, string base64Password, out string errorMessage)
      {
         _tcpConnection.Connect(_port);

         errorMessage = Receive();
         if (!errorMessage.StartsWith("220"))
            throw new AuthenticationException("Error when connecting: " + errorMessage);

         if (!Logon(base64Username, base64Password, out errorMessage))
            throw new AuthenticationException("Logon failed: " + errorMessage);
      }

      private bool Logon(string base64Username, string base64Password, out string errorMessage)
      {
         errorMessage = SendAndReceive("EHLO test\r\n");
         if (!errorMessage.StartsWith("250"))
            return false;

         errorMessage = SendAndReceive("AUTH LOGIN\r\n");
         if (!errorMessage.StartsWith("334"))
            return false;

         ;
         errorMessage = SendAndReceive(base64Username + "\r\n");
         if (!errorMessage.StartsWith("334"))
            return false;

         errorMessage = SendAndReceive(base64Password + "\r\n");
         if (!errorMessage.StartsWith("235"))
            return false;
         
         return true;
      }

      public void Disconnect()
      {
         _tcpConnection.Disconnect();
      }

      public string Receive()
      {
         return _tcpConnection.Receive();
      }

      public string SendAndReceive(string sData)
      {
         _tcpConnection.Send(sData);
         return Receive();
      }

      public string GetWelcomeMessage()
      {
         _tcpConnection.Connect(_port);
         string sData = _tcpConnection.Receive();

         _tcpConnection.Disconnect();
         return sData;
      }

      public void Send(string sFrom, List<string> lstRecipients, string sSubject, string sBody)
      {
         if (!_tcpConnection.Connect(_ipaddress, _port))
         {
            throw new DeliveryFailedException("Failed to connect to server");
         }

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
               throw new DeliveryFailedException("Unexpected response from server: " + sData);

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
         _tcpConnection.Send("Date: " + GetCurrentMIMEDateTime() + "\r\n");

         _tcpConnection.Send("\r\n");

         // Send body

         _tcpConnection.Send(sBody);

         _tcpConnection.Send("\r\n");
         _tcpConnection.Send(".\r\n");

         // Wait for OK.
         sData = _tcpConnection.Receive();
         if (sData.Substring(0, 3) != "250")
            throw new DeliveryFailedException("Unexpected response from server: " + sData);

         // Quit again
         _tcpConnection.Send("QUIT\r\n");
         
         sData = _tcpConnection.Receive();

         _tcpConnection.Disconnect();

         
      }

      public void Send(string sFrom, string sTo, string sSubject, string sBody)
      {
         string result = "";

         Send(false, "", "", sFrom, sTo, sSubject, sBody, out result);
      }

      public void Send(string sFrom, string sTo, string sSubject, string sBody, out string result)
      {
         Send(false, "", "", sFrom, sTo, sSubject, sBody, out result);
      }

      public void Send(bool useStartTls, string username, string password, string sFrom, string sTo, string sSubject, string sBody, out string errorMessage)
      {
         string sData;

         if (!_tcpConnection.Connect(_ipaddress, _port))
            throw new DeliveryFailedException("Unable to connect to server.");

         // Receive welcome message.
         _tcpConnection.Receive();

         if (useStartTls)
         {
            var capabilities1 = SendAndReceive("EHLO example.com\r\n");

            if (!capabilities1.Contains("STARTTLS"))
               throw new DeliveryFailedException("Server does not support STARTTLS.");

            SendAndReceive("STARTTLS\r\n");

            _tcpConnection.HandshakeAsClient();
         }

         if (!string.IsNullOrEmpty(username))
         {
            if (!Logon(EncodeBase64(username), EncodeBase64(password), out errorMessage))
               throw new DeliveryFailedException("Login failed: " + errorMessage);
         }

         _tcpConnection.Send("HELO example.com\r\n");
         sData = _tcpConnection.Receive();

         // User
         _tcpConnection.Send("MAIL FROM:<" + sFrom + ">\r\n");
         sData = _tcpConnection.Receive();

         _tcpConnection.Send("RCPT TO:<" + sTo + ">\r\n");
         sData = _tcpConnection.Receive();
         if (sData.StartsWith("2") == false)
         {
            errorMessage = TrimNewlline(sData);
            throw new DeliveryFailedException("Unexpected response from server: " + errorMessage);
         }

         // Select inbox
         _tcpConnection.Send("DATA\r\n");
         sData = _tcpConnection.Receive();
         if (sData.Substring(0, 3) != "354")
         {
            errorMessage = TrimNewlline(sData);
            throw new DeliveryFailedException("Unexpected response from server: " + errorMessage);
         }


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
            errorMessage = TrimNewlline(sData);
            throw new DeliveryFailedException("Unexpected response from server: " + errorMessage);
         }

         // Quit again
         _tcpConnection.Send("QUIT\r\n");
         _tcpConnection.Receive();

         _tcpConnection.Disconnect();

         errorMessage = "";
      }

      private string TrimNewlline(string input)
      {
         return input.TrimEnd('\r', '\n');
      }

      public void SendRaw(string sFrom, string sTo, string text)
      {
         if (!_tcpConnection.Connect(_port))
            throw new DeliveryFailedException("Unable to connect.");

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
            throw new DeliveryFailedException("Unexpected response from server: " + sData);

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
            throw new DeliveryFailedException("Unexpected response from server: " + sData);

         // Quit again
         _tcpConnection.Send("QUIT\r\n");
         sData = _tcpConnection.Receive();

         _tcpConnection.Disconnect();

      }

      public static void StaticSend(string sFrom, List<string> lstRecipients, string sSubject, string sBody)
      {
         var oSimulator = new SmtpClientSimulator();
         oSimulator.Send(sFrom, lstRecipients, sSubject, sBody);
      }

      public static void StaticSendRaw(string sFrom, string recipient, string sBody)
      {
         var oSimulator = new SmtpClientSimulator();
         oSimulator.SendRaw(sFrom, recipient, sBody);
      }

      public static void StaticSend(string sFrom, string recipient, string sSubject, string sBody)
      {
         var messageRecipients = new List<string>();
         messageRecipients.Add(recipient);

         var oSimulator = new SmtpClientSimulator();
         oSimulator.Send(sFrom, messageRecipients, sSubject, sBody);
      }

      private string EncodeBase64(string s)
      {
         byte[] bytes = Encoding.UTF8.GetBytes(s);
         return Convert.ToBase64String(bytes);
      }



      private static string GetCurrentMIMEDateTime()
      {
         DateTime now = DateTime.Now;

         string dayOfWeek = "";
         switch (now.DayOfWeek)
         {
            case DayOfWeek.Monday:
               dayOfWeek = "Mon";
               break;
            case DayOfWeek.Tuesday:
               dayOfWeek = "Tue";
               break;
            case DayOfWeek.Wednesday:
               dayOfWeek = "Wed";
               break;
            case DayOfWeek.Thursday:
               dayOfWeek = "Thu";
               break;
            case DayOfWeek.Friday:
               dayOfWeek = "Fri";
               break;
            case DayOfWeek.Saturday:
               dayOfWeek = "Sat";
               break;
            case DayOfWeek.Sunday:
               dayOfWeek = "Sun";
               break;
         }

         string monthName = "";
         switch (now.Month)
         {
            case 1:
               monthName = "Jan";
               break;
            case 2:
               monthName = "Feb";
               break;
            case 3:
               monthName = "Mar";
               break;
            case 4:
               monthName = "Apr";
               break;
            case 5:
               monthName = "May";
               break;
            case 6:
               monthName = "Jun";
               break;
            case 7:
               monthName = "Jul";
               break;
            case 8:
               monthName = "Aug";
               break;
            case 9:
               monthName = "Sep";
               break;
            case 10:
               monthName = "Oct";
               break;
            case 11:
               monthName = "Nov";
               break;
            case 12:
               monthName = "Dec";
               break;
         }

         string timeString = now.ToString("HH':'mm':'ss");
         string dateString = string.Format("{0}, {1} {2} {3} {4} +0100", dayOfWeek, now.Day, monthName, now.Year,
                                           timeString);

         return dateString;
      }

      
   }
}