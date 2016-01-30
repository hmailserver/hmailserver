// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.IO.Ports;
using hMailServer;
using RegressionTests.SSL;

namespace RegressionTests.Shared
{
   internal class Pop3ServerSimulator : TcpServer
   {
      private readonly List<string> _messages;
      private bool _disconnectImmediate;

      public enum BufferMode
      {
         Split = 0,
         SingleBuffer = 1,
         MessageAndTerminatonTogether = 2
      }

      
      public Pop3ServerSimulator(int maxNumberOfConnections, int port, List<string> messages) :
         this(maxNumberOfConnections, port, messages, eConnectionSecurity.eCSNone)
      {
         
      }

      public Pop3ServerSimulator(int maxNumberOfConnections, int port, List<string> messages, eConnectionSecurity connectionSecurity) :
         base(maxNumberOfConnections, port, connectionSecurity)
      {
         _messages = messages;
         DeletedMessages = new List<int>();
         _disconnectImmediate = false;
         SupportsUIDL = true;
         SendBufferMode = BufferMode.Split;
      }

      public List<int> DeletedMessages { get; set; }
      public List<int> RetrievedMessages { get; set; }
      public bool SupportsUIDL { get; set; }
      public bool DisconnectAfterRetrCompletion { get; set; }
      public BufferMode SendBufferMode { get; set; }

      public bool DisconnectImmediate
      {
         get { return _disconnectImmediate; }
         set { _disconnectImmediate = value; }
      }

      protected override void HandleClient()
      {
         Run();
      }

      public void Run()
      {
         Send("+OK SimulatedServer\r\n");

         DeletedMessages = new List<int>();
         RetrievedMessages = new List<int>();

         if (_disconnectImmediate)
            return;

         while (ProcessCommand(Receive()))
         {
         }
      }

      public bool ProcessCommand(string command)
      {
         if (command.ToLower().StartsWith("quit"))
         {
            // Remove the messages...
            DeletedMessages.Sort();
            DeletedMessages.Reverse();
            foreach (int deletedMessage in DeletedMessages)
            {
               _messages.RemoveAt(deletedMessage - 1);
            }

            return false;
         }

         if (command.ToLower().StartsWith("user"))
         {
            Send("+OK\r\n");
            return true;
         }

         if (command.ToLower().StartsWith("capa"))
         {
            string capabilities = "USER\r\nUIDL\r\nTOP\r\n";

            if (_connectionSecurity == eConnectionSecurity.eCSSTARTTLSRequired ||
                _connectionSecurity == eConnectionSecurity.eCSSTARTTLSOptional)
               capabilities += "STLS\r\n";

            string response = "+OK CAPA list follows\r\n" + capabilities + "." + "\r\n";
            Send(response);
            return true;
         }

         if (command.ToLower().StartsWith("stls"))
         {
            Send("+OK Begin TLS negotiation\r\n");
            _tcpConnection.HandshakeAsServer(SslSetup.GetCertificate());
            return true;
         }

         if (command.ToLower().StartsWith("pass"))
         {
            Send("+OK\r\n");
            return true;
         }

         if (command.ToLower().StartsWith("uidl"))
         {
            if (!SupportsUIDL)
            {
               Send("-ERR unhandled command\r\n");
               return true;
            }

            Send("+OK\r\n");

            for (int i = 0; i < _messages.Count; i++)
            {
               Send(string.Format("{0} UniqueID-{1}\r\n", i + 1, _messages[i].GetHashCode()));
            }

            Send(".\r\n");
            return true;
         }

         if (command.ToLower().StartsWith("retr"))
         {
            command = command.Substring(5);
            command = command.TrimEnd('\n');
            command = command.TrimEnd('\r');

            int messageID = Convert.ToInt32(command);

            RetrievedMessages.Add(messageID);

            string message = _messages[messageID - 1];

            switch (SendBufferMode)
            {
               case BufferMode.Split:
                  {
                     Send("+OK\r\n");
                     Send(message);
                     Send("\r\n.\r\n");
                     break;
                  }
               case BufferMode.SingleBuffer:
                  {
                     Send("+OK\r\n" + message + "\r\n.\r\n");
                     break;
                  }
               case BufferMode.MessageAndTerminatonTogether:
                  {
                     Send("+OK\r\n");
                     Send(message + "\r\n.\r\n");
                     break;
                  }
            }


            if (DisconnectAfterRetrCompletion)
               return false;

            return true;
         }

         if (command.ToLower().StartsWith("dele"))
         {
            command = command.Substring(5);
            command = command.TrimEnd('\n');
            command = command.TrimEnd('\r');

            int messageID = Convert.ToInt32(command);

            DeletedMessages.Add(messageID);

            Send("+OK\r\n");

            return true;
         }

         return true;
      }
   }
}