// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../common/Util/File.h"
#include "../common/Util/TransparentTransmissionBuffer.h"
#include "../Common/TCPIP/TCPConnection.h"

namespace HM
{
   class Messages;
   class ByteBuffer;

   class POP3Connection : public TCPConnection
   {
   public:

      POP3Connection(ConnectionSecurity connection_security,
         boost::asio::io_service& io_service, 
         boost::asio::ssl::context& context);
	   virtual ~POP3Connection();

      virtual void ParseData(const AnsiString &Request);
      virtual void ParseData(std::shared_ptr<ByteBuffer> pBuffer) { };

      virtual void OnDataSent();

   protected:

      virtual void OnConnected();
      virtual AnsiString GetCommandSeparator() const;

      virtual void EnqueueWrite_(const String &sData) ;

      virtual void OnDisconnect();
      virtual void OnConnectionTimeout();
      virtual void OnExcessiveDataReceived();
      virtual void OnHandshakeCompleted();
      virtual void OnHandshakeFailed() {};

   private:


      enum ParseResult
      {
         ResultNormalResponse = 0,
         ResultStartSendMessage = 1,
         ResultDisconnect = 2,
         ResultStartTls = 3
      };

      enum POP3Command
      {
         INVALID = 0,
         NOOP = 1,
         USER = 2,
         PASS = 3,
         HELP = 4,
         QUIT = 5,
         STAT = 6,
         LIST = 7,
         RETR = 8,
         TOP = 9,
         RSET = 10,
         DELE = 11,
         UIDL = 12,
         CAPA = 13,
         STLS = 14
      };

      enum ConnectionState
      {
         AUTHORIZATION = 1,
         TRANSACTION = 2,
         UPDATE = 3,
      };

      ParseResult InternalParseData(const AnsiString &Request);

      POP3Command GetCommand(ConnectionState currentState, String command);

      void LogClientCommand_(const String &sClientData);
      void GetMailboxContents_(int &iNoOfMessages, __int64 &iTotalBytes);

      void SendBanner_();
      ParseResult ProtocolRETR_(const String &Parameter);
      bool ProtocolLIST_(const String &sParameter);
      bool ProtocolDELE_(const String &Parameter);
      void ProtocolUSER_(const String &Parameter);
      ParseResult ProtocolPASS_(const String &Parameter);
      bool ProtocolTOP_(const String &Parameter);
      bool ProtocolUIDL_(const String &Parameter);
      bool ProtocolSTAT_(const String &sParameter);
      void ProtocolRSET_();
      void ProtocolQUIT_();
      bool ProtocolSTLS_();
      void ProtocolCAPA_();

      bool SendFileHeader_(const String &sFilename, int iNoOfLines = 0);

      void SaveMailboxChanges_();
      void UnlockMailbox_();

      void StartSendFile_(std::shared_ptr<Message> message);
	  void ReadAndSend_();
      void ResetMailbox_();
      std::shared_ptr<Message> GetMessage_(unsigned int index);

      String username_;
      String password_;

      std::shared_ptr<const Account> account_;

      ConnectionState current_state_;

      std::vector<std::shared_ptr<Message>> messages_;
      TransparentTransmissionBuffer transmission_buffer_;

      bool pending_disconnect_;
      File current_file_;
   };
}
