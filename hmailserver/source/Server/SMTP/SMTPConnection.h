// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once


#include "../Common/TCPIP/ProtocolParser.h"
#include "RecipientParser.h"
#include "../Common/BO/Collection.h"
#include "../common/persistence/PersistentDomain.h"

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
      SMTP_COMMAND_ETRN = 1013
   };

   class SMTPConnection : public ProtocolParser, 
                          public boost::enable_shared_from_this<SMTPConnection>
   {
   public:
	   SMTPConnection();
	   virtual ~SMTPConnection();
      
   protected:

      virtual void OnConnected();
      virtual AnsiString GetCommandSeparator() const;

      virtual void ParseData(const AnsiString &sRequest);
      virtual void ParseData(shared_ptr<ByteBuffer> pBuf);

      virtual void _SendData(const String &sData);

      virtual void OnConnectionTimeout();
      virtual void OnExcessiveDataReceived();

   private:

      bool _TryExtractAddress(const String &mailFromParameter, String& address);
      void _HandleSMTPFinalizationTaskCompleted();

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

      void _InitializeSpamProtectionType(const String &sFromAddress);

      bool _CheckLineEndings() const;

      void _LogClientCommand(const String &sClientData) const;

      void _LogAwstatsMessageRejected();
      
      bool _DoSpamProtection(SpamProtectionType spType, const String &sFromAddress, const String &hostName, const IPAddress &lIPAddress);
      // Does IP based spam protection. Returns true if we should
      // continue delivery, false otherwise.

      void _ResetCurrentMessage();
      bool CheckIfValidSenderAddress(const String &sFromAddress);
      
      bool ReAuthenticateUser();

      void _AppendMessageHeaders();

      eSMTPCommandTypes _GetCommandType(const String &sType);

      void _DoPreAcceptMessageModifications();
      // Make changes to the message before it's accepted for delivery. This is
      // for example where message signature and spam-headers are added.

      void _SetMessageSignature(shared_ptr<MessageData> &pMessageData);
      // Sets the signature of the message, based on the signature in the account
      // settings and domain settings.

      bool _OnPreAcceptTransfer();
      bool _DoPreAcceptSpamProtection();

      void _ProtocolEHLO(const String &sRequest);
      void _ProtocolHELO(const String &sRequest);
      void _ProtocolAUTH(const String &sRequest);
      void _ProtocolNOOP();
      void _ProtocolRSET();

      bool _LookupRoute(const String &sToAddress, bool &bDomainIsRemote);
      bool _ProtocolMAIL(const String &Request);
      void _ProtocolQUIT();
      void _ProtocolHELP();
      void _ProtocolRCPT(const String &Request);
      void _ProtocolETRN(const String &sRequest);

      void _TarpitCheckDelay();

      void _AuthenticateUsingPLAIN(const String &sLine);
      // Authenticates using a PLAIN line.

      void _Authenticate();
      // validates the username and password.

      void _RestartAuthentication();
      // restarts the authentication process.
      
      void _ResetLoginCredentials();
      // restarts the authentication process.

      bool _SendEHLOKeywords();

      int _GetMaxMessageSize(shared_ptr<const Domain> pDomain);

      bool _ReadDomainAddressFromHelo(const String &sRequest);

      void _SendErrorResponse(int iErrorCode, const String &sResponse);

      bool _GetDoSpamProtection() const;

      bool _GetIsLocalSender();

      String _GetSpamTestResultMessage(set<shared_ptr<SpamTestResult> > testResult) const;

      enum ConnectionState
      {
         AUTHENTICATION = 1,
         SMTPSELECTAUTH = 2,
         SMTPUSERNAME = 3,
         SMTPUPASSWORD = 4,
         HEADER = 5,
         DATA = 6
      };
  
      enum AuthenticationType
      {
         AUTH_NONE = 0,
         AUTH_PLAIN = 2,
         AUTH_LOGIN = 3,
      };

      

      ConnectionState m_CurrentState;

      shared_ptr<Message> m_pCurrentMessage;

      bool m_bTraceHeadersWritten;

      String m_sUsername;
      String m_sPassword;

      shared_ptr<SMTPConfiguration> m_SMTPConf;
   
      AuthenticationType _requestedAuthenticationType;
      
      DWORD m_lMessageStartTC;

      int m_iMaxMessageSizeKB;
      // Maximum message size in KB.

      String m_sHeloHost;

      shared_ptr<TransparentTransmissionBuffer> m_pTransmissionBuffer;

      // Spam detection 
      bool m_bRejectedByDelayedGreyListing;
      int m_iCurNoOfRCPTTO;
      int m_iCurNoOfInvalidCommands;
      
      shared_ptr<const Domain> m_pSenderDomain;
      shared_ptr<const Account> m_pSenderAccount;

      set<shared_ptr<SpamTestResult> > m_setSpamTestResults;

      bool m_bReAuthenticateUser;
      bool m_bPendingDisconnect;
      bool _isAuthenticated;
      SpamProtectionType m_spType;

      RecipientParser _recipientParser;
   };
}
