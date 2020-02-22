// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../common/Util/TransparentTransmissionBuffer.h"
#include "../common/BO/Message.h"
#include "../common/TCPIP/TCPConnection.h"


namespace HM
{
   class ByteBuffer;
   class MessageRecipient;
  
   class SMTPClientConnection : public TCPConnection
   {
   public:
      SMTPClientConnection(ConnectionSecurity connection_security,
         boost::asio::io_service& io_service, 
         boost::asio::ssl::context& context,
         std::shared_ptr<Event> disconnected,
         AnsiString remote_hostname);
	   virtual ~SMTPClientConnection();

      void OnCouldNotConnect(const AnsiString &sErrorDescription);

      virtual void ParseData(const AnsiString &Request);
      virtual void ParseData(std::shared_ptr<ByteBuffer> ) {}
      int SetDelivery(std::shared_ptr<Message> pDelMsg, std::vector<std::shared_ptr<MessageRecipient> > &vecRecipients);
           
      void SetAuthInfo(const String &sUsername, const String &sPassword);
   protected:

      virtual void OnConnected();
      virtual void OnHandshakeCompleted();
      virtual void OnHandshakeFailed();
      virtual AnsiString GetCommandSeparator() const;

      virtual void EnqueueWrite_(const String &sData);
      virtual void OnConnectionTimeout();
      virtual void OnExcessiveDataReceived();
      virtual void OnDataSent();
      virtual void OnReadError(int errorCode);

   private:

      void LogReceivedResponse_(const String &response);

      void ProtocolStateHELOEHLO_(const AnsiString &request);
      void ProtocolSendMailFrom_();
      void ProtocolHELOSent_(const AnsiString &request);
      void ProtocolEHLOSent_(int code, const AnsiString &request);
      void ProtocolSTARTTLSSent_(int code);
      void ProtocolMailFromSent_();
      void ProtocolRcptToSent_(int code, const AnsiString &request);
      void ProtocolData_();

      void HandleHandshakeFailed_();
      bool InternalParseData(const AnsiString &Request);
  	   void ReadAndSend_();
	  
      bool IsPositiveCompletion(int iErrorCode);
      bool IsPermanentNegative(int lErrorCode);

      void LogSentCommand_(const String &sData);
      void StartSendFile_(const String &sFilename);

      void SendQUIT_();

      void ProtocolSendUsername_();
      void ProtocolSendPassword_();

      void UpdateAllRecipientsWithError_(int iErrorCode, const AnsiString &sResponse, bool bPreConnectError);
      void UpdateRecipientWithError_(int iErrorCode, const AnsiString &sResponse,std::shared_ptr<MessageRecipient> pRecipient, bool bPreConnectError);

      std::shared_ptr<MessageRecipient> GetNextRecipient_();
      void UpdateSuccessfulRecipients_();

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

      void SetState_(ConnectionState eCurState);
  
      ConnectionState current_state_;

      std::shared_ptr<Message> delivery_message_;


      // These are the recipients which will hMailServer should
      // try to deliver to.
      std::vector<std::shared_ptr<MessageRecipient> > recipients_;

      // The actual recipients are the recipients we've sent RCPT TO
      // for and the remote server has said OK to.
      std::set<std::shared_ptr<MessageRecipient> > actual_recipients_;

      bool use_smtpauth_;

      String username_;
      String password_;

      unsigned int cur_recipient_;

      bool session_ended_;

      AnsiString last_sent_data_;
      
      File current_file_;   
      TransparentTransmissionBuffer transmission_buffer_;

      AnsiString multi_line_response_buffer_;
   };
}
