// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../common/TCPIP/TCPConnection.h"

namespace HM
{
   class FetchAccount;
   class Message;
   class TransparentTransmissionBuffer;
   class MimeHeader;
   class Result;
   class FetchAccountUIDList;

   class POP3ClientConnection : 
      public TCPConnection
   {
   public:
      POP3ClientConnection(std::shared_ptr<FetchAccount> pAccount, 
         ConnectionSecurity connectionSecurity,
         boost::asio::io_service& io_service, 
         boost::asio::ssl::context& context,
         std::shared_ptr<Event> disconnected,
         AnsiString remote_hostname);
      ~POP3ClientConnection(void);

      virtual void ParseData(const AnsiString &Request);
      virtual void ParseData(std::shared_ptr<ByteBuffer> pBuf);

      
      virtual AnsiString GetCommandSeparator() const;

      void OnCouldNotConnect(const AnsiString &sErrorDescription);

   protected:

     virtual void OnConnected();
     virtual void OnHandshakeCompleted();
     virtual void OnHandshakeFailed() {};
     virtual void OnConnectionTimeout();
     virtual void OnExcessiveDataReceived();

      void EnqueueWrite_(const String &sData) ;
   
   // This is temp function to log ETRN client commands to SMTP
      void EnqueueWrite_LogAsSMTP(const String &sData) ;

   private:

      void SendCAPA_();
      bool HandleEtrn_(const String &sRequest, const String &account_name);
      int GetDaysToKeep_(const String &sUID);
      void FireOnExternalAccountDownload_(std::shared_ptr<Message> message, const String &uid);

      void HandlePOP3FinalizationTaskCompleted_();

      bool InternalParseData(const String &sRequest);

      void CreateRecipentList_(std::shared_ptr<MimeHeader> pHeader);

      // Checks whether the POP3 command hMailServer sent
      // to the remote server was successful.
      bool CommandIsSuccessfull_(const String &sData);

      // Logs a line in the POP3 log.
      void LogPOP3String_(const String &sLogString, bool bSent);

      // This is temp function to log ETRN client commands to SMTP
      void LogSMTPString_(const String &sLogString, bool bSent);

      void ParseStateConnected_(const String &sData);
      void ParseStateCAPASent_(const String &sData);
      bool ParseStateSTLSSent_(const String &sData);
      void ParseUsernameSent_(const String &sData);
      void ParsePasswordSent_(const String &sData);
      void ParseUIDLResponse_(const String &sData);
      void ParseRETRResponse_(const String &sData);
      bool ParseQuitResponse_(const String &sData);
      void ParseDELEResponse_(const String &sData);
      bool RequestNextMessage_();

      bool ParseFirstBinary_(std::shared_ptr<ByteBuffer> pBuf);
      void ProcessMIMERecipients_(std::shared_ptr<MimeHeader> pHeader);
      void ProcessReceivedHeaders_(std::shared_ptr<MimeHeader> pHeader);

      void RetrieveReceivedDate_(std::shared_ptr<MimeHeader> pHeader);

      void PrependHeaders_();
      // Adds headers to the beginning of the message.

      void QuitNow_();
      // Sends a QUIT message and switch over to
      // quit-state

      std::shared_ptr<FetchAccountUIDList> GetUIDList_();

      void MarkCurrentMessageAsRead_();
      void ParseMessageHeaders_();
      void SaveMessage_();
      bool DoSpamProtection_();
      void SendUserName_();
      void StartMailboxCleanup_();
      // Triggers a clean up start.

      void MailboxCleanup_();
      // Cleans up the entire mailbox

      bool MessageCleanup_();
      // Cleans up the current message.

      void DeleteUIDsNoLongerOnServer_();
      // Deletes the UID's in the local database if 
      // the UID does not exist on the POP3 server.

      std::shared_ptr<FetchAccount> account_;
      // The current fetch account.

      void RemoveInvalidRecipients_();

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

      std::map<int ,String> uidlresponse_;
      // The messages on the server (id,UID)

      std::map<int ,String> downloaded_messages_;
      // Messages which have been downloaded from the remote server.

      std::map<int ,String>::iterator cur_message_;

      std::shared_ptr<Message> current_message_;

      String receiving_account_address_;

      std::shared_ptr<TransparentTransmissionBuffer> transmission_buffer_;

      std::map<String, std::shared_ptr<Result> > event_results_;

      std::shared_ptr<FetchAccountUIDList> fetch_account_uidlist_;

      std::shared_ptr<ByteBuffer> _firstRetrResponseBuffer;
  };
}