// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using hMailServer;
using RegressionTests.SSL;

namespace RegressionTests.Shared
{
   public class Pop3ServerSimulator : TcpServer
   {
      public enum BufferMode
      {
         Split = 0,
         SingleBuffer = 1,
         MessageAndTerminatonTogether = 2
      }

      public enum OversizedResponseType
      {
         None = 0,
         Capa = 1,
         UidlLine = 2,
         UidlEntries = 3,
         UidlLongLines = 4,
         UidlTotalSize = 5,
         RetrFirstLine = 6
      }

      private readonly List<string> _messages;


      public Pop3ServerSimulator(int maxNumberOfConnections, int port, List<string> messages) :
         this(maxNumberOfConnections, port, messages, eConnectionSecurity.eCSNone)
      {
      }

      public Pop3ServerSimulator(int maxNumberOfConnections, int port, List<string> messages,
         eConnectionSecurity connectionSecurity) :
         base(maxNumberOfConnections, port, connectionSecurity)
      {
         _messages = messages;
         DeletedMessages = new List<int>();
         DisconnectImmediate = false;
         SupportsUIDL = true;
         SendBufferMode = BufferMode.Split;
      }

      public List<int> DeletedMessages { get; set; }
      public List<int> RetrievedMessages { get; set; }
      public bool SupportsUIDL { get; set; }
      public bool DisconnectAfterRetrCompletion { get; set; }
      public BufferMode SendBufferMode { get; set; }

      public bool DisconnectImmediate { get; set; }

      // Response which is never terminated, sent until OversizedResponseSize bytes are sent or the client disconnects.
      public OversizedResponseType OversizedResponse { get; set; }
      public long OversizedResponseSize { get; set; }

      // Extra UIDL entries with 70 character UIDs, listed after the real messages. RETR of one of them disconnects.
      public int PaddingUidCount { get; set; }
      public bool ClientDisconnectedDuringOversizedResponse { get; private set; }

      // UIDs returned by UIDL, one per message. When null, UIDs are generated.
      public List<string> Uids { get; set; }

      protected override void HandleClient()
      {
         Run();
      }

      public void Run()
      {
         Send("+OK SimulatedServer\r\n");

         DeletedMessages = new List<int>();
         RetrievedMessages = new List<int>();

         if (DisconnectImmediate)
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
            foreach (var deletedMessage in DeletedMessages) _messages.RemoveAt(deletedMessage - 1);

            return false;
         }

         if (command.ToLower().StartsWith("user"))
         {
            Send("+OK\r\n");
            return true;
         }

         if (command.ToLower().StartsWith("capa"))
         {
            if (OversizedResponse == OversizedResponseType.Capa)
               return SendOversizedResponse("+OK\r\n", i => new string('X', 1000) + "\r\n");

            var capabilities = "USER\r\nUIDL\r\nTOP\r\n";

            if (_connectionSecurity == eConnectionSecurity.eCSSTARTTLSRequired ||
                _connectionSecurity == eConnectionSecurity.eCSSTARTTLSOptional)
               capabilities += "STLS\r\n";

            var response = "+OK CAPA list follows\r\n" + capabilities + "." + "\r\n";
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

            if (OversizedResponse == OversizedResponseType.UidlLine)
               return SendOversizedResponse("+OK\r\n1 ", i => new string('X', 1000));

            if (OversizedResponse == OversizedResponseType.UidlEntries)
               return SendOversizedResponse("+OK\r\n", i => string.Format("{0} UID{0}\r\n", i + 1));

            // Complete lines, each longer than any valid UIDL line.
            if (OversizedResponse == OversizedResponseType.UidlLongLines)
               return SendOversizedResponse("+OK\r\n", i => string.Format("{0} {1}\r\n", i + 1, new string('X', 1000)));

            // Lines of about 200 bytes, so the total size is reached before the number of entries.
            if (OversizedResponse == OversizedResponseType.UidlTotalSize)
               return SendOversizedResponse("+OK\r\n", i => string.Format("{0} {1}\r\n", i + 1, ("UID" + (i + 1)).PadRight(190, 'X')));

            Send("+OK\r\n");

            var builder = new StringBuilder();

            for (var i = 0; i < _messages.Count; i++)
               builder.Append(string.Format("{0} {1}\r\n", i + 1,
                  Uids != null ? Uids[i] : "UniqueID-" + _messages[i].GetHashCode()));

            for (var i = _messages.Count + 1; i <= _messages.Count + PaddingUidCount; i++)
               builder.Append(string.Format("{0} {1}\r\n", i, ("Padding-" + i).PadRight(70, 'X')));

            Send(builder.ToString());

            Send(".\r\n");
            return true;
         }

         if (command.ToLower().StartsWith("retr"))
         {
            command = command.Substring(5);
            command = command.TrimEnd('\n');
            command = command.TrimEnd('\r');

            var messageID = Convert.ToInt32(command);

            RetrievedMessages.Add(messageID);

            if (OversizedResponse == OversizedResponseType.RetrFirstLine)
               return SendOversizedResponse("+OK ", i => new string('X', 1000));

            if (messageID > _messages.Count)
               return false;

            var message = _messages[messageID - 1];

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

            var messageID = Convert.ToInt32(command);

            DeletedMessages.Add(messageID);

            Send("+OK\r\n");

            return true;
         }

         return true;
      }

      // Sends start followed by line(0), line(1), ... in chunks of about 64 KB.
      private bool SendOversizedResponse(string start, Func<long, string> line)
      {
         try
         {
            _tcpConnection.Send(start);

            long lineIndex = 0;
            var chunk = new StringBuilder();

            // Send directly, since Send() would add all data to Conversation.
            for (long sent = 0; sent < OversizedResponseSize; sent += chunk.Length)
            {
               chunk.Clear();
               while (chunk.Length < 65536)
                  chunk.Append(line(lineIndex++));

               _tcpConnection.Send(chunk.ToString());
            }
         }
         catch (Exception e) when (e is IOException || e is InvalidOperationException)
         {
            ClientDisconnectedDuringOversizedResponse = true;
         }

         return false;
      }
   }
}
