// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using hMailServer;
using RegressionTests.SSL;

namespace RegressionTests.Shared
{
   internal enum SimulatedErrorType
   {
      None,
      DisconnectAfterSessionStart,
      Sleep15MinutesAfterSessionStart,
      DisconnectAfterWelcomeBannerSent,
      ForceAuthenticationFailure,
      DisconnectAfterDeliveryStarted,
      DisconnectWithoutReplyOnQuit,
      DisconnectAfterMessageAccept
   }

   internal class SmtpServerSimulator : TcpServer
   {
      private readonly List<Dictionary<string, int>> _recipientResults;
      private Dictionary<string, int> _currentRecipientResult;
      private bool _expectingPassword;
      private bool _expectingUsername;
      private bool _hasMailFrom;
      private int _mailFromresult = 250;
      private int _quitResult = 221;

      private bool _transmittingData;


      public SmtpServerSimulator(int maxNumberOfConnections, int port, eConnectionSecurity connectionSecurity) :
         base(maxNumberOfConnections, port, connectionSecurity)
      {
         _recipientResults = new List<Dictionary<string, int>>();
         ServerSupportsEhlo = true;
         ServerSupportsHelo = true;
      }

      public SmtpServerSimulator(int maxNumberOfConnections, int port) :
         this(maxNumberOfConnections, port, eConnectionSecurity.eCSNone)
      {
      }

      public bool ServerSupportsEhlo { get; set; }
      public bool ServerSupportsHelo { get; set; }

      public int RcptTosReceived { get; set; }

      public string MailFrom { get; private set; } = "";
      // The envelope sender of the last message the server was offered, without its
      // angle brackets. Empty for a bounce, which is sent with no sender at all.

      public List<string> RcptTos { get; } = new List<string>();

      public int QuitResult
      {
         set => _quitResult = value;
      }

      public SimulatedErrorType SimulatedError { set; get; }

      public int MailFromResult
      {
         set => _mailFromresult = value;
      }

      public string MessageData { get; private set; } = "";

      public void AddRecipientResult(Dictionary<string, int> result)
      {
         _recipientResults.Add(result);
      }

      protected override void HandleClient()
      {
         _currentRecipientResult = _recipientResults[0];
         _recipientResults.Remove(_currentRecipientResult);

         Run();
      }

      public void Run()
      {
         if (SimulatedError == SimulatedErrorType.DisconnectAfterSessionStart)
            return;
         if (SimulatedError == SimulatedErrorType.Sleep15MinutesAfterSessionStart)
            Thread.Sleep(TimeSpan.FromMinutes(15));


         Send("220 ESMTP Test Server\r\n");

         if (SimulatedError == SimulatedErrorType.DisconnectAfterSessionStart)
            return;

         while (true)
         {
            var text = ReadUntil("\r\n");

            if (string.IsNullOrEmpty(text))
               break;

            var quit = ProcessCommand(text);

            if (quit)
               break;
         }
      }


      private bool ProcessCommand(string command)
      {
         if (ServerSupportsHelo && command.ToUpper().StartsWith("HELO"))
         {
            Send("250 Test Server - Helo\r\n");
            return false;
         }

         if (ServerSupportsEhlo && command.ToUpper().StartsWith("EHLO"))
         {
            var response = new StringBuilder();

            if (_connectionSecurity == eConnectionSecurity.eCSSTARTTLSRequired ||
                _connectionSecurity == eConnectionSecurity.eCSSTARTTLSOptional)
               response.AppendLine("250-STARTTLS");

            response.AppendLine("250 AUTH LOGIN PLAIN");

            Send(response.ToString());
            return false;
         }

         if (command.ToUpper().StartsWith("STARTTLS"))
         {
            Send("220 Ready to start TLS\r\n");
            _tcpConnection.HandshakeAsServer(SslSetup.GetCertificate());
            return false;
         }

         if (command.ToUpper().StartsWith("AUTH LOGIN"))
         {
            if (_connectionSecurity == eConnectionSecurity.eCSSTARTTLSRequired &&
                !_tcpConnection.IsSslConnection)
            {
               Send("503 STARTTLS required..\r\n");
               return false;
            }

            Send("334 VXNlcm5hbWU6\r\n");
            _expectingUsername = true;
            return false;
         }

         if (command.ToUpper().StartsWith("MAIL"))
         {
            if (_connectionSecurity == eConnectionSecurity.eCSSTARTTLSRequired &&
                !_tcpConnection.IsSslConnection)
            {
               Send("503 STARTTLS required..\r\n");
               return false;
            }

            var mailFromStartPos = command.IndexOf("<") + 1;
            var mailFromEndPos = command.LastIndexOf(">");

            if (mailFromStartPos > 0 && mailFromEndPos >= mailFromStartPos)
               MailFrom = command.Substring(mailFromStartPos, mailFromEndPos - mailFromStartPos);

            Send(_mailFromresult + "\r\n");

            if (_mailFromresult == 250)
               _hasMailFrom = true;

            return false;
         }

         if (command.ToUpper().StartsWith("RCPT"))
         {
            if (!_hasMailFrom)
            {
               Send("503 must have sender first.\r\n");
               return false;
            }

            var StartPos = command.IndexOf("<") + 1;
            var EndPos = command.LastIndexOf(">");
            var length = EndPos - StartPos;

            var address = command.Substring(StartPos, length);

            if (!_currentRecipientResult.ContainsKey(address))
               throw new Exception("Unexpected address");

            var result = _currentRecipientResult[address].ToString();

            Send(result + " " + address + "\r\n");

            RcptTos.Add(address);
            RcptTosReceived++;

            return false;
         }

         if (command.ToUpper().StartsWith("DATA"))
         {
            Send("354 Test Server - Give it to me...\r\n");
            _transmittingData = true;
            MessageData = "";
            return false;
         }

         if (command.ToUpper().StartsWith("QUIT"))
         {
            if (SimulatedError != SimulatedErrorType.DisconnectWithoutReplyOnQuit)
               Send(_quitResult + " Test Server - Goodbye\r\n");

            Disconnect();

            return true;
         }

         if (_transmittingData)
         {
            if (SimulatedError == SimulatedErrorType.DisconnectAfterDeliveryStarted)
            {
               // We've received some message data. Disconnect!
               Disconnect();
               return true;
            }

            MessageData += command;

            if (MessageData.IndexOf("\r\n.\r\n") > 0)
            {
               // remove the ending...
               MessageData = MessageData.Replace("\r\n.\r\n", "\r\n");

               Send("250 Test Server - Queued for delivery\r\n");

               if (SimulatedError == SimulatedErrorType.DisconnectAfterMessageAccept)
               {
                  Disconnect();
                  return true;
               }

               _transmittingData = false;
               return false;
            }

            return false;
         }

         if (_expectingUsername)
         {
            _expectingUsername = false;
            Send("334 UGFzc3dvcmQ6\r\n");
            _expectingPassword = true;
            return false;
         }

         if (_expectingPassword)
         {
            if (SimulatedError == SimulatedErrorType.ForceAuthenticationFailure)
               Send("535 Authentication failed. Restarting authentication process.\r\n");
            else
               Send("235 authenticated.\r\n");

            _expectingPassword = false;

            return false;
         }


         var commandName = command.Substring(0, 4);

         Send(string.Format("550 Command {0} not recognized.\r\n", commandName));

         return false;
      }
   }
}