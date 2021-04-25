
// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once


#include "../common/TCPIP/TCPConnection.h"

#include "RecipientParser.h"

namespace HM
{

   class SMTPConfiguration;
   class Message;
   class Messages;
   class TransparentTransmissionBuffer;
   class MessageData;
   class Domain;
   class SpamTestResult;

   enum eSMTPCommandTypes
   {
      SMTP_COMMAND_UNKNOWN = 0,
      SMTP_COMMAND_HELO = 1001,  
      SMTP_COMMAND_HELP = 1002,  
      SMTP_COMMAND_QUIT = 1003,  
      SMTP_COMMAND_EHLO = 1004,  
      SMTP_COMMAND_AUTH = 1005,  
      SMTP_COMMAND_MAIL = 1006,  
      SMTP_COMMAND_RCPT = 1007,  
      SMTP_COMMAND_TURN = 1008,  
      SMTP_COMMAND_VRFY = 1009,
      SMTP_COMMAND_DATA = 1010,
      SMTP_COMMAND_RSET = 1011,
      SMTP_COMMAND_NOOP = 1012,
      SMTP_COMMAND_ETRN = 1013,
      SMTP_COMMAND_STARTTLS = 1014
   };

   class SMTPConnection : public TCPConnection
   {
   public:
      SMTPConnection(ConnectionSecurity connection_security,
         boost::asio::io_service& io_service, 
         boost::asio::ssl::context& context);
	   virtual ~SMTPConnection();
      
   protected:

      virtual void OnConnected();
      virtual void OnHandshakeCompleted();
      virtual void OnHandshakeFailed() {};
      virtual AnsiString GetCommandSeparator() const;

      virtual void ParseData(const AnsiString &sRequest);
      virtual void ParseData(std::shared_ptr<ByteBuffer> pBuf);

      virtual void OnConnectionTimeout();
      virtual void OnExcessiveDataReceived();

   private:

      bool CheckStartTlsRequired_();
      void EnqueueWrite_(const String &sData);
      void SendBanner_();

      bool ParseAddressWithExtensions_(String mailFrom, String &address, String &parameters);
      void HandleSMTPFinalizationTaskCompleted_();

      virtual void InternalParseData(const AnsiString &sRequest);

      enum SpamProtectionType
      {
         SPNone = 0,
         SPPreTransmission = 1,
         SPPostTransmission = 2
      };
         
      enum Constants
      {
         MaxNumberOfRecipients = 50000
      };

      void InitializeSpamProtectionType_(const String &sFromAddress);

      bool CheckLineEndings_() const;

      void LogClientCommand_(const String &sClientData);

      void LogAwstatsMessageRejected_();
      
      bool DoSpamProtection_(SpamProtectionType spType, const String &sFromAddress, const String &hostName, const IPAddress &lIPAddress);
      // Does IP based spam protection. Returns true if we should
      // continue delivery, false otherwise.

      void ResetCurrentMessage_();
      bool CheckIfValidSenderAddress(const String &sFromAddress);
      
      bool ReAuthenticateUser();

      void AppendMessageHeaders_();

      eSMTPCommandTypes GetCommandType_(const String &sType);

      void DoPreAcceptMessageModifications_();
      // Make changes to the message before it's accepted for delivery. This is
      // for example where message signature and spam-headers are added.

      void SetMessageSignature_(std::shared_ptr<MessageData> &pMessageData);
      // Sets the signature of the message, based on the signature in the account
      // settings and domain settings.

      bool OnPreAcceptTransfer_();
      bool DoPreAcceptSpamProtection_();

      void ProtocolEHLO_(const String &sRequest);
      void ProtocolHELO_(const String &sRequest);
      void ProtocolAUTH_(const String &sRequest);
      void ProtocolNOOP_();
      void ProtocolRSET_();

      bool LookupRoute_(const String &sToAddress, bool &bDomainIsRemote);
      void ProtocolMAIL_(const String &Request);
      void ProtocolQUIT_();
      void ProtocolHELP_();
      void ProtocolRCPT_(const String &Request);
      void ProtocolETRN_(const String &sRequest);
      void ProtocolSTARTTLS_(const String &sRequest);
      void ProtocolUsername_(const String &sRequest);
      void ProtocolPassword_(const String &sRequest);
      void ProtocolDATA_();

      void ReportUnsupportedEsmtpExtension_(const String &parameter);

      void AuthenticateUsingPLAIN_(const String &sLine);
      // Authenticates using a PLAIN line.

      void Authenticate_();
      // validates the username and password.

      void RestartAuthentication_();
      // restarts the authentication process.
      
      void ResetLoginCredentials_();
      // restarts the authentication process.

      bool SendEHLOKeywords_();

      int GetMaxMessageSize_(std::shared_ptr<const Domain> pDomain);

      bool ReadDomainAddressFromHelo_(const String &sRequest);

      void SendErrorResponse_(int iErrorCode, const String &sResponse);

      bool GetDoSpamProtection_();

      bool GetIsLocalSender_();

      bool GetAuthIsEnabled_();

      void HandleUnableToSaveMessageDataFile_(const String &file_name);

      String GetSpamTestResultMessage_(std::set<std::shared_ptr<SpamTestResult> > testResult) const;



      enum ConnectionState
      {
         INITIAL = 1,
         SMTPUSERNAME = 3,
         SMTPUPASSWORD = 4,
         HEADER = 5,
         DATA = 6,
         STARTTLS = 7
      };
  
      enum AuthenticationType
      {
         AUTH_NONE = 0,
         AUTH_PLAIN = 2,
         AUTH_LOGIN = 3,
      };

      

      ConnectionState current_state_;

      std::shared_ptr<Message> current_message_;

      bool trace_headers_written_;

      String username_;
      String password_;

      std::shared_ptr<SMTPConfiguration> smtpconf_;
   
      AuthenticationType requestedAuthenticationType_;
      
      DWORD message_start_tc_;

      size_t max_message_size_kb_;
      // Maximum message size in KB.

      String helo_host_;

      std::shared_ptr<TransparentTransmissionBuffer> transmission_buffer_;

      // Spam detection 
      bool rejected_by_delayed_grey_listing_;
      int cur_no_of_rcptto_;
      int cur_no_of_invalid_commands_;
      
      std::shared_ptr<const Domain> sender_domain_;
      std::shared_ptr<const Account> sender_account_;

      std::set<std::shared_ptr<SpamTestResult> > spam_test_results_;

      bool re_authenticate_user_;
      bool pending_disconnect_;
      bool isAuthenticated_;
      SpamProtectionType type_;

      RecipientParser recipientParser_;
      bool start_tls_used_;
   };
}
