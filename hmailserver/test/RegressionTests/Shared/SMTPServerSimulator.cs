// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.IO.Ports;
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
      private string _messageData = "";
      private int _quitResult = 221;

      private SimulatedErrorType _simulatedError;
      private bool _transmittingData;

      public bool ServerSupportsEhlo { get; set; }
      public bool ServerSupportsHelo { get; set; }

      
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

      public int RcptTosReceived { get; set; }

      public int QuitResult
      {
         set { _quitResult = value; }
      }

      public SimulatedErrorType SimulatedError
      {
         set { _simulatedError = value; }
         get { return _simulatedError; }
      }

      public int MailFromResult
      {
         set { _mailFromresult = value; }
      }

      public string MessageData
      {
         get { return _messageData; }
      }

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
            string text = ReadUntil("\r\n");

            if (string.IsNullOrEmpty(text))
               break;

            bool quit = ProcessCommand(text);

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
            {
               response.AppendLine("250-STARTTLS");
            }

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

            Send(_mailFromresult.ToString() + "\r\n");

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
            int StartPos = command.IndexOf("<") + 1;
            int EndPos = command.LastIndexOf(">");
            int length = EndPos - StartPos;

            string address = command.Substring(StartPos, length);

            if (!_currentRecipientResult.ContainsKey(address))
               throw new Exception("Unexpected address");

            string result = _currentRecipientResult[address].ToString();

            Send(result + " " + address + "\r\n");
            
            RcptTosReceived++;

            return false;
         }

         if (command.ToUpper().StartsWith("DATA"))
         {
            Send("354 Test Server - Give it to me...\r\n");
            _transmittingData = true;
            _messageData = "";
            return false;
         }

         if (command.ToUpper().StartsWith("QUIT"))
         {
            if (_simulatedError != SimulatedErrorType.DisconnectWithoutReplyOnQuit)
            {
               Send(_quitResult.ToString() + " Test Server - Goodbye\r\n");
            }

            Disconnect();

            return true;
         }

         if (_transmittingData)
         {
            if (_simulatedError == SimulatedErrorType.DisconnectAfterDeliveryStarted)
            {
               // We've received some message data. Disconenct!
               Disconnect();
               return true;
            }
            
            _messageData += command;

            if (_messageData.IndexOf("\r\n.\r\n") > 0)
            {
               // remove the ending...
               _messageData = _messageData.Replace("\r\n.\r\n", "\r\n");

               Send("250 Test Server - Queued for delivery\r\n");

               if (_simulatedError == SimulatedErrorType.DisconnectAfterMessageAccept)
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