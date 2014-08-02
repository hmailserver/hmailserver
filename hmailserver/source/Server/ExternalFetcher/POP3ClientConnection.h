// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../common/TCPIP/AnsiStringConnection.h"

namespace HM
{
   class FetchAccount;
   class Message;
   class TransparentTransmissionBuffer;
   class MimeHeader;
   class Result;
   class FetchAccountUIDList;

   class POP3ClientConnection : 
      public AnsiStringConnection
   {
   public:
      POP3ClientConnection(shared_ptr<FetchAccount> pAccount, 
         ConnectionSecurity connectionSecurity,
         boost::asio::io_service& io_service, 
         boost::asio::ssl::context& context,
         shared_ptr<Event> disconnected);
      ~POP3ClientConnection(void);

      virtual void ParseData(const AnsiString &Request);
      virtual void ParseData(shared_ptr<ByteBuffer> pBuf);

      
      virtual AnsiString GetCommandSeparator() const;

      void OnCouldNotConnect(const AnsiString &sErrorDescription);

   protected:

     virtual void OnConnected();
     virtual void OnHandshakeCompleted();
     virtual void OnHandshakeFailed() {};
     virtual void OnConnectionTimeout();
     virtual void OnExcessiveDataReceived();

      void _SendData(const String &sData) ;
   
   // This is temp function to log ETRN client commands to SMTP
      void _SendDataLogAsSMTP(const String &sData) ;

   private:

      void _SendCAPA();
      bool _HandleEtrn(const String &account_name);
      int _GetDaysToKeep(const String &sUID);
      void _FireOnExternalAccountDownload(shared_ptr<Message> message, const String &uid);

      void _HandlePOP3FinalizationTaskCompleted();

      bool InternalParseData(const String &sRequest);

      void _CreateRecipentList(shared_ptr<MimeHeader> pHeader);

      // Checks whether the POP3 command hMailServer sent
      // to the remote server was successful.
      bool _CommandIsSuccessfull(const String &sData);

      // Logs a line in the POP3 log.
      void _LogPOP3String(const String &sLogString, bool bSent);

      // This is temp function to log ETRN client commands to SMTP
      void _LogSMTPString(const String &sLogString, bool bSent);

      void _ParseStateConnected(const String &sData);
      void _ParseStateCAPASent(const String &sData);
      bool _ParseStateSTLSSent(const String &sData);
      void _ParseUsernameSent(const String &sData);
      void _ParsePasswordSent(const String &sData);
      void _ParseUIDLResponse(const String &sData);
      void _ParseRETRResponse(const String &sData);
      bool _ParseQuitResponse(const String &sData);
      void _ParseDELEResponse(const String &sData);
      bool _RequestNextMessage();

      bool _ParseFirstBinary(shared_ptr<ByteBuffer> pBuf);
      void _ProcessMIMERecipients(shared_ptr<MimeHeader> pHeader);
      void _ProcessReceivedHeaders(shared_ptr<MimeHeader> pHeader);

      void _RetrieveReceivedDate(shared_ptr<MimeHeader> pHeader);

      void _PrependHeaders();
      // Adds headers to the beginning of the message.

      void _QuitNow();
      // Sends a QUIT message and switch over to
      // quit-state

      shared_ptr<FetchAccountUIDList> _GetUIDList();

      void _MarkCurrentMessageAsRead();
      void _ParseMessageHeaders();
      void _SaveMessage();
      bool _DoSpamProtection();
      void _SendUserName();
      void _StartMailboxCleanup();
      // Triggers a clean up start.

      void _MailboxCleanup();
      // Cleans up the entire mailbox

      bool _MessageCleanup();
      // Cleans up the current message.

      void _DeleteUIDsNoLongerOnServer();
      // Deletes the UID's in the local database if 
      // the UID does not exist on the POP3 server.

      shared_ptr<FetchAccount> account_;
      // The current fetch account.

      void _RemoveInvalidRecipients();

      enum State
      {
         StateConnected,
         StateCAPASent,
         StateSTLSSent,
         StateUsernameSent,
         StatePasswordSent,
         StateUIDLRequestSent,
         StateRETRSent,
         StateDELESent,
         StateQUITSent
      };

      State current_state_;
     
      AnsiString command_buffer_;

      map<int ,String> uidlresponse_;
      // The messages on the server (id,UID)

      map<int ,String> downloaded_messages_;
      // Messages which have been downloaded from the remote server.

      map<int ,String>::iterator cur_message_;

      shared_ptr<Message> current_message_;

      String receiving_account_address_;

      shared_ptr<TransparentTransmissionBuffer> transmission_buffer_;

      map<String, shared_ptr<Result> > _eventResults;

      shared_ptr<FetchAccountUIDList> _fetchAccountUIDList;

  };
}