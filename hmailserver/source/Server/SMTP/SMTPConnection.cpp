// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com


#include "stdafx.h"
#include "../common/bo/Messages.h"
#include "../common/bo/MessageData.h"

#include "../common/Cache/CacheContainer.h"
#include "../common/Util/PasswordValidator.h"
#include "../common/Util/AccountLogon.h"
#include "../common/persistence/PersistentMessage.h"
#include "../common/persistence/PersistentAccount.h"
#include "../common/BO/Message.h"
#include "../common/Mime/MimeCode.h"
#include "../common/Mime/Mime.h"
#include "../common/util/MessageUtilities.h"
#include "../common/util/Utilities.h"
#include "../common/util/File.h"
#include "../common/util/Time.h"
#include "../common/Application/ClientINfo.h"
#include "../common/AntiSpam/SpamTestResult.h"
#include "../Common/UTil/Math.h"
#include "../Common/UTil/SignatureAdder.h"
#include "../common/BO/Routes.h"
#include "../common/BO/RouteAddresses.h"
#include "../common/BO/SecurityRange.h"
#include "../common/BO/MessageRecipient.h"
#include "../common/BO/MessageRecipients.h"
#include "../Common/Util/ByteBuffer.h"
#include "../Common/Util/ServerStatus.h"
#include "../Common/Util/AWstats.h"
#include "../Common/Util/TransparentTransmissionBuffer.h"
#include "../Common/Application/ObjectCache.h"
#include "../Common/Application/DefaultDomain.h"
#include "../Common/Application/SessionManager.h"
#include "../Common/Cache/MessageCache.h"
#include "../Common/BO/DomainAliases.h"
#include "../Common/BO/Account.h"
#include "../Common/BO/Domains.h"
#include "../Common/BO/Domain.h"

#include "../Common/BO/Collection.h"
#include "../common/persistence/PersistentDomain.h"

#include "../common/Threading/AsynchronousTask.h"
#include "../common/Threading/WorkQueue.h"

#include "SMTPConnection.h"
#include "SMTPConfiguration.h"
#include "SMTPDeliveryManager.h"

#include "../Common/AntiSpam/AntiSpamConfiguration.h"
#include "../Common/AntiSpam/SpamProtection.h"

#include "../Common/Application/TimeoutCalculator.h"
#include "../Common/Application/ScriptingHost/ScriptServer.h"
#include "../Common/Application/ScriptingHost/ScriptObjectContainer.h"
#include "../Common/Application/ScriptingHost/Result.h"

#include "../Common/Application/IniFileSettings.h"

using namespace std;

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   SMTPConnection::SMTPConnection() :  
      m_bRejectedByDelayedGreyListing(false),
      m_CurrentState(AUTHENTICATION),
      m_bTraceHeadersWritten(true),
      _requestedAuthenticationType(AUTH_NONE),
      m_iMaxMessageSizeKB(0),
      m_iCurNoOfRCPTTO(0),
      m_iCurNoOfInvalidCommands(0),
      m_bReAuthenticateUser(false),
      m_spType(SPNone),
      m_bPendingDisconnect(false),
      _isAuthenticated(false)
   {
      m_SMTPConf = Configuration::Instance()->GetSMTPConfiguration();

      /* RFC 2821:    
         An SMTP server SHOULD have a timeout of at least 5 minutes while it
         is awaiting the next command from the sender. 

         Since the DATA command has a timeout on 10 minutes, we can just
         as well set the entire timeout to 10.

         Under very high load, the timeout will decrease below the values specified
         by the RFC. The reasoning is that it's better to disconnect slow clients
         than it is to disconnect everyone.
      */

      TimeoutCalculator calculator;
      SetTimeout(calculator.Calculate(IniFileSettings::Instance()->GetSMTPDMinTimeout(), IniFileSettings::Instance()->GetSMTPDMaxTimeout()));
//      SetTimeout(calculator.Calculate(10, 30 * 60));
   }

   SMTPConnection::~SMTPConnection()
   {
      _ResetCurrentMessage();

      SessionManager::Instance()->OnDisconnect(STSMTP);
   }

   void
   SMTPConnection::OnConnected()
   {
      String sWelcome = Configuration::Instance()->GetSMTPConfiguration()->GetWelcomeMessage();

      String sData = "220 ";

      if (sWelcome.IsEmpty())
         sData += Utilities::ComputerName() + " ESMTP";
      else
         sData += sWelcome;

      _SendData(sData);

      PostReceive();
   }

   AnsiString 
   SMTPConnection::GetCommandSeparator() const
   {
      return "\r\n";
   }

   eSMTPCommandTypes
   SMTPConnection::_GetCommandType(const String &sFirstWord)
   {
      if (sFirstWord == _T("HELO"))
         return SMTP_COMMAND_HELO;
      else if (sFirstWord == _T("HELP"))
         return SMTP_COMMAND_HELP;
      else if (sFirstWord == _T("QUIT"))
         return SMTP_COMMAND_QUIT;
      else if (sFirstWord == _T("EHLO"))
         return SMTP_COMMAND_EHLO;
      else if (sFirstWord == _T("AUTH"))
         return SMTP_COMMAND_AUTH;
      else if (sFirstWord == _T("MAIL"))
         return SMTP_COMMAND_MAIL;
      else if (sFirstWord == _T("RCPT"))
         return SMTP_COMMAND_RCPT;
      else if (sFirstWord == _T("TURN"))
         return SMTP_COMMAND_TURN;
      else if (sFirstWord == _T("VRFY"))
         return SMTP_COMMAND_VRFY;
      else if (sFirstWord == _T("DATA"))
         return SMTP_COMMAND_DATA;
      else if (sFirstWord == _T("RSET"))
         return SMTP_COMMAND_RSET;
      else if (sFirstWord == _T("NOOP"))
         return SMTP_COMMAND_NOOP;
      else if (sFirstWord == _T("ETRN"))
         return SMTP_COMMAND_ETRN;

      return SMTP_COMMAND_UNKNOWN;
   }

   void 
   SMTPConnection::_LogClientCommand(const String &sClientData) const
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Logs one client command.
   //---------------------------------------------------------------------------()
   {
      if (Logger::Instance()->GetLogSMTP())
      {

         String sLogData = sClientData;

         if (m_CurrentState == SMTPUSERNAME && _requestedAuthenticationType == AUTH_PLAIN)
         {
            // Both user name and password in line. 
            sLogData = "***";

            String sAuthentication;
            StringParser::Base64Decode(sClientData, sAuthentication);

            // Extract the username and password from the decoded string.
            int iSecondTab = sAuthentication.Find(_T("\t"),1);
            if (iSecondTab > 0)
            {
               String username = sAuthentication.Mid(0, iSecondTab);


               sLogData = username + " ***";
            }
         }
         else if (m_CurrentState == SMTPUPASSWORD)
         {
            sLogData = "***";
         }         
         
         // Append
         sLogData = "RECEIVED: " + sLogData;
         sLogData.Replace(_T("\r\n"), _T("[nl]"));

         LOG_SMTP(GetSessionID(), GetIPAddressString(), sLogData);      
      }
   }

   void
   SMTPConnection::ParseData(const AnsiString &sRequest)
   {
      InternalParseData(sRequest);

      if (m_bPendingDisconnect == false)
      {
         if (m_CurrentState == DATA)
            PostBufferReceive();
         else
            PostReceive();
      }
   }

   void
   SMTPConnection::InternalParseData(const AnsiString &sRequest)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Parses a clients SMTP command in ASCII mode.
   //---------------------------------------------------------------------------()
   {
      _LogClientCommand(sRequest);

      if (sRequest.GetLength() > 510)
      {
         // This line is to long... is this an evil user?
         _SendData("500 Line to long.");
         return;
      }

      int lFirstSpace = sRequest.Find(" ");
      String sFirstWord;
      if (lFirstSpace > -1)
         sFirstWord = sRequest.Mid(0,lFirstSpace);
      else
         sFirstWord = sRequest;

      sFirstWord.MakeUpper();

      if (Configuration::Instance()->GetDisconnectInvalidClients() &&
          m_iCurNoOfInvalidCommands > Configuration::Instance()->GetMaximumIncorrectCommands())
      {
         // Disconnect
         _SendData("Too many invalid commands. Bye!");
         m_bPendingDisconnect = true;
         PostDisconnect();
         return;
      }

      eSMTPCommandTypes eCommandType = _GetCommandType(sFirstWord);

      // The following commands are independent of state.
      switch (eCommandType)
      {
      case SMTP_COMMAND_HELP:
         {
            _ProtocolHELP();
            return;
         }
      case SMTP_COMMAND_EHLO:
         {
            _ProtocolEHLO(sRequest);
            return;
         }
      case SMTP_COMMAND_HELO:
         {
            _ProtocolHELO(sRequest);
            return;
         }
      case SMTP_COMMAND_QUIT:
         {
            _ProtocolQUIT();
            return;
         }
      case SMTP_COMMAND_NOOP:
         {
            _ProtocolNOOP();
            return;
         }
      case SMTP_COMMAND_RSET:
         {
            _ProtocolRSET();
            return;
         }
      }

      if (m_CurrentState == AUTHENTICATION)
      {      
         _requestedAuthenticationType = AUTH_NONE;
         _SendErrorResponse(502, "Use HELO/EHLO first."); 
         return;
      }
      else if (m_CurrentState == SMTPUSERNAME)
      {
         if (_requestedAuthenticationType == AUTH_LOGIN)
         {
            StringParser::Base64Decode(sRequest, m_sUsername);
            String sEncoded;
            StringParser::Base64Encode("Password:", sEncoded);
            _SendData("334 " + sEncoded);
            m_CurrentState = SMTPUPASSWORD;
         }
         else
         {
            _AuthenticateUsingPLAIN(sRequest);
         }

         return;

      }
      else if (m_CurrentState == SMTPUPASSWORD)
      {
         if (_requestedAuthenticationType == AUTH_LOGIN)
            StringParser::Base64Decode(sRequest, m_sPassword);
         else if (_requestedAuthenticationType == AUTH_PLAIN)
            m_sPassword = sRequest;

         _Authenticate();

         return;

      }
      else if (m_CurrentState == HEADER)
      {
         if (eCommandType == SMTP_COMMAND_AUTH)
         {
            _ProtocolAUTH(sRequest);
            return;
         }
         else if (eCommandType == SMTP_COMMAND_MAIL)
         {
            _ProtocolMAIL(sRequest);
            return;
         }
         else if (eCommandType == SMTP_COMMAND_RCPT)
         {
            _ProtocolRCPT(sRequest);
            return;
         }
         else if (eCommandType == SMTP_COMMAND_TURN)
         {
            _SendData("502 TURN disallowed.");
         
            return;
         }
         else if (eCommandType == SMTP_COMMAND_ETRN)
         {
            _ProtocolETRN(sRequest);
         
            return;
         }
         else if (eCommandType == SMTP_COMMAND_VRFY)
         {
            _SendData("502 VRFY disallowed.");
         
            return;
         }
         else if (eCommandType == SMTP_COMMAND_DATA)
         {
   
            if (!m_pCurrentMessage)
            {
               // User tried to send a mail without specifying a correct mail from or rcpt to.
               _SendData("503 Must have sender and recipient first.");
            
               return;
            }  
            else if ( m_pCurrentMessage->GetRecipients()->GetCount() == 0)
            {
               // User tried to send a mail without specifying a correct mail from or rcpt to.
               _SendData("503 Must have sender and recipient first.");
            
               return;
            }  

            // Let's add an event call on DATA so we can act on reception during SMTP conversation..
            if (Configuration::Instance()->GetUseScriptServer())
            {
               shared_ptr<ScriptObjectContainer> pContainer = shared_ptr<ScriptObjectContainer>(new ScriptObjectContainer);
               shared_ptr<Result> pResult = shared_ptr<Result>(new Result);
               shared_ptr<ClientInfo> pClientInfo = shared_ptr<ClientInfo>(new ClientInfo);

               pClientInfo->SetUsername(m_sUsername);
               pClientInfo->SetIPAddress(GetIPAddressString());
               pClientInfo->SetPort(GetLocalPort());
               pClientInfo->SetHELO(m_sHeloHost);

               pContainer->AddObject("HMAILSERVER_MESSAGE", m_pCurrentMessage, ScriptObject::OTMessage);
               pContainer->AddObject("HMAILSERVER_CLIENT", pClientInfo, ScriptObject::OTClient);
               pContainer->AddObject("Result", pResult, ScriptObject::OTResult);

               String sEventCaller = "OnSMTPData(HMAILSERVER_CLIENT, HMAILSERVER_MESSAGE)";
               ScriptServer::Instance()->FireEvent(ScriptServer::EventOnSMTPData, sEventCaller, pContainer);

               switch (pResult->GetValue())
               {
               case 1:
                  {
                     String sErrorMessage = "554 Rejected";
                     _SendData(sErrorMessage);
                     _LogAwstatsMessageRejected();
                     return;
                  }
               case 2:
                  {
                     String sErrorMessage = "554 " + pResult->GetMessage();
                     _SendData(sErrorMessage);
                     _LogAwstatsMessageRejected();
                     return;
                  }
               case 3:
                  {
                     String sErrorMessage = "453 " + pResult->GetMessage();
                     _SendData(sErrorMessage);
                     _LogAwstatsMessageRejected();
                     return;
                  }
               }
            }      

            m_CurrentState = DATA;

            m_pTransmissionBuffer = shared_ptr<TransparentTransmissionBuffer>(new TransparentTransmissionBuffer(false));
            m_pTransmissionBuffer->Initialize(PersistentMessage::GetFileName(m_pCurrentMessage));
            m_pTransmissionBuffer->SetMaxSizeKB(m_iMaxMessageSizeKB);

            SetReceiveBinary(true);
            m_bTraceHeadersWritten = true;
            m_lMessageStartTC = GetTickCount();

            _SendData("354 OK, send.");

            return;
         }
      }

      _SendErrorResponse(502, "Unimplemented command.");

      return;
   }

   void 
   SMTPConnection::_InitializeSpamProtectionType(const String &sFromAddress)
   {
      // Check if spam protection is enabled for this IP address.
      if (!GetSecurityRange()->GetSpamProtection() ||
           SpamProtection::IsWhiteListed(sFromAddress, GetIPAddress()))
      {
         m_spType = SPNone;
         return;
      }

      shared_ptr<IncomingRelays> incomingRelays = Configuration::Instance()->GetSMTPConfiguration()->GetIncomingRelays();
      // Check if we should do it before or after data transfer
      if (incomingRelays->IsIncomingRelay(GetIPAddress()))
         m_spType = SPPostTransmission;
      else 
         m_spType = SPPreTransmission;
   }

   void 
   SMTPConnection::_ProtocolNOOP()
   {
      _SendData("250 OK");
   }

   void
   SMTPConnection::_ProtocolRSET()
   {
      _ResetCurrentMessage();

      _SendData("250 OK");

      return;
   }

   bool
   SMTPConnection::_ProtocolMAIL(const String &Request)
   {
      if (m_pCurrentMessage) 
      {
         _SendData("503 Issue a reset if you want to start over"); 
         return 0;
      }
     
      if (Request.GetLength() < 10)
      {
         _SendErrorResponse(550, "Invalid syntax. Syntax should be MAIL FROM:<userdomain>[crlf]");
         return 0;
      }

      if (!Request.StartsWith(_T("MAIL FROM:")))
      {
         _SendErrorResponse(550, "Invalid syntax. Syntax should be MAIL FROM:<userdomain>[crlf]");
         return 0;
      }

      // Parse the contents of the MAIL FROM: command
      String sMailFromParameters = Request.Mid(10).Trim();
      String sFromAddress;

      std::vector<String> vecParams = StringParser::SplitString(sMailFromParameters, " ");
      std::vector<String>::iterator iterParam = vecParams.begin();

      if (iterParam != vecParams.end())
      {
         if (!_TryExtractAddress((*iterParam), sFromAddress))
         {
            _SendErrorResponse(550, "Invalid syntax. Syntax should be MAIL FROM:<userdomain>[crlf]");
            return 0;
         }         

         iterParam++;
      }

      sFromAddress = DefaultDomain::ApplyDefaultDomain(sFromAddress);

      if (!CheckIfValidSenderAddress(sFromAddress))
         return false;

      // Parse the extensions 
      String sAuthParam;
      int iEstimatedMessageSize = 0;
      while (iterParam != vecParams.end())
      {
         String sParam = (*iterParam);
         if (sParam.Left(4).CompareNoCase(_T("SIZE")) == 0)
            iEstimatedMessageSize = _ttoi(sParam.Mid(5));
         if (sParam.Left(5).CompareNoCase(_T("AUTH")) == 0)
            sAuthParam = sParam.Mid(5);

         iterParam++;
      }

      // Initialize spam protection now when we know the sender address.
      _InitializeSpamProtectionType(sFromAddress);

      // Apply domain name aliases to this domain name.
      shared_ptr<DomainAliases> pDA = ObjectCache::Instance()->GetDomainAliases();
      const String sAccountAddress = pDA->ApplyAliasesOnAddress(sFromAddress);

      // Pre-transmission spam protection.
      if (m_spType == SPPreTransmission)
      {
         if (IniFileSettings::Instance()->GetDNSBLChecksAfterMailFrom())
         {
            // The message is not arriving from a white listed host or a host
            // which is configured to be a forwarding relay. This means that
            // we can start spam protection now.

            if (!_DoSpamProtection(SPPreTransmission, sFromAddress, m_sHeloHost, GetIPAddress()))
               return false;
         }
      }

      try
      {
         m_pSenderDomain = CacheContainer::Instance()->GetDomain(StringParser::ExtractDomain(sAccountAddress));
         m_pSenderAccount = CacheContainer::Instance()->GetAccount(sAccountAddress);
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 10001, "SMTPConnection::_ProtocolMAIL", "Exception 1");
         throw;
      }
         

      try
      {
         // Check the max size
         m_iMaxMessageSizeKB = _GetMaxMessageSize(m_pSenderDomain);

         // Check if estimated message size exceedes our
         // maximum message size (according to RFC1653)
         if (m_iMaxMessageSizeKB > 0 && 
             iEstimatedMessageSize / 1024 > m_iMaxMessageSizeKB)
         {
            // Message to big. Reject it.
            String sMessage;
            sMessage.Format(_T("552 Message size exceeds fixed maximum message size. Size: %d KB, Max size: %d KB"), 
                  iEstimatedMessageSize / 1024, m_iMaxMessageSizeKB);
            _SendData(sMessage);
            return false;
         }
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 10002, "SMTPConnection::_ProtocolMAIL", "Exception 2");
         throw;
      }
      
      try
      {
         if (m_bReAuthenticateUser && !ReAuthenticateUser())
            return false;
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 10003, "SMTPConnection::_ProtocolMAIL", "Exception 3");
         throw;
      }
            

      try
      {
         // Next time we do a mail from, we should re-authenticate the login credentials
         m_bReAuthenticateUser = true;

         m_pCurrentMessage = shared_ptr<Message> (new Message);
         m_pCurrentMessage->SetFromAddress(sFromAddress);
         m_pCurrentMessage->SetState(Message::Delivering);
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 10004, "SMTPConnection::_ProtocolMAIL", "Exception 4");
         throw;
      }
      
      try
      {
         _SendData("250 OK"); 
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 10005, "SMTPConnection::_ProtocolMAIL", "Exception 5");
         throw;
      }      
   
      return 0;
   }

   bool 
   SMTPConnection::ReAuthenticateUser()
   {
      if (!_isAuthenticated)
      {
         // Nothing to re-authenticate
         return true;
      }
         
      shared_ptr<const Account> pAccount = PasswordValidator::ValidatePassword(m_sUsername, m_sPassword);
      
      if (pAccount)
         return true;
         
      // Reset login credentials
      _ResetLoginCredentials();      

      _SendErrorResponse(550, "Login credentials no longer valid. Please re-authenticate.");                      
      
      return false;
   }

   bool 
   SMTPConnection::CheckIfValidSenderAddress(const String &sFromAddress)
   {
      if (sFromAddress.IsEmpty())
      {
         // The user is trying to send an e-mail without
         // specifying an email address. Should we allow this?
         if (!m_SMTPConf->GetAllowMailFromNull())
         {
            // Nope, we should'nt... We send the below text even
            // though RFC 822 tells us not to...
            _SendErrorResponse(550, "Sender address must be specified.");             
            return false;
         }
      }
      else
      {
         if (!StringParser::IsValidEmailAddress(sFromAddress))
         {
            // The address is not valid...
            _SendErrorResponse(550, "The address is not valid.");
            return false;
         }
      }

      return true;
   }

   void
   SMTPConnection::_ProtocolRCPT(const String &Request)
   {
      m_iCurNoOfRCPTTO ++;

      if (!m_pCurrentMessage) 
      {
         _SendData("503 must have sender first."); 
         return;
      }

      if (!Request.StartsWith(_T("RCPT TO:")))
      {
         _SendErrorResponse(550, "Invalid syntax. Syntax should be RCPT TO:<userdomain>[crlf]");
         return;
      }

      String sRecipientAddress;

      if (!_TryExtractAddress(Request.Mid(8), sRecipientAddress))
      {
         _SendErrorResponse(550, "Invalid syntax. Syntax should be RCPT TO:<userdomain>[crlf]");
         return;
      }

      sRecipientAddress = DefaultDomain::ApplyDefaultDomain(sRecipientAddress);

      if (!StringParser::IsValidEmailAddress(sRecipientAddress))
      {
         // The address is not valid...
         _SendErrorResponse(550, "A valid address is required.");
         return;
      }

      if (m_pCurrentMessage->GetRecipients()->GetCount() >= MaxNumberOfRecipients)
      {
         // The user has added too many recipients for this message. Let's not try
         // to deliver it.
         _SendErrorResponse(550, "Too many recipients.");
         return;
      }


      String sErrMsg = "";
      bool localDelivery = false;
      
      RecipientParser::DeliveryPossibility dp = _recipientParser.CheckDeliveryPossibility(_isAuthenticated, m_pCurrentMessage->GetFromAddress(), sRecipientAddress, sErrMsg, localDelivery, 0);

      if (dp != RecipientParser::DP_Possible)
      {
         AWStats::LogDeliveryFailure(GetIPAddressString(), m_pCurrentMessage->GetFromAddress(), sRecipientAddress, 550);
         _SendData(sErrMsg);

         return;
      }

      bool localSender = _GetIsLocalSender();

      bool authenticationRequired = true;
      if (localSender && localDelivery)
         authenticationRequired = GetSecurityRange()->GetRequireSMTPAuthLocalToLocal();
      else if (localSender && !localDelivery)
         authenticationRequired = GetSecurityRange()->GetRequireSMTPAuthLocalToExternal();
      else if (!localSender && localDelivery)
         authenticationRequired = GetSecurityRange()->GetRequireSMTPAuthExternalToLocal();
      else if (!localSender && !localDelivery)
         authenticationRequired = GetSecurityRange()->GetRequireSMTPAuthExternalToExternal();

      // If the user is local but not authenticated, maybe we should do SMTP authentication.
      if (authenticationRequired && !_isAuthenticated)
      {
         // Authentication is required, but the user hasn't authenticated.
         _SendErrorResponse(530, "SMTP authentication is required.");
         AWStats::LogDeliveryFailure(GetIPAddressString(), m_pCurrentMessage->GetFromAddress(), sRecipientAddress, 530);
         return;
      }

      int iRelayOption = 0;
      if (localSender && localDelivery)
         iRelayOption = SecurityRange::IPRANGE_RELAY_LOCAL_TO_LOCAL;
      else if (localSender && !localDelivery)
         iRelayOption = SecurityRange::IPRANGE_RELAY_LOCAL_TO_REMOTE;
      else if (!localSender && localDelivery)
         iRelayOption = SecurityRange::IPRANGE_RELAY_REMOTE_TO_LOCAL;
      else if (!localSender && !localDelivery)
         iRelayOption = SecurityRange::IPRANGE_RELAY_REMOTE_TO_REMOTE;

      bool bAllowRelay = GetSecurityRange()->GetAllowOption(iRelayOption);
         
      if (bAllowRelay == false)
      {
         // User is not allowed to send this email.
         _SendErrorResponse(550, "Delivery is not allowed to this address.");
         AWStats::LogDeliveryFailure(GetIPAddressString(), m_pCurrentMessage->GetFromAddress(), sRecipientAddress, 550);
         return;
      }


      // Pre-transmission spam protection.
      if (m_spType == SPPreTransmission)
      {
         if (!IniFileSettings::Instance()->GetDNSBLChecksAfterMailFrom())
         {
            // The message is not arriving from a white listed host or a host
            // which is configured to be a forwarding relay. This means that
            // we can start spam protection now.

            if (!_DoSpamProtection(SPPreTransmission, m_pCurrentMessage->GetFromAddress(), m_sHeloHost, GetIPAddress()))
            {
               AWStats::LogDeliveryFailure(GetIPAddressString(), m_pCurrentMessage->GetFromAddress(), sRecipientAddress, 550);
               return;
            }
         }
      }

      if (_GetDoSpamProtection())
      {
         shared_ptr<DomainAliases> pDA = ObjectCache::Instance()->GetDomainAliases();
         const String sToAddress = pDA->ApplyAliasesOnAddress(sRecipientAddress);

         if (!SpamProtection::Instance()->PerformGreyListing(m_pCurrentMessage, m_setSpamTestResults, sToAddress, GetIPAddress()))
         {
            if (m_pCurrentMessage->GetFromAddress().IsEmpty())
            {
               // We got a message with an empty sender address.
               // When this happens, we should delay the greylist-reject 
               // until after the DATA command. The reason for this is
               // that this may be a SMTP callback from another server
               // that is veriying that the recipient exists, using the
               // RCPT TO command. And we don't want to delay that.
               m_bRejectedByDelayedGreyListing = true;
            }
            else
            {
               // The sender is greylisted. We don't log to awstats here,
               // since we tell the client to try again later.
               _SendErrorResponse(451, "Please try again later.");
               return;
            }
         }
      }

      // OK, the recipient is acceptable.
      shared_ptr<MessageRecipients> pRecipients = m_pCurrentMessage->GetRecipients();
      bool recipientOK = false;
      _recipientParser.CreateMessageRecipientList(sRecipientAddress, pRecipients, recipientOK);

      if (!recipientOK)
      {
         _SendData("550 Unknown user");
         return;
      }
   
      _SendData("250 OK");
   }

   bool
   SMTPConnection::_DoSpamProtection(SpamProtectionType spType, const String &sFromAddress, const String &hostName, const IPAddress & lIPAddress)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Does IP based spam protection. Returns true if we should
   // continue delivery, false otherwise.   
   //---------------------------------------------------------------------------()
   {
      if (!_GetDoSpamProtection())
         return true;

      if (spType == SPPreTransmission)
      {
         set<shared_ptr<SpamTestResult> > setResult = 
            SpamProtection::Instance()->RunPreTransmissionTests(sFromAddress, lIPAddress, GetIPAddress(), hostName);

         m_setSpamTestResults.insert(setResult.begin(), setResult.end());
      }
      else if (spType == SPPostTransmission)
      {
         set<shared_ptr<SpamTestResult> > setResult = 
            SpamProtection::Instance()->RunPostTransmissionTests(sFromAddress, lIPAddress, GetIPAddress(), m_pCurrentMessage);

         m_setSpamTestResults.insert(setResult.begin(), setResult.end());

      }

      int iTotalSpamScore = SpamProtection::CalculateTotalSpamScore(m_setSpamTestResults);

      int deleteThreshold = Configuration::Instance()->GetAntiSpamConfiguration().GetSpamDeleteThreshold();
      int markThreshold = Configuration::Instance()->GetAntiSpamConfiguration().GetSpamMarkThreshold();

      if (deleteThreshold > 0 && iTotalSpamScore >= deleteThreshold)
      {
         ServerStatus::Instance()->OnSpamMessageDetected();

         // Generate a text string to send to the client.
         String messageText = _GetSpamTestResultMessage(m_setSpamTestResults);

         if (spType == SPPreTransmission)
            _SendData("550 " + messageText);
         else
            _SendData("554 " + messageText);

         String sLogMessage;
         sLogMessage.Format(_T("hMailServer SpamProtection rejected RCPT (Sender: %s, IP:%s, Reason: %s)"), sFromAddress, String(GetIPAddressString()), messageText);
         LOG_APPLICATION(sLogMessage);

         return false;
      }
      else if (markThreshold > 0 && iTotalSpamScore >= markThreshold)
      {
         // This message is spam, but we shouldn't delete it. Instead, we will add spam headers to it.
         return true;
      }

      return true;
   }

   String 
   SMTPConnection::_GetSpamTestResultMessage(set<shared_ptr<SpamTestResult> > testResults) const
   {
      boost_foreach(shared_ptr<SpamTestResult> result, testResults)
      {
         if (result->GetResult() == SpamTestResult::Fail)
            return result->GetMessage();
      }

      return "";
   }

   void
   SMTPConnection::_ProtocolQUIT()
   {
      // Reset the message here in case a message transmission has started, 
      // but hasn't ended. This can happen if the client sends DATA and then
      // the actual email message in the same buffer (which would be a RFC-violation).
      _ResetCurrentMessage();

      _SendData("221 goodbye");
      
      m_bPendingDisconnect = true;
      PostDisconnect();
   }

   void 
   SMTPConnection::_AppendMessageHeaders()
   {
      if (m_bTraceHeadersWritten)
      {
         shared_ptr<ByteBuffer> pBuffer = m_pTransmissionBuffer->GetBuffer();
         shared_ptr<MimeHeader> pHeader = Utilities::GetMimeHeader(pBuffer->GetBuffer(), pBuffer->GetSize());

         String sOutput;

         // Add received by tag.
         String sReceivedLine;
         String sReceivedIP;
         String sAUTHIP;
         String sAuthSenderReplacementIP = IniFileSettings::Instance()->GetAuthUserReplacementIP();
         bool bAddXAuthUserIP = IniFileSettings::Instance()->GetAddXAuthUserIP();
         

         // If sender is logged in and replace IP is enabled use it
         if (!m_sUsername.IsEmpty() && !sAuthSenderReplacementIP.empty())
         {
            sReceivedIP = sAuthSenderReplacementIP;
            sAUTHIP = GetIPAddressString();
         }
         else
         {
            sReceivedIP = GetIPAddressString();
            sAUTHIP = sReceivedIP;
         }

         sReceivedLine.Format(_T("Received: %s\r\n"), Utilities::GenerateReceivedHeader(sReceivedIP, m_sHeloHost));
         sOutput += sReceivedLine;

         String sComputerName = Utilities::ComputerName(); 

         // Add Message-ID header if it does not exist.
         if (!pHeader->FieldExists("Message-ID"))
         {
            String sTemp;
            sTemp.Format(_T("Message-ID: %s\r\n"), Utilities::GenerateMessageID());
            sOutput += sTemp;
         }

         // Add X-AuthUser header if it does not exist.
         if (IniFileSettings::Instance()->GetAddXAuthUserHeader() && !m_sUsername.IsEmpty())
         {
            if (!pHeader->FieldExists("X-AuthUser"))
               sOutput += "X-AuthUser: " + m_sUsername + "\r\n";
         }

         // Now add x- header for AUTH user if enabled since it was replaced above if so
         // Likely would be good idea for this to be optional at some point
         if (!m_sUsername.IsEmpty() && !sAuthSenderReplacementIP.empty() && bAddXAuthUserIP)
         {
            if (!pHeader->FieldExists("X-AuthUserIP"))
               sOutput += "X-AuthUserIP: " + sAUTHIP + "\r\n";
         }
         
         // We need to prepend these headers to the message buffer.
         shared_ptr<ByteBuffer> pTempBuf = shared_ptr<ByteBuffer>(new ByteBuffer);

         AnsiString sOutputStr = sOutput;
         pTempBuf->Add((BYTE*) sOutputStr.GetBuffer(), sOutputStr.GetLength());
         pTempBuf->Add(m_pTransmissionBuffer->GetBuffer()->GetBuffer(), m_pTransmissionBuffer->GetBuffer()->GetSize());

         // Add to the original buffer
         m_pTransmissionBuffer->GetBuffer()->Empty();
         m_pTransmissionBuffer->GetBuffer()->Add(pTempBuf->GetBuffer(), pTempBuf->GetSize());

         m_bTraceHeadersWritten = false;
      }
   }

   void
   SMTPConnection::ParseData(shared_ptr<ByteBuffer> pBuf)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Parses a clients SMTP command in Binary mode.
   //---------------------------------------------------------------------------()
   {
      // Move the data from the incoming buffer to the transparent transmission buffer.
      // If we've received more data than the max message size, don't save it.

      m_pTransmissionBuffer->Append(pBuf->GetBuffer(), pBuf->GetSize());

      // We need current message size in KB
      int iBufSizeKB = m_pTransmissionBuffer->GetSize() / 1024;

      // Clear the old buffer
      pBuf->Empty();

      // Check if it's time to flush.
      if (m_pTransmissionBuffer->GetRequiresFlush())
      {
         // We need to prepend the transmission buffer
         // with the headers...
         _AppendMessageHeaders();
      }

      // Flush the transmission buffer
      m_pTransmissionBuffer->Flush();

      if (!m_pTransmissionBuffer->GetTransmissionEnded())
      {

         String sLogData;
         int iMaxSizeDrop = IniFileSettings::Instance()->GetSMTPDMaxSizeDrop();
         if (iMaxSizeDrop > 0 && iBufSizeKB >= iMaxSizeDrop) 
         {
            sLogData.Format(_T("Size: %d KB, Max size: %d KB - DROP!!"), 
            iBufSizeKB, iMaxSizeDrop);
            LOG_SMTP(GetSessionID(), GetIPAddressString(), sLogData);      
            String sMessage;
            sMessage.Format(_T("552 Message size exceeds the drop maximum message size. Size: %d KB, Max size: %d KB - DROP!"), 
                iBufSizeKB, iMaxSizeDrop);
            _SendData(sMessage);
         _LogAwstatsMessageRejected();
         _ResetCurrentMessage();
         SetReceiveBinary(false);
         m_bPendingDisconnect = true;
//         PostReceive();
         PostDisconnect();

         return;

      } else {
         // We need more data.
         PostBufferReceive();
         return;
      }
   }

      // Since this may be a time-consuming task, do it asynchronously
      shared_ptr<AsynchronousTask<TCPConnection> > finalizationTask = 
         shared_ptr<AsynchronousTask<TCPConnection> >(new AsynchronousTask<TCPConnection>
            (boost::bind(&SMTPConnection::_HandleSMTPFinalizationTaskCompleted, shared_from_this()), GetTCPConnectionTemporaryPointer()));
      
      Application::Instance()->GetAsyncWorkQueue()->AddTask(finalizationTask);
   }
   
   void
   SMTPConnection::_HandleSMTPFinalizationTaskCompleted()
   {
      if (!_DoPreAcceptSpamProtection())
      {
         // This message was stopped by spam protection. The user either needs
         // to quit or start from rset again.
         _LogAwstatsMessageRejected();

         _ResetCurrentMessage();
         SetReceiveBinary(false);
         PostReceive();
         return;
      }

      _DoPreAcceptMessageModifications();

      // Transmission has ended.
      m_pCurrentMessage->SetSize(FileUtilities::FileSize(PersistentMessage::GetFileName(m_pCurrentMessage)));


   // Let's archive message we just received
   String sArchiveDir = IniFileSettings::Instance()->GetArchiveDir();

   if (!sArchiveDir.empty()) 
   {
      LOG_SMTP(GetSessionID(), GetIPAddressString(), "Archiving..");      

      bool bArchiveHardlinks = IniFileSettings::Instance()->GetArchiveHardlinks();
      String _messageFileName;
      String sFileNameExclPath;
      String sMessageArchivePath;
      String sFromAddress1 = m_pCurrentMessage->GetFromAddress();
      std::vector<String> vecParams1 = StringParser::SplitString(sFromAddress1,  "@");

      // We need exactly 2 or not an email address
      if (vecParams1.size() == 2)
      {
         String sResponse;
         String sSenderName = vecParams1[0];
         sSenderName = sSenderName.ToLower();
         String sSenderDomain = vecParams1[1];
         sSenderDomain = sSenderDomain.ToLower();
         bool blocalSender1 = _GetIsLocalSender();

         if (blocalSender1)
         {
            // First copy goes to local sender
            _messageFileName = PersistentMessage::GetFileName(m_pCurrentMessage);
            sFileNameExclPath = FileUtilities::GetFileNameFromFullPath(_messageFileName);
            sMessageArchivePath = sArchiveDir + "\\" + sSenderDomain + "\\" + sSenderName + "\\Sent-" + sFileNameExclPath;

            LOG_SMTP(GetSessionID(), GetIPAddressString(), "Local sender: " + sFromAddress1 + ". Putting in user folder: " + sMessageArchivePath);      

            FileUtilities::Copy(_messageFileName, sMessageArchivePath, true);
         }
         else
         {
            LOG_SMTP(GetSessionID(), GetIPAddressString(), "Non local sender, putting in common Inbound folder..");      

            // First copy goes to common archive folder instead
            _messageFileName = PersistentMessage::GetFileName(m_pCurrentMessage);
            sFileNameExclPath = FileUtilities::GetFileNameFromFullPath(_messageFileName);
            sMessageArchivePath = sArchiveDir + "\\Inbound\\" + sFileNameExclPath;

            FileUtilities::Copy(_messageFileName, sMessageArchivePath, true);
         }
            String sMessageArchivePath2;

            // Now create hardlink/copy for each *local* recipient
            shared_ptr<const Domain> pDomaintmp;
            bool bDomainIsLocal = false;

            const std::vector<shared_ptr<MessageRecipient> > vecRecipients = m_pCurrentMessage->GetRecipients()->GetVector();
            std::vector<shared_ptr<MessageRecipient> >::const_iterator iterRecipient = vecRecipients.begin();
            while (iterRecipient != vecRecipients.end())
            {
               String sRecipientAddress = (*iterRecipient)->GetAddress();
               vecParams1 = StringParser::SplitString(sRecipientAddress,  "@");

               // We need exactly 2 or not an email address
               if (vecParams1.size() == 2)
               {
                  String sResponse;
                  String sSenderName = vecParams1[0];
                  sSenderName = sSenderName.ToLower();
                  String sSenderDomain = vecParams1[1];
                  sSenderDomain = sSenderDomain.ToLower();

                  pDomaintmp = CacheContainer::Instance()->GetDomain(sSenderDomain);
                  bDomainIsLocal = pDomaintmp ? true : false;

                  if (bDomainIsLocal)
                  {
                     sMessageArchivePath2 = sArchiveDir + "\\" + sSenderDomain + "\\" + sSenderName + "\\" + sFileNameExclPath;
                     LOG_SMTP(GetSessionID(), GetIPAddressString(), "Local recipient: " + sRecipientAddress + ". Putting in user folder: " + sMessageArchivePath2);      

                     if (bArchiveHardlinks) 
                     {
                        FileUtilities::CreateDirectoryRecursive(sArchiveDir + "\\" + sSenderDomain + "\\" + sSenderName);
                        // This function call is odd in that original is 2nd anc destination is 1st..
                        BOOL fCreatedLink = CreateHardLink( sMessageArchivePath2, sMessageArchivePath, NULL ); // Last is reserved, must be NULL

                        if ( fCreatedLink == FALSE )
                        {
                           // If error try normal copy
                           FileUtilities::Copy(sMessageArchivePath, sMessageArchivePath2, true);
                           LOG_SMTP(GetSessionID(), GetIPAddressString(), "HardLink failed.. Falling back to Copy.");      
                        }
                        else
                        {
                           LOG_SMTP(GetSessionID(), GetIPAddressString(), "HardLink succeeded.");      
                        }
                    }
                    else
                    {
                       FileUtilities::Copy(sMessageArchivePath, sMessageArchivePath2, true);
                    }
                  }
               }
            iterRecipient++;
         }
   }
   else
   {
      // Sender is either null/blank (ie <>) or some other odd thing happed so we'll save in Error folder
      // either way as failsafe.
      LOG_SMTP(GetSessionID(), GetIPAddressString(), "Sender is NULL or invalid. Saving to Error folder.");      

      _messageFileName = PersistentMessage::GetFileName(m_pCurrentMessage);
      sFileNameExclPath = FileUtilities::GetFileNameFromFullPath(_messageFileName);
      sMessageArchivePath = sArchiveDir + "\\Error\\" + sFileNameExclPath;
      FileUtilities::Copy(_messageFileName, sMessageArchivePath, true);
   }
}   

      float dTime = ((float) GetTickCount() - (float) m_lMessageStartTC) / (float) 1000;
      double dTCDiff = Math::Round(dTime ,3);

      if (_OnPreAcceptTransfer())
      {
         // Add the message to the database.
         if (PersistentMessage::SaveObject(m_pCurrentMessage))
         {
            // Make sure the transmission buffer has released the handle
            // to the file.
            if (m_pTransmissionBuffer)
               m_pTransmissionBuffer.reset();

            // Add this message to the delivery queue cache. This way,
            // we won't have to read it from the database.
            MessageCache::Instance()->AddMessage(m_pCurrentMessage);

            // Free the message, so we don't access it the same time
            // as the SMTP delivery manager.
            m_pCurrentMessage.reset();

            // Tell the deliverer that a new message is pending. This
            // will cause the SMTP delivery manager to start a new delivery
            // thread and deliver the message.
            Application::Instance()->SubmitPendingEmail();

            // Reset the spam protection results.
            m_setSpamTestResults.clear();

            // Tell the client that everything went fine. This
            // will cause the client to either disconnect or to
            // start a new message.
            String sResponse;
            sResponse.Format(_T("250 Queued (%.3f seconds)"), dTCDiff);
            _SendData(sResponse);

            // The message delivery is complete, or
            // it has failed. Any way, we should start
            // a new message.
            m_CurrentState = HEADER;
         }
         else
         {
            // The delivery of the message failed. This may happen if tables are
            // corrupt in the database. We now return an error message to the sender. 
            // Hopefully, the sending server will retry later. 
            _SendData("554 Your message was received but it could not be saved. Please retry later.");

            // Delete the file now since we could not save it in the database.
            _ResetCurrentMessage();
            
         }
      }
      else
      {
         // The message was rejected by _OnPreAcceptTransfer. For example
         // this may happen if the message was rejected by a script subscribing
         // to OnAcceptMessage.
         _ResetCurrentMessage();
      }

      SetReceiveBinary(false);
      PostReceive();
  
   }

   void
   SMTPConnection::_DoPreAcceptMessageModifications()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Make changes to the message before it's accepted for delivery. This is
   // for example where message signature and spam-headers are added.
   //---------------------------------------------------------------------------()
   {
      shared_ptr<MessageData> pMsgData;

      // Check if we should add a spam header.
      int iTotalSpamScore = SpamProtection::CalculateTotalSpamScore(m_setSpamTestResults);
      int iSpamMarkThreshold = Configuration::Instance()->GetAntiSpamConfiguration().GetSpamMarkThreshold();

      if (iTotalSpamScore >= iSpamMarkThreshold)
      {
         pMsgData = SpamProtection::TagMessageAsSpam(m_pCurrentMessage, m_setSpamTestResults);

         // Increase the spam-counter
         ServerStatus::Instance()->OnSpamMessageDetected();
      }

      _SetMessageSignature(pMsgData);

      if (pMsgData)
         pMsgData->Write(PersistentMessage::GetFileName(m_pCurrentMessage));
   }

   void
   SMTPConnection::_SetMessageSignature(shared_ptr<MessageData> &pMessageData)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Sets the signature of the message, based on the signature in the account
   // settings and domain settings.
   //---------------------------------------------------------------------------()
   {
      shared_ptr<SignatureAdder> pSignatureAdder = shared_ptr<SignatureAdder>(new SignatureAdder);
      pSignatureAdder->SetSignature(m_pCurrentMessage, m_pSenderDomain, m_pSenderAccount, pMessageData);
   }

   bool
   SMTPConnection::_OnPreAcceptTransfer()
   {
      if (m_pTransmissionBuffer->GetCancelTransmission())
      {
         _SendData("554 "  + m_pTransmissionBuffer->GetCancelMessage());
         _LogAwstatsMessageRejected();
         return false;
      }

      const String fileName = PersistentMessage::GetFileName(m_pCurrentMessage);

      if (!FileUtilities::Exists(fileName))
      {
         String sErrorMsg;
         sErrorMsg.Format(_T("Rejected message because no mail data has been saved in file %s"), fileName);

         ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5019, "SMTPConnection::_OnPreAcceptTransfer", sErrorMsg);
      
         _SendData("451 Rejected - No data saved.");
         _LogAwstatsMessageRejected();
         return false;
      }


      // Check so that message isn't to big. Max message
      // size is specified in KB.
      if (m_iMaxMessageSizeKB > 0 && (m_pTransmissionBuffer->GetSize() / 1024) > m_iMaxMessageSizeKB)
      {
         String sMessage;
         sMessage.Format(_T("554 Rejected - Message size exceeds fixed maximum message size. Size: %d KB, Max size: %d KB"), 
            m_pTransmissionBuffer->GetSize() / 1024, m_iMaxMessageSizeKB);
         _SendData(sMessage);
         _LogAwstatsMessageRejected();
         return false;
      }

      // Check for bare LF's.
      if (!Configuration::Instance()->GetSMTPConfiguration()->GetAllowIncorrectLineEndings())
      {
         if (!_CheckLineEndings())
         {
            String sMessage;
            sMessage.Format(_T("554 Rejected - Message containing bare LF's."));
            
            _SendData(sMessage);
            _LogAwstatsMessageRejected();
            return false;
         }

      }

      if (Configuration::Instance()->GetUseScriptServer())
      {
         shared_ptr<ScriptObjectContainer> pContainer = shared_ptr<ScriptObjectContainer>(new ScriptObjectContainer);
         shared_ptr<Result> pResult = shared_ptr<Result>(new Result);
         shared_ptr<ClientInfo> pClientInfo = shared_ptr<ClientInfo>(new ClientInfo);

         pClientInfo->SetUsername(m_sUsername);
         pClientInfo->SetIPAddress(GetIPAddressString());
         pClientInfo->SetPort(GetLocalPort());
         pClientInfo->SetHELO(m_sHeloHost);

         pContainer->AddObject("HMAILSERVER_MESSAGE", m_pCurrentMessage, ScriptObject::OTMessage);
         pContainer->AddObject("HMAILSERVER_CLIENT", pClientInfo, ScriptObject::OTClient);
         pContainer->AddObject("Result", pResult, ScriptObject::OTResult);

         String sEventCaller = "OnAcceptMessage(HMAILSERVER_CLIENT, HMAILSERVER_MESSAGE)";
         ScriptServer::Instance()->FireEvent(ScriptServer::EventOnAcceptMessage, sEventCaller, pContainer);

         switch (pResult->GetValue())
         {
         case 1:
            {
               String sErrorMessage = "554 Rejected";
               _SendData(sErrorMessage);
               _LogAwstatsMessageRejected();
               return false;
            }
         case 2:
            {
               String sErrorMessage = "554 " + pResult->GetMessage();
               _SendData(sErrorMessage);
               _LogAwstatsMessageRejected();
               return false;
            }
         case 3:
            {
               String sErrorMessage = "453 " + pResult->GetMessage();
               _SendData(sErrorMessage);
               _LogAwstatsMessageRejected();
               return false;
            }
         }
      }      

      if (GetSecurityRange()->GetVirusProtection())
      {
         m_pCurrentMessage->SetFlagVirusScan(true);
      }

      return true;
   }

   bool 
   SMTPConnection::_CheckLineEndings() const
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Checks if the message contains any CR with missing LF, or any LF with
   // missing CR.
   //---------------------------------------------------------------------------()
   {
      try
      {
         if (!m_pCurrentMessage)
            return false;

         const String fileName = PersistentMessage::GetFileName(m_pCurrentMessage);

         File oFile;
         if (!oFile.Open(fileName, File::OTReadOnly))
            return false;

         const int iChunkSize = 10000;
         shared_ptr<ByteBuffer> pBuffer = oFile.ReadChunk(iChunkSize);
         while (pBuffer)
         {
            // Check that buffer contains correct line endings.
            const char *pChar = pBuffer->GetCharBuffer();
            int iBufferSize = pBuffer->GetSize();

            // Check from pos 3 to size-3. Not 100% sure, but
            // we don't have to worry about buffer start/endings.

            for (int i = 3; i < iBufferSize - 3; i++)
            {
               const char *pCurrentChar = pChar + i;

               // Check chars.
               if (*pCurrentChar == '\r')
               {
                  // Check next character
                  if (i >= iBufferSize)
                     return false;

                  const char *pNextChar = pCurrentChar + 1;
                  if (*pNextChar != '\n')
                     return false;
               }
               else if (*pCurrentChar == '\n')
               {
                  // Check previous char
                  if (i == 0)
                     return false;

                  const char *pPreviousChar = pCurrentChar - 1;
                  if (*pPreviousChar != '\r')
                     return false;
               }
            }

            // Read next chunk
            pBuffer = oFile.ReadChunk(iChunkSize);
         }
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4328, "SMTPConnection::_CheckLineEndings", "Failed to check line endings.");
      }


      return true;
   }

   void 
   SMTPConnection::_LogAwstatsMessageRejected()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // If awstats logging is enabled, this function goes through all the recipients
   // of the message, and logs to the awstats log that they have been rejected.
   // This is used if a message is rejected after it has been transferred from the
   // client to the server.
   //---------------------------------------------------------------------------()
   {
      // Check that message exists, and that the awstats log is enabled.
      if (!m_pCurrentMessage || !AWStats::GetEnabled())
         return;

      // Go through the recipients and log one row for each of them.
      String sFromAddress = m_pCurrentMessage->GetFromAddress();

      const std::vector<shared_ptr<MessageRecipient> > vecRecipients = m_pCurrentMessage->GetRecipients()->GetVector();
      std::vector<shared_ptr<MessageRecipient> >::const_iterator iterRecipient = vecRecipients.begin();
      while (iterRecipient != vecRecipients.end())
      {
         String sRecipientAddress = (*iterRecipient)->GetAddress();

         // Log the error message
         AWStats::LogDeliveryFailure(GetIPAddressString(), sFromAddress, sRecipientAddress, 554);

         iterRecipient++;
      }

   }

   void
   SMTPConnection::_ResetCurrentMessage()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Reset the transmission buffer to free
   // any handles it has opened to the message
   // file
   //---------------------------------------------------------------------------()
   {
      if (m_pTransmissionBuffer)
      {
         m_pTransmissionBuffer.reset();
      }

      // Reset the current message.
      if (m_pCurrentMessage)
      {
         // This message isn't complete, so we should delete it from disk now.
         shared_ptr<Account> emptyAccount;

         PersistentMessage::DeleteFile(emptyAccount, m_pCurrentMessage);

         // Reset message object
         m_pCurrentMessage.reset();
      }

      m_bRejectedByDelayedGreyListing = false;

      m_pSenderDomain.reset();
      m_pSenderAccount.reset();

      m_setSpamTestResults.clear();

      // Reset the number of RCPT TO's for this 
      // message.
      m_iCurNoOfRCPTTO = 0;

      // Switch back to normal ASCII mode and start of session, in
      // case we are in binary transmission mode.
      m_CurrentState = HEADER;
   }


   bool
   SMTPConnection::_SendEHLOKeywords()
   {
      String sComputerName = Utilities::ComputerName(); 

      String sData = "250-" + sComputerName + "\r\n";
      
      // Append size keyword
      {
         String sSizeKeyword;
         int iMaxSize = m_SMTPConf->GetMaxMessageSize() * 1000;
         if (iMaxSize > 0)
            sSizeKeyword.Format(_T("250-SIZE %d\r\n"), iMaxSize);
         else
            sSizeKeyword.Format(_T("250-SIZE\r\n"));
         sData += sSizeKeyword;
      }

      String sAuth = "250 AUTH LOGIN";

      if (m_SMTPConf->GetAuthAllowPlainText())
         sAuth += " PLAIN";

      sData += sAuth;
	   
      _SendData(sData);
   
      return true;
   }

   void
   SMTPConnection::OnConnectionTimeout()
   {
      _ResetCurrentMessage();

      _SendData("421 Connection timeout.\r\n");
   }
  
   void
   SMTPConnection::OnExcessiveDataReceived()
   {
      _ResetCurrentMessage();
      _SendData("421 Excessive amounts of data sent to server.\r\n");
   }

   void 
   SMTPConnection::_SendData(const String &sData)
   {
      if (Logger::Instance()->GetLogSMTP())
      {
         String sLogData = "SENT: " + sData;

         sLogData.TrimRight(_T("\r\n"));
         sLogData.Replace(_T("\r\n"), _T("[nl]"));

         LOG_SMTP(GetSessionID(), GetIPAddressString(), sLogData);
      }

      SendData(sData + "\r\n");
   }

   bool
   SMTPConnection::_ReadDomainAddressFromHelo(const  String &sRequest)
   {
      int iFirstSpace = sRequest.Find(_T(" "));
      
      if (iFirstSpace == -1)
      {
         // No host name has been specified => RFC violation
         return false;
      }

      // Cut out the string after the space.
      m_sHeloHost = sRequest.Mid(iFirstSpace + 1);

      // Trim it incase of leading or trailing spaces.
      m_sHeloHost = m_sHeloHost.Trim();

      if (m_sHeloHost.IsEmpty())
         return false;

      return true;

   }

   void
   SMTPConnection::_ProtocolEHLO(const String &sRequest)
   {

      if (!_ReadDomainAddressFromHelo(sRequest))
      {
         // The client did not supply a parameter to
         // the helo command which is syntaxically
         // incorrect. Reject.
         _SendErrorResponse(501, "EHLO Invalid domain address.");
         return;
      }

      _SendEHLOKeywords();

      if (m_CurrentState == AUTHENTICATION)
         m_CurrentState = HEADER;
   }
   
   void
   SMTPConnection::_ProtocolHELO(const String &sRequest)
   {
      if (!_ReadDomainAddressFromHelo(sRequest))
      {
         // The client did not supply a parameter to
         // the helo command which is syntaxically
         // incorrect. Reject.
         _SendErrorResponse(501, "HELO Invalid domain address.");
         return;
      }

      _SendData("250 Hello.");

      if (m_CurrentState == AUTHENTICATION)
         m_CurrentState = HEADER;

   }

   void
   SMTPConnection::_ProtocolHELP()
   {
      _SendData("211 DATA HELO EHLO MAIL NOOP QUIT RCPT RSET SAML TURN VRFY\r\n");
   }

   void 
   SMTPConnection::_ProtocolAUTH(const String &sRequest)
   {
      _requestedAuthenticationType = AUTH_NONE;

      std::vector<String> vecParams = StringParser::SplitString(sRequest,  " ");

      if (vecParams.size() == 1)
      {
         _SendErrorResponse(504, "Authentication type not specified.");
         return;
      }
      
      String sAuthenticationType = vecParams[1];
      sAuthenticationType.MakeUpper();

      if (sAuthenticationType == _T("LOGIN"))
      {
         _requestedAuthenticationType = AUTH_LOGIN;

         String sResponse;

         if (vecParams.size() == 3)
         {
            // Fetch username from third parameter.
            StringParser::Base64Decode(vecParams[2], m_sUsername);
            m_CurrentState = SMTPUPASSWORD;

            StringParser::Base64Encode("Password:", sResponse);
         }
         else
         {
            m_CurrentState = SMTPUSERNAME;
            StringParser::Base64Encode("Username:", sResponse);
         }

         _SendData("334 " + sResponse);
         return;

      }
      else if (sAuthenticationType == _T("PLAIN") && 
               m_SMTPConf->GetAuthAllowPlainText())
      {
         _requestedAuthenticationType = AUTH_PLAIN;

         // Stupid user has selected plain text authentication.
         if (vecParams.size() == 3)
         {
            // Fetch username and password directly from command.
            _AuthenticateUsingPLAIN(vecParams[2]);
         }
         else
         {
            _SendData("334 Log on");
            m_CurrentState = SMTPUSERNAME;
         }

         return;
      }

      _SendErrorResponse(504, "Authentication mechanism not supported.");
   }

   void 
   SMTPConnection::_ProtocolETRN(const String &sRequest)
   {
      // RFC ETRN Codes
      //   250 OK, queuing for node <x> started
      //   251 OK, no messages waiting for node <x>
      //   252 OK, pending messages for node <x> started
      //   253 OK, <n> pending messages for node <x> started
      //   458 Unable to queue messages for node <x>
      //   459 Node <x> not allowed: <reason>
      //   500 Syntax Error
      //   501 Syntax Error in Parameters


// WE SHOULD ADD SOME LOGGING HERE

      std::vector<String> vecParams = StringParser::SplitString(sRequest,  " ");

      // We need at least 1 parameter. ETRN alone results in error
      if (vecParams.size() == 1)
      {
         _SendErrorResponse(500, "Syntax Error: No domain parameter included");
         LOG_SMTP(GetSessionID(), GetIPAddressString(), "SMTPDeliverer - ETRN - No domain parameter included");      
         return;
      }
      
      String sResponse;
      String sETRNDomain = vecParams[1];
      String sETRNDomain2 = sETRNDomain.ToLower();
      String sLogData;

      bool bIsRouteDomain = false;
      vector<shared_ptr<Route> > routes = Configuration::Instance()->GetSMTPConfiguration()->GetRoutes()->GetItemsByName(sETRNDomain.ToLower());

      // See if sender supplied param matches one of our domains
      if (routes.size() > 0)
       {
          boost_foreach(shared_ptr<Route> route, routes)
          {
             if (route->GetName() == sETRNDomain2) 
             {
             bIsRouteDomain = true;
             break;
             }
          }
       }       

      if (bIsRouteDomain)
      {
         LOG_SMTP(GetSessionID(), GetIPAddressString(), "SMTPDeliverer - ETRN - Route found, continuing..");      

         shared_ptr<Routes> pRoutes = Configuration::Instance()->GetSMTPConfiguration()->GetRoutes();
         shared_ptr<Route> pRoute = pRoutes->GetItemByName(sETRNDomain.ToLower());

         if (pRoute)
         {
            __int64 iRouteID = pRoute->GetID();

            LOG_SMTP(GetSessionID(), GetIPAddressString(), "SMTPDeliverer - ETRN - Route settings read successfully.");      

            int lTmpNoOfRetries = pRoute->NumberOfTries();
            int lTmpMinutesBetween = pRoute->MinutesBetweenTry();

            // Here we change ID back to 0, type back to 1 & next try to ASAP for Route ID
            // Special 1901-01-01 00:00:01 tells admin it is HOLD
            SQLCommand command("update hm_messages set messageaccountid = 0, messagetype = 1, messagenexttrytime = '1901-01-01 00:00:00' where messagetype = 3 and messageaccountid = @ROUTEID");
            command.AddParameter("@ROUTEID", iRouteID);
            if (Application::Instance()->GetDBManager()->Execute(command))
            {
               // Need to tell hmail to reload the settings
               //Configuration::Instance()->Load();
               _SendData("250 OK, message queuing started for " + sETRNDomain.ToLower());
               LOG_SMTP(GetSessionID(), GetIPAddressString(), "SMTPDeliverer - ETRN - 250 OK, message queuing started.");      
            }
            else
            {
               _SendData("458 Unable to queue messages for " + sETRNDomain.ToLower());
               LOG_SMTP(GetSessionID(), GetIPAddressString(), "SMTPDeliverer - ETRN - 458 Unable to queue messages");      
            }
         return;

       }
       else
       {
          // Send that we don't accept ETRN for that domain or invalid param
          _SendData("458 Error getting info for " + sETRNDomain.ToLower());
          LOG_SMTP(GetSessionID(), GetIPAddressString(), "SMTPDeliverer - ETRN - Could not get Route values");      
          return;
       }
     }
     else
     {
         // Send that we don't accept ETRN for that domain or invalid param
         _SendData("501 ETRN not supported for " + sETRNDomain.ToLower());
         LOG_SMTP(GetSessionID(), GetIPAddressString(), "SMTPDeliverer - ETRN - Domain is not Route");      
         return;
     }
   }
   void
   SMTPConnection::_AuthenticateUsingPLAIN(const String &sLine)
   {
      String sAuthentication;
      StringParser::Base64Decode(sLine, sAuthentication);

      // Extract the username and password from the decoded string.
      int iSecondTab = sAuthentication.Find(_T("\t"),1);
      if (iSecondTab < 0)
      {
         _RestartAuthentication();
         return;
      }

      m_sUsername = sAuthentication.Mid(1, iSecondTab-1);
      m_sPassword = sAuthentication.Mid(iSecondTab+1);

      // Authenticate the user.
      _Authenticate();      
   }

   void
   SMTPConnection::_Authenticate()
   {
      AccountLogon accountLogon;
      bool disconnect;

      shared_ptr<const Account> pAccount = accountLogon.Logon(GetIPAddress(), m_sUsername, m_sPassword, disconnect);
         
      if (disconnect)
      {
         _SendErrorResponse(535, "Authentication failed. Too many invalid logon attempts.");
         m_bPendingDisconnect = true;
         PostDisconnect();
         return;
      }
     
      if (pAccount)
      {
         _SendData("235 authenticated.");

         _isAuthenticated = true;
         m_CurrentState = HEADER;
      }
      else
      {
         _RestartAuthentication();
      }
   }

   void 
   SMTPConnection::_RestartAuthentication()
   {
      _ResetLoginCredentials();
      
      _SendErrorResponse(535, "Authentication failed. Restarting authentication process.");
   }

   void 
   SMTPConnection::_ResetLoginCredentials()
   {
      _requestedAuthenticationType = AUTH_NONE;
      _isAuthenticated = false;

      m_CurrentState = HEADER;
      m_sUsername = "";
      m_bReAuthenticateUser = false;
   }


   int 
   SMTPConnection::_GetMaxMessageSize(shared_ptr<const Domain> pDomain)
   {
      int iMaxMessageSizeKB = m_SMTPConf->GetMaxMessageSize();
      
      if (pDomain)
      {
         int iDomainMaxSizeKB = pDomain->GetMaxMessageSize(); 
         if (iDomainMaxSizeKB > 0)
         {
            if (iMaxMessageSizeKB == 0 || iMaxMessageSizeKB > iDomainMaxSizeKB)
               iMaxMessageSizeKB = iDomainMaxSizeKB;
         }
      }

      return iMaxMessageSizeKB;
   }

   void 
   SMTPConnection::_SendErrorResponse(int iErrorCode, const String &sResponse)
   {
      String sData;
      sData.Format(_T("%d %s"), iErrorCode, sResponse);
      
      _SendData(sData);

      if (iErrorCode >= 500 && iErrorCode <= 599)
         m_iCurNoOfInvalidCommands++;   
   }

   bool
   SMTPConnection::_DoPreAcceptSpamProtection()
   {
      if (m_bRejectedByDelayedGreyListing)
      {
         _SendErrorResponse(450, "Please try again later.");
         // Don't log to awstats here, since we tell the client to try again later.
         return false;
      }

      // Check if we should do pre-transmissions tests after transmission. This
      // happens if the message is delivered from a forwarding relay server.
      if (m_spType == SPPostTransmission)
      {
         // Do all spam proteciton now. It has been delayed since we trust the 
         // server which has forwarded to us.
         // Retrieve the IP address from the message headers.
         IPAddress iIPAddress;
         String hostName;
         
         MessageUtilities::RetrieveOriginatingAddress(m_pCurrentMessage, hostName, iIPAddress);
      
         // Do spam protection now using the IP address in the header.
         if (!_DoSpamProtection(SPPreTransmission, m_pCurrentMessage->GetFromAddress(), hostName, iIPAddress))
         {
            // We should stop the message delivery.
            return false;
         }

         if (!_DoSpamProtection(SPPostTransmission, m_pCurrentMessage->GetFromAddress(), hostName, iIPAddress))
         {
            // We should stop the message delivery.
            return false;
         }
         
      }
      else
      {
         // Do normal post transmission spam protection. (typically SURBL)
         if (!_DoSpamProtection(SPPostTransmission, m_pCurrentMessage->GetFromAddress(), m_sHeloHost, GetIPAddress()))
         {
            // We should stop message delivery
            return false;
         }
      }

      // The message should be delivered.
      return true;
   }


   bool 
   SMTPConnection::_GetDoSpamProtection() const
   {
      if (_isAuthenticated)
         return false;

      if (!GetSecurityRange()->GetSpamProtection())
         return false;

      if (m_pCurrentMessage)
      {
         if (SpamProtection::IsWhiteListed(m_pCurrentMessage->GetFromAddress(), GetIPAddress()))
            return false;
      }

      return true;
   }

   /*
      Returns true if 
      - the domain-part of the email matches an active local domain.
      - the sender address matches a route address.
   */
   bool
   SMTPConnection::_GetIsLocalSender()
   {
       if (m_pSenderDomain && m_pSenderDomain->GetIsActive())
          return true;

       const String senderAddress = m_pCurrentMessage->GetFromAddress();

       String senderDomainName = StringParser::ExtractDomain(senderAddress);
       vector<shared_ptr<Route> > routes = Configuration::Instance()->GetSMTPConfiguration()->GetRoutes()->GetItemsByName(senderDomainName);

       if (routes.size() > 0)
       {
          boost_foreach(shared_ptr<Route> route, routes)
          {
             if (route->ToAllAddresses() || route->GetAddresses()->GetItemByName(senderAddress))
             {
                if (route->GetTreatSenderAsLocalDomain())
                   return true;
             }
          }
       }       

       // Does not match a local domain or route.
       return false;
   }

   bool 
   SMTPConnection::_TryExtractAddress(const String &mailFromParameter, String& address)
   {
      // Start of by removing any leading and trailing space.
      address = mailFromParameter;
      address = address.TrimLeft();
      address = address.TrimRight();

      // Empty address is OK
      if (address.GetLength() == 0)
         return true;

      // If the user is supplying a <, the address must end with a >
      if (address.StartsWith(_T("<")))
      {
         if (!address.EndsWith(_T(">")))
            return false;

         address.TrimLeft('<');
         address.TrimRight('>');

         address.TrimLeft();
         address.TrimRight();
      }
      else if (address.EndsWith(_T(">")))
      {
         // The address starts with something other than < but ands with >: Syntax error
         return false;
      }

      return true;
   }
}
