// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../common/Util/TransparentTransmissionBuffer.h"
#include "../common/BO/Message.h"
#include "../common/TCPIP/AnsiStringConnection.h"

namespace HM
{
   class ByteBuffer;
   class MessageRecipient;
  
   class SMTPClientConnection : public AnsiStringConnection
   {
   public:
      SMTPClientConnection(ConnectionSecurity connection_security,
         boost::asio::io_service& io_service, 
         boost::asio::ssl::context& context,
         shared_ptr<Event> disconnected);
	   virtual ~SMTPClientConnection();

      void OnCouldNotConnect(const AnsiString &sErrorDescription);

      virtual void ParseData(const AnsiString &Request);
      virtual void ParseData(shared_ptr<ByteBuffer> ) {}
      int SetDelivery(shared_ptr<Message> pDelMsg, std::vector<shared_ptr<MessageRecipient> > &vecRecipients);
           
      void SetAuthInfo(const String &sUsername, const String &sPassword);

   protected:

      virtual void OnConnected();
      virtual void OnHandshakeCompleted();
      virtual AnsiString GetCommandSeparator() const;

      virtual void _SendData(const String &sData);
      virtual void OnConnectionTimeout();
      virtual void OnExcessiveDataReceived();
      virtual void OnDataSent();

   private:

      void _ProtocolStateHELOEHLO();
      void _ProtocolSendMailFrom();
      void _ProtocolHELOEHLOSent(const AnsiString &request);
      void _ProtocolMailFromSent();
      void _ProtocolRcptToSent(int code, const AnsiString &request);
      void _ProtocolData();
      

      bool InternalParseData(const AnsiString &Request);
  	   void _ReadAndSend();
	  
      bool IsPositiveCompletion(int iErrorCode);
      bool IsPermanentNegative(int lErrorCode);

      void _LogSentCommand(const String &sData);
      void _StartSendFile(const String &sFilename);

      void _SendQUIT();

      void _ProtocolSendUsername();
      void _ProtocolSendPassword();

      void _UpdateAllRecipientsWithError(int iErrorCode, const AnsiString &sResponse, bool bPreConnectError);
      void _UpdateRecipientWithError(int iErrorCode, const AnsiString &sResponse,shared_ptr<MessageRecipient> pRecipient, bool bPreConnectError);

      shared_ptr<MessageRecipient> _GetNextRecipient();
      void _UpdateSuccessfulRecipients();

      enum ConnectionState
      {
	      HELO = 1,
         HELOSENT = 9,
         EHLOSENT = 10,
         AUTHLOGINSENT = 11,
         USERNAMESENT = 12,
         PASSWORDSENT = 13,
         MAILFROMSENT = 3,
         RCPTTOSENT = 5,
         DATAQUESTION = 6,
         DATACOMMANDSENT = 7,
         SENDINGDATA = 13,
         DATASENT = 8,
         QUITSENT = 14,
         STARTTLSSENT = 15
      };

      void _SetState(ConnectionState eCurState);
  
      ConnectionState m_CurrentState;

      shared_ptr<Message> m_pDeliveryMessage;


      // These are the recipients which will hMailServer should
      // try to deliver to.
      std::vector<shared_ptr<MessageRecipient> > m_vecRecipients;

      // The actual recipients are the recipients we've sent RCPT TO
      // for and the remote server has said OK to.
      std::set<shared_ptr<MessageRecipient> > _actualRecipients;

      bool m_bUseSMTPAuth;

      String m_sUsername;
      String m_sPassword;

      unsigned int m_iCurRecipient;

      bool m_bSessionEnded;

      AnsiString m_sLastSentData;
      
      File _currentFile;   
      TransparentTransmissionBuffer _transmissionBuffer;

      AnsiString m_sMultiLineResponseBuffer;
   };
}
