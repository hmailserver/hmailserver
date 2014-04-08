// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../common/TCPIP/ProtocolParser.h"

namespace HM
{
   class FetchAccount;
   class Message;
   class TransparentTransmissionBuffer;
   class MimeHeader;
   class Result;
   class FetchAccountUIDList;

   class POP3ClientConnection : public ProtocolParser, public boost::enable_shared_from_this<POP3ClientConnection>
   {
   public:
      POP3ClientConnection(shared_ptr<FetchAccount> pAccount);
      ~POP3ClientConnection(void);

      virtual void ParseData(const AnsiString &Request);
      virtual void ParseData(shared_ptr<ByteBuffer> pBuf);

      
      virtual AnsiString GetCommandSeparator() const;

      void OnCouldNotConnect(const AnsiString &sErrorDescription);

   protected:

     virtual void OnConnected();
     virtual void OnConnectionTimeout();
     virtual void OnExcessiveDataReceived();

      void _SendData(const String &sData) ;
   
   // This is temp function to log ETRN client commands to SMTP
      void _SendData2(const String &sData) ;

   private:

      int _GetDaysToKeep(const String &sUID);
      void _FireOnExternalAccountDownload(shared_ptr<Message> message, const String &uid);

      void _HandlePOP3FinalizationTaskCompleted();

      void InternalParseData(const String &sRequest);

      void _CreateRecipentList(shared_ptr<MimeHeader> pHeader);

      // Checks whether the POP3 command hMailServer sent
      // to the remote server was successful.
      bool _CommandIsSuccessfull(const String &sData);

      // Logs a line in the POP3 log.
      void _LogPOP3String(const String &sLogString, bool bSent) const;

      // This is temp function to log ETRN client commands to SMTP
      void _LogSMTPString(const String &sLogString, bool bSent) const;

      void _ParseStateConnected(const String &sData);
      void _ParseUsernameSent(const String &sData);
      void _ParsePasswordSent(const String &sData);
      void _ParseUIDLResponse(const String &sData);
      void _ParseRETRResponse(const String &sData);
      void _ParseQuitResponse(const String &sData);
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
      
      void _StartMailboxCleanup();
      // Triggers a clean up start.

      void _MailboxCleanup();
      // Cleans up the entire mailbox

      bool _MessageCleanup();
      // Cleans up the current message.

      void _DeleteUIDsNoLongerOnServer();
      // Deletes the UID's in the local database if 
      // the UID does not exist on the POP3 server.

      shared_ptr<FetchAccount> m_pAccount;
      // The current fetch account.

      void _RemoveInvalidRecipients();

      enum State
      {
         StateConnected = 1,
         StateUsernameSent = 2,
         StatePasswordSent = 3,
         StateUIDLRequestSent = 4,
         StateRETRSent = 5,
         StateDELESent = 6,
         StateQUITSent = 100
      };

      State m_eCurrentState;

      bool m_bAwaitingMultilineResponse;
      // True if the POP3ClientConnection class expects
      // a multi-line response from the server on the
      // other side.
     
      String m_sCommandBuffer;

      map<int ,String> m_mapUIDLResponse;
      // The messages on the server (id,UID)

      map<int ,String> m_mapDownloadedMessages;
      // Messages which have been downloaded from the remote server.

      map<int ,String>::iterator m_iterCurMessage;

      shared_ptr<Message> m_pCurrentMessage;

      String m_sReceivingAccountAddress;

      shared_ptr<TransparentTransmissionBuffer> m_pTransmissionBuffer;

      bool m_bPendingDisconnect;

      map<String, shared_ptr<Result> > _eventResults;

      shared_ptr<FetchAccountUIDList> _fetchAccountUIDList;

  };
}