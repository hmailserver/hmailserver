// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com


#include "stdafx.h"

#include <Boost/Regex.hpp>

#include "../common/bo/MessageData.h"

#include "../common/Cache/CacheContainer.h"
#include "../common/Util/PasswordValidator.h"
#include "../common/Util/AccountLogon.h"
#include "../common/persistence/PersistentMessage.h"
#include "../common/BO/Message.h"
#include "../common/BO/SecurityRange.h"
#include "../common/Mime/Mime.h"
#include "../common/util/MessageUtilities.h"
#include "../common/util/Utilities.h"
#include "../common/util/File.h"
#include "../common/Scripting/ClientInfo.h"
#include "../common/AntiSpam/SpamTestResult.h"
#include "../Common/UTil/Math.h"
#include "../Common/UTil/SignatureAdder.h"
#include "../common/BO/Routes.h"
#include "../common/BO/RouteAddresses.h"
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
#include "../Common/BO/Domain.h"

#include "../Common/BO/Collection.h"

#include "../common/Threading/AsynchronousTask.h"
#include "../common/Threading/WorkQueue.h"

#include "../Common/AntiSpam/AntiSpamConfiguration.h"
#include "../Common/AntiSpam/SpamProtection.h"

#include "../Common/Application/TimeoutCalculator.h"
#include "../Common/Scripting/ScriptServer.h"
#include "../Common/Scripting/ScriptObjectContainer.h"
#include "../Common/Scripting/Result.h"

#include "../Common/Application/IniFileSettings.h"

#include "../Common/Util/CrashSimulation.h"

#include "SMTPConnection.h"
#include "SMTPConfiguration.h"
#include "SMTPMessageHeaderCreator.h"

#include "../Common/TCPIP/CipherInfo.h"

using namespace std;

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   SMTPConnection::SMTPConnection(ConnectionSecurity connection_security,
      boost::asio::io_service& io_service, 
      boost::asio::ssl::context& context) :  
      TCPConnection(connection_security, io_service, context, std::shared_ptr<Event>(), ""),
      rejected_by_delayed_grey_listing_(false),
      current_state_(INITIAL),
      trace_headers_written_(true),
      requestedAuthenticationType_(AUTH_NONE),
      max_message_size_kb_(0),
      cur_no_of_rcptto_(0),
      cur_no_of_invalid_commands_(0),
      re_authenticate_user_(false),
      type_(SPNone),
      pending_disconnect_(false),
      isAuthenticated_(false),
      start_tls_used_(false)
   {

      smtpconf_ = Configuration::Instance()->GetSMTPConfiguration();

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
   }

   const String CONST_UNKNOWN_USER = "Unknown user";

   SMTPConnection::~SMTPConnection()
   {
      try
      {
         ResetCurrentMessage_();

         if (GetConnectionState() != StatePendingConnect)
            SessionManager::Instance()->OnSessionEnded(STSMTP);
      }
      catch (...)
      {

      }
   }


   void
   SMTPConnection::OnConnected()
   {
      if (GetConnectionSecurity() == CSNone ||
          GetConnectionSecurity() == CSSTARTTLSOptional ||
          GetConnectionSecurity() == CSSTARTTLSRequired)
      {
         SendBanner_();
      }

   }

   void
   SMTPConnection::OnHandshakeCompleted()
   {
      if (GetConnectionSecurity() == CSSSL)
      {
         SendBanner_();
      }
      else if (GetConnectionSecurity() == CSSTARTTLSOptional ||
               GetConnectionSecurity() == CSSTARTTLSRequired)
      {
         /*
           Upon completion of the TLS handshake, the SMTP protocol is reset to
           the initial state (the state in SMTP after a server issues a 220
           service ready greeting). The server MUST discard any knowledge
           obtained from the client, such as the argument to the EHLO command,
           which was not obtained from the TLS negotiation itself.
         */

         helo_host_.Empty();
         start_tls_used_ = true;
         ResetLoginCredentials_();
         ResetCurrentMessage_();

         EnqueueRead();
      }
   }

   void 
   SMTPConnection::SendBanner_()
   {

      String sWelcome = Configuration::Instance()->GetSMTPConfiguration()->GetWelcomeMessage();
      
      String sESMTP = " ESMTP";

      String sData = "220 ";

      if (sWelcome.IsEmpty())
         sData += Utilities::ComputerName() + sESMTP;
      else if (!sWelcome.EndsWith(sESMTP))
         sData += sWelcome + sESMTP;
      else
         sData += sWelcome;

      EnqueueWrite_(sData);

      EnqueueRead();

   }

   AnsiString 
   SMTPConnection::GetCommandSeparator() const
   {
      return "\r\n";
   }

   eSMTPCommandTypes
   SMTPConnection::GetCommandType_(const String &sFirstWord)
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
      else if (sFirstWord == _T("STARTTLS"))
         return SMTP_COMMAND_STARTTLS;

      return SMTP_COMMAND_UNKNOWN;
   }


   void 
   SMTPConnection::LogClientCommand_(const String &sClientData)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Logs one client command.
   //---------------------------------------------------------------------------()
   {
      if (Logger::Instance()->GetLogSMTP())
      {

         String sLogData = sClientData;

         String sRegex = "^(?>AUTH PLAIN )((?:[A-Z\\d+/]{4})*(?:[A-Z\\d+/]{3}=|[A-Z\\d+/]{2}==)?)$";
         boost::wregex expression(sRegex, boost::wregex::icase);
         boost::wsmatch matches;
         // AUTH PLAIN command and both user name and password in line. 
         if (current_state_ == HEADER && boost::regex_match(sLogData, matches, expression))
         {
            if (matches.size() > 0)
            {
               // Both user name and password in line.
               String sAuthentication;
               String sBase64Encoded = matches[1];
               StringParser::Base64Decode(sBase64Encoded, sAuthentication);
               std::vector<String> plain_args = StringParser::SplitString(sAuthentication, "\t");

               if (plain_args.size() == 3 && plain_args[1].GetLength() > 0)
               {
                  // Extract the username from the decoded string.
                  String username = plain_args[1];
                  String usernameBase64Encoded;
                  StringParser::Base64Encode(username, usernameBase64Encoded);
                  sLogData = "AUTH PLAIN " + usernameBase64Encoded + " ***";
               }
               else
               {
                  sLogData = "AUTH PLAIN ***";
               }
            }
         }
         else if (current_state_ == SMTPUSERNAME && requestedAuthenticationType_ == AUTH_PLAIN)
         {
            // Both user name and password in line.
            String sAuthentication;
            StringParser::Base64Decode(sClientData, sAuthentication);
            std::vector<String> plain_args = StringParser::SplitString(sAuthentication, "\t");

            if (plain_args.size() == 3 && plain_args[1].GetLength() > 0)
            {
               // Extract the username from the decoded string.
               String username = plain_args[1];
               String usernameBase64Encoded;
               StringParser::Base64Encode(username, usernameBase64Encoded);
               sLogData = usernameBase64Encoded + " ***";
            }
            else 
            {
               sLogData = "***";
            }
         }
         else if (current_state_ == SMTPUPASSWORD)
         {
            sLogData = "***";
         }         
         
         // Append
         sLogData = "RECEIVED: " + sLogData;

         LOG_SMTP(GetSessionID(), GetIPAddressString(), sLogData);      
      }
   }

   void
   SMTPConnection::ParseData(const AnsiString &sRequest)
   {
      InternalParseData(sRequest);

      if (pending_disconnect_ == false)
      {
         switch (current_state_)
         {
         case DATA:
            EnqueueRead("");
            break;
         case STARTTLS:
            break;
         default:
            EnqueueRead();
            break;
         }
      }
   }

   void
   SMTPConnection::InternalParseData(const AnsiString &sRequest)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Parses a clients SMTP command in ASCII mode.
   //---------------------------------------------------------------------------()
   {
      LogClientCommand_(sRequest);

      if (sRequest.GetLength() > 510)
      {
         // This line is too long... is this an evil user?
         EnqueueWrite_("500 Line too long.");
         return;
      }

      int lFirstSpace = sRequest.Find(" ");
      
      String sFirstWord;
      if (lFirstSpace > -1)
         sFirstWord = sRequest.Mid(0,lFirstSpace);
      else
         sFirstWord = sRequest;

      sFirstWord.MakeUpper();

      eSMTPCommandTypes eCommandType = GetCommandType_(sFirstWord);

      // The following commands are available regardless of of state.
      switch (eCommandType)
      {
         case SMTP_COMMAND_HELP: ProtocolHELP_(); return; 
         case SMTP_COMMAND_EHLO: ProtocolEHLO_(sRequest); return;
         case SMTP_COMMAND_HELO: ProtocolHELO_(sRequest); return;
         case SMTP_COMMAND_QUIT: ProtocolQUIT_(); return;
         case SMTP_COMMAND_NOOP: ProtocolNOOP_(); return;
         case SMTP_COMMAND_RSET: ProtocolRSET_(); return;
      }

      switch (current_state_)
      {
         case INITIAL:
            {
               requestedAuthenticationType_ = AUTH_NONE;
               SendErrorResponse_(503, "Bad sequence of commands"); 
               break;
            }
         case HEADER:
            {
               switch (eCommandType)
               {
                  case SMTP_COMMAND_STARTTLS: ProtocolSTARTTLS_(sRequest); break;
                  case SMTP_COMMAND_AUTH: ProtocolAUTH_(sRequest); break;
                  case SMTP_COMMAND_MAIL: ProtocolMAIL_(sRequest); break;
                  case SMTP_COMMAND_RCPT: ProtocolRCPT_(sRequest); break;
                  case SMTP_COMMAND_TURN: EnqueueWrite_("502 TURN disallowed."); break;
                  case SMTP_COMMAND_ETRN: ProtocolETRN_(sRequest); break;
                  case SMTP_COMMAND_VRFY: EnqueueWrite_("502 VRFY disallowed."); break;
                  case SMTP_COMMAND_DATA: ProtocolDATA_(); break;
                  default:
                     SendErrorResponse_(503, "Bad sequence of commands"); 
               }
               break;
            }
         case SMTPUSERNAME:
            {
               if (requestedAuthenticationType_ == AUTH_LOGIN)
               {
                  ProtocolUsername_(sRequest);
               }
               else
               {
                  AuthenticateUsingPLAIN_(sRequest);
               }

               break;
            }
         case SMTPUPASSWORD:
            {
               ProtocolPassword_(sRequest);
               break;
            }
         default:
            {
               ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5500, "SMTPConnection::InternalParseData", 
                  Formatter::Format(_T("Received unexpected string data: {0}"), sRequest));
            }
      }
 
      return;
   }

   void 
   SMTPConnection::InitializeSpamProtectionType_(const String &sFromAddress)
   {
      // Check if spam protection is enabled for this IP address.
      if (!GetSecurityRange()->GetSpamProtection() ||
           SpamProtection::IsWhiteListed(sFromAddress, GetRemoteEndpointAddress()))
      {
         type_ = SPNone;
         return;
      }

      std::shared_ptr<IncomingRelays> incomingRelays = Configuration::Instance()->GetSMTPConfiguration()->GetIncomingRelays();
      // Check if we should do it before or after data transfer
      if (incomingRelays->IsIncomingRelay(GetRemoteEndpointAddress()))
         type_ = SPPostTransmission;
      else 
         type_ = SPPreTransmission;
   }

   void 
   SMTPConnection::ProtocolNOOP_()
   {
      EnqueueWrite_("250 OK");
   }

   void
   SMTPConnection::ProtocolRSET_()
   {
      // 530 Must issue STARTTLS first
      // to every command other than NOOP, EHLO, STARTTLS, or QUIT.
      if (!CheckStartTlsRequired_())
         return;

      ResetCurrentMessage_();

      EnqueueWrite_("250 OK");

      return;
   }

   void
   SMTPConnection::ProtocolMAIL_(const String &Request)
   {
      // 530 Must issue STARTTLS first
      // to every command other than NOOP, EHLO, STARTTLS, or QUIT.
      if (!CheckStartTlsRequired_())
         return;

      if (current_message_) 
      {
         EnqueueWrite_("503 Issue a reset if you want to start over"); 
         return;
      }
     
      if (Request.GetLength() < 10)
      {
         SendErrorResponse_(550, "Invalid syntax. Syntax should be MAIL FROM:<mailbox@domain>[crlf]");
         return;
      }

      if (!Request.StartsWith(_T("MAIL FROM:")))
      {
         SendErrorResponse_(550, "Invalid syntax. Syntax should be MAIL FROM:<mailbox@domain>[crlf]");
         return;
      }

      // Parse the contents of the MAIL FROM: command
      String sMailFromArguments = Request.Mid(10).Trim();
      
      String sFromAddress;
      String sParameters;
      if (!ParseAddressWithExtensions_(sMailFromArguments, sFromAddress, sParameters))
      {
         SendErrorResponse_(550, "Invalid syntax. Syntax should be MAIL FROM:<mailbox@domain>[crlf]");
         return;

      }

      sFromAddress = DefaultDomain::ApplyDefaultDomain(sFromAddress);

      if (!CheckIfValidSenderAddress(sFromAddress))
         return;

      // Parse the extensions 
      std::vector<String> vecParams = StringParser::SplitString(sParameters, " ");
      std::vector<String>::iterator iterParam = vecParams.begin();

      String sAuthParam;
      size_t iEstimatedMessageSize = 0;
      while (iterParam != vecParams.end())
      {
         String parameter = (*iterParam);
         if (parameter.Left(4).CompareNoCase(_T("SIZE")) == 0)
            iEstimatedMessageSize = _ttoi(parameter.Mid(5));
         else if (parameter.Left(4).CompareNoCase(_T("AUTH")) == 0)
            sAuthParam = parameter.Mid(5);
         else
         {
            ReportUnsupportedEsmtpExtension_(parameter);
            return;
         }

         iterParam++;
      }

      // Initialize spam protection now when we know the sender address.
      InitializeSpamProtectionType_(sFromAddress);

      // Apply domain name aliases to this domain name.
      std::shared_ptr<DomainAliases> pDA = ObjectCache::Instance()->GetDomainAliases();
      const String sAccountAddress = pDA->ApplyAliasesOnAddress(sFromAddress);

      // Pre-transmission spam protection.
      if (type_ == SPPreTransmission)
      {
         if (IniFileSettings::Instance()->GetDNSBLChecksAfterMailFrom())
         {
            // The message is not arriving from a white listed host or a host
            // which is configured to be a forwarding relay. This means that
            // we can start spam protection now.

            if (!DoSpamProtection_(SPPreTransmission, sFromAddress, helo_host_, GetRemoteEndpointAddress()))
               return;
         }
      }

      sender_domain_ = CacheContainer::Instance()->GetDomain(StringParser::ExtractDomain(sAccountAddress));
      sender_account_ = CacheContainer::Instance()->GetAccount(sAccountAddress);

      // Check the max size
      max_message_size_kb_ = GetMaxMessageSize_(sender_domain_);

      // Check if estimated message size exceedes our
      // maximum message size (according to RFC1653)
      if (max_message_size_kb_ > 0 && 
          iEstimatedMessageSize / 1024 > max_message_size_kb_)
      {
         // Message too big. Reject it.
         String sMessage;
         sMessage.Format(_T("552 Message size exceeds fixed maximum message size. Size: %d KB, Max size: %d KB"), 
               iEstimatedMessageSize / 1024, max_message_size_kb_);
         EnqueueWrite_(sMessage);
         return ;
      }
      
      if (re_authenticate_user_ && !ReAuthenticateUser())
         return;

      // Next time we do a mail from, we should re-authenticate the login credentials
      re_authenticate_user_ = true;

         current_message_ = std::shared_ptr<Message> (new Message);
      current_message_->SetFromAddress(sFromAddress);
      current_message_->SetState(Message::Delivering);
      
      EnqueueWrite_("250 OK"); 
   }

   bool 
   SMTPConnection::ReAuthenticateUser()
   {
      if (!isAuthenticated_)
      {
         // Nothing to re-authenticate
         return true;
      }
         
      std::shared_ptr<const Account> pAccount = PasswordValidator::ValidatePassword(username_, password_);
      
      if (pAccount)
         return true;
         
      // Reset login credentials
      ResetLoginCredentials_();      

      SendErrorResponse_(550, "Login credentials no longer valid. Please re-authenticate.");                      
      
      return false;
   }

   bool 
   SMTPConnection::CheckIfValidSenderAddress(const String &sFromAddress)
   {
      if (sFromAddress.IsEmpty())
      {
         // The user is trying to send an e-mail without
         // specifying an email address. Should we allow this?
         if (!smtpconf_->GetAllowMailFromNull())
         {
            // Nope, we should'nt... We send the below text even
            // though RFC 822 tells us not to...
            SendErrorResponse_(550, "Sender address must be specified.");             
            return false;
         }
      }
      else
      {
         if (!StringParser::IsValidEmailAddress(sFromAddress))
         {
            // The address is not valid...
            SendErrorResponse_(550, "The address is not valid.");
            return false;
         }
      }

      return true;
   }

   void
   SMTPConnection::ProtocolRCPT_(const String &Request)
   {
      cur_no_of_rcptto_ ++;

      // 530 Must issue STARTTLS first
      // to every command other than NOOP, EHLO, STARTTLS, or QUIT.
      if (!CheckStartTlsRequired_())
         return;

      if (!current_message_) 
      {
         EnqueueWrite_("503 Must have sender first."); 
         return;
      }

      if (!Request.StartsWith(_T("RCPT TO:")))
      {
         SendErrorResponse_(550, "Invalid syntax. Syntax should be RCPT TO:<mailbox@domain>[crlf]");
         return;
      }

      // Parse the contents of the RCPT TO: command
      String sRcptToArguments = Request.Mid(8).Trim();

      String sRecipientAddress;
      String sParameters;
      
      if (!ParseAddressWithExtensions_(sRcptToArguments, sRecipientAddress, sParameters))
      {
         SendErrorResponse_(550, "Invalid syntax. Syntax should be RCPT TO:<mailbox@domain>[crlf]");
         return;
      }

      std::vector<String> vecParams = StringParser::SplitString(sParameters, " ");
      auto iterParam = vecParams.begin();

      // Parse the extensions 
      if (iterParam != vecParams.end())
      {
         String parameter = *iterParam;
         ReportUnsupportedEsmtpExtension_(parameter);
         return;
      }

      sRecipientAddress = DefaultDomain::ApplyDefaultDomain(sRecipientAddress);

      if (!StringParser::IsValidEmailAddress(sRecipientAddress))
      {
         // The address is not valid...
         SendErrorResponse_(550, "A valid address is required.");
         return;
      }

      if (current_message_->GetRecipients()->GetCount() >= MaxNumberOfRecipients)
      {
         // The user has added too many recipients for this message. Let's not try
         // to deliver it.
         SendErrorResponse_(550, "Too many recipients.");
         return;
      }


      String sErrMsg = "";
      bool localDelivery = false;
      
      RecipientParser::DeliveryPossibility dp = recipientParser_.CheckDeliveryPossibility(isAuthenticated_, current_message_->GetFromAddress(), sRecipientAddress, sErrMsg, localDelivery, 0);

      if (dp != RecipientParser::DP_Possible)
      {
         AWStats::LogDeliveryFailure(GetIPAddressString(), current_message_->GetFromAddress(), sRecipientAddress, 550);
         
         SendErrorResponse_(550, sErrMsg);
         return;
      }

      bool localSender = GetIsLocalSender_();

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
         SendErrorResponse_(550, "Delivery is not allowed to this address.");
         AWStats::LogDeliveryFailure(GetIPAddressString(), current_message_->GetFromAddress(), sRecipientAddress, 550);
         return;
      }

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
      if (authenticationRequired && !isAuthenticated_)
      {
         // Authentication is required, but the user hasn't authenticated.
         SendErrorResponse_(530, "SMTP authentication is required.");
         AWStats::LogDeliveryFailure(GetIPAddressString(), current_message_->GetFromAddress(), sRecipientAddress, 530);
         return;
      }

      // Pre-transmission spam protection.
      if (type_ == SPPreTransmission)
      {
         if (!IniFileSettings::Instance()->GetDNSBLChecksAfterMailFrom())
         {
            // The message is not arriving from a white listed host or a host
            // which is configured to be a forwarding relay. This means that
            // we can start spam protection now.

            if (!DoSpamProtection_(SPPreTransmission, current_message_->GetFromAddress(), helo_host_, GetRemoteEndpointAddress()))
            {
               AWStats::LogDeliveryFailure(GetIPAddressString(), current_message_->GetFromAddress(), sRecipientAddress, 550);
               return;
            }
         }
      }

      if (GetDoSpamProtection_())
      {
         std::shared_ptr<DomainAliases> pDA = ObjectCache::Instance()->GetDomainAliases();
         const String sToAddress = pDA->ApplyAliasesOnAddress(sRecipientAddress);

         if (!SpamProtection::Instance()->PerformGreyListing(current_message_, spam_test_results_, sToAddress, GetRemoteEndpointAddress()))
         {
            if (current_message_->GetFromAddress().IsEmpty())
            {
               // We got a message with an empty sender address.
               // When this happens, we should delay the greylist-reject 
               // until after the DATA command. The reason for this is
               // that this may be a SMTP callback from another server
               // that is veriying that the recipient exists, using the
               // RCPT TO command. And we don't want to delay that.
               rejected_by_delayed_grey_listing_ = true;
            }
            else
            {
               // The sender is greylisted. We don't log to awstats here,
               // since we tell the client to try again later.
               SendErrorResponse_(451, "Please try again later.");
               return;
            }
         }
      }

      // OK, the recipient is acceptable.
      std::shared_ptr<MessageRecipients> pRecipients = current_message_->GetRecipients();
      bool recipientOK = false;
      recipientParser_.CreateMessageRecipientList(sRecipientAddress, pRecipients, recipientOK);

      if (!recipientOK)
      {
         SendErrorResponse_(550, CONST_UNKNOWN_USER);
         return;
      }
   
      EnqueueWrite_("250 OK");
   }

   bool
   SMTPConnection::DoSpamProtection_(SpamProtectionType spType, const String &sFromAddress, const String &hostName, const IPAddress & lIPAddress)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Does IP based spam protection. Returns true if we should
   // continue delivery, false otherwise.   
   //---------------------------------------------------------------------------()
   {
      if (!GetDoSpamProtection_())
         return true;

      if (spType == SPPreTransmission)
      {
         std::set<std::shared_ptr<SpamTestResult> > setResult = 
            SpamProtection::Instance()->RunPreTransmissionTests(sFromAddress, lIPAddress, GetRemoteEndpointAddress(), hostName);

         spam_test_results_.insert(setResult.begin(), setResult.end());
      }
      else if (spType == SPPostTransmission)
      {
         std::set<std::shared_ptr<SpamTestResult> > setResult = 
            SpamProtection::Instance()->RunPostTransmissionTests(sFromAddress, lIPAddress, GetRemoteEndpointAddress(), current_message_);

         spam_test_results_.insert(setResult.begin(), setResult.end());

      }

      int iTotalSpamScore = SpamProtection::CalculateTotalSpamScore(spam_test_results_);

      int deleteThreshold = Configuration::Instance()->GetAntiSpamConfiguration().GetSpamDeleteThreshold();
      int markThreshold = Configuration::Instance()->GetAntiSpamConfiguration().GetSpamMarkThreshold();

      if (deleteThreshold > 0 && iTotalSpamScore >= deleteThreshold)
      {
         ServerStatus::Instance()->OnSpamMessageDetected();

         // Generate a text string to send to the client.
         String messageText = GetSpamTestResultMessage_(spam_test_results_);

         if (spType == SPPreTransmission)
            EnqueueWrite_("550 " + messageText);
         else
            EnqueueWrite_("554 " + messageText);

         String sLogMessage;
         sLogMessage.Format(_T("hMailServer SpamProtection rejected RCPT (Sender: %s, IP:%s, Reason: %s)"), sFromAddress.c_str(), String(GetIPAddressString()).c_str(), messageText.c_str());
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
   SMTPConnection::GetSpamTestResultMessage_(std::set<std::shared_ptr<SpamTestResult> > testResults) const
   {
      for(std::shared_ptr<SpamTestResult> result : testResults)
      {
         if (result->GetResult() == SpamTestResult::Fail)
            return result->GetMessage();
      }

      return "";
   }

   void
   SMTPConnection::ProtocolQUIT_()
   {
      // Reset the message here in case a message transmission has started, 
      // but hasn't ended. This can happen if the client sends DATA and then
      // the actual email message in the same buffer (which would be a RFC-violation).
      ResetCurrentMessage_();

      EnqueueWrite_("221 goodbye");
      
      pending_disconnect_ = true;
      EnqueueDisconnect();
   }

   void 
   SMTPConnection::AppendMessageHeaders_()
   {
      if (trace_headers_written_)
      {
         std::shared_ptr<MimeHeader> original_headers = Utilities::GetMimeHeader(transmission_buffer_->GetBuffer()->GetBuffer(), transmission_buffer_->GetBuffer()->GetSize());

         SMTPMessageHeaderCreator header_creator(username_, GetIPAddressString(), isAuthenticated_, helo_host_, original_headers);
         
         if (IsSSLConnection())
            header_creator.SetCipherInfo(GetCipherInfo());

         AnsiString new_headers = header_creator.Create();
         
         std::shared_ptr<ByteBuffer> new_data = std::shared_ptr<ByteBuffer>(new ByteBuffer);
         new_data->Add((BYTE*)new_headers.GetBuffer(), new_headers.GetLength());
         new_data->Add(transmission_buffer_->GetBuffer()->GetBuffer(), transmission_buffer_->GetBuffer()->GetSize());
        
         // Add to the original buffer
         transmission_buffer_->GetBuffer()->Empty();
         transmission_buffer_->GetBuffer()->Add(new_data);

         trace_headers_written_ = false;
      }
   }

   void
   SMTPConnection::ParseData(std::shared_ptr<ByteBuffer> pBuf)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Parses a clients SMTP command in Binary mode.
   //---------------------------------------------------------------------------()
   {
      // Move the data from the incoming buffer to the transparent transmission buffer.
      // If we've received more data than the max message size, don't save it.

      transmission_buffer_->Append(pBuf->GetBuffer(), pBuf->GetSize());

      // We need current message size in KB
      size_t iBufSizeKB = transmission_buffer_->GetSize() / 1024;

      // Clear the old buffer
      pBuf->Empty();

      // Check if it's time to flush.
      if (transmission_buffer_->GetRequiresFlush())
      {
         // We need to prepend the transmission buffer
         // with the headers...
         AppendMessageHeaders_();
      }

      // Flush the transmission buffer
      transmission_buffer_->Flush();

      if (!transmission_buffer_->GetTransmissionEnded())
      {

         String sLogData;
         size_t iMaxSizeDrop = IniFileSettings::Instance()->GetSMTPDMaxSizeDrop();
         if (iMaxSizeDrop > 0 && iBufSizeKB >= iMaxSizeDrop) 
         {
            sLogData.Format(_T("Size: %d KB, Max size: %d KB - DROP!!"), 
            iBufSizeKB, iMaxSizeDrop);
            LOG_SMTP(GetSessionID(), GetIPAddressString(), sLogData);      
            String sMessage;
            sMessage.Format(_T("552 Message size exceeds the drop maximum message size. Size: %d KB, Max size: %d KB - DROP!"), 
                iBufSizeKB, iMaxSizeDrop);
            EnqueueWrite_(sMessage);
            LogAwstatsMessageRejected_();
            ResetCurrentMessage_();
            SetReceiveBinary(false);
            pending_disconnect_ = true;
            EnqueueDisconnect();
            return;

         } 
         else 
         {
            // We need more data.
            EnqueueRead("");
            return;
         }
      }

      // Since this may be a time-consuming task, do it asynchronously
      std::shared_ptr<AsynchronousTask<TCPConnection> > finalizationTask = 
         std::shared_ptr<AsynchronousTask<TCPConnection> >(new AsynchronousTask<TCPConnection>
            (std::bind(&SMTPConnection::HandleSMTPFinalizationTaskCompleted_, this), shared_from_this()));
      
      Application::Instance()->GetAsyncWorkQueue()->AddTask(finalizationTask);
   }
   
   void
   SMTPConnection::HandleSMTPFinalizationTaskCompleted_()
   {
      if (!DoPreAcceptSpamProtection_())
      {
         // This message was stopped by spam protection. The user either needs
         // to quit or start from rset again.
         LogAwstatsMessageRejected_();

         ResetCurrentMessage_();
         SetReceiveBinary(false);
         EnqueueRead();
         return;
      }

      DoPreAcceptMessageModifications_();

      // Transmission has ended.
      current_message_->SetSize(FileUtilities::FileSize(PersistentMessage::GetFileName(current_message_)));


      // Let's archive message we just received
      String sArchiveDir = IniFileSettings::Instance()->GetArchiveDir();

      if (!sArchiveDir.empty()) 
      {
         LOG_SMTP(GetSessionID(), GetIPAddressString(), "Archiving..");      

         bool bArchiveHardlinks = IniFileSettings::Instance()->GetArchiveHardlinks();
         String _messageFileName;
         String sFileNameExclPath;
         String sMessageArchivePath;
         String sFromAddress1 = current_message_->GetFromAddress();
         std::vector<String> vecParams1 = StringParser::SplitString(sFromAddress1,  "@");

         // We need exactly 2 or not an email address
         if (vecParams1.size() == 2)
         {
            String sResponse;
            String sSenderName = vecParams1[0];
            sSenderName = sSenderName.ToLower();
            String sSenderDomain = vecParams1[1];
            sSenderDomain = sSenderDomain.ToLower();
            bool blocalSender1 = GetIsLocalSender_();

            if (blocalSender1)
            {
               // First copy goes to local sender
               _messageFileName = PersistentMessage::GetFileName(current_message_);
               sFileNameExclPath = FileUtilities::GetFileNameFromFullPath(_messageFileName);
               sMessageArchivePath = sArchiveDir + "\\" + sSenderDomain + "\\" + sSenderName + "\\Sent-" + sFileNameExclPath;

               LOG_SMTP(GetSessionID(), GetIPAddressString(), "Local sender: " + sFromAddress1 + ". Putting in user folder: " + sMessageArchivePath);      

               FileUtilities::Copy(_messageFileName, sMessageArchivePath, true);
            }
            else
            {
               LOG_SMTP(GetSessionID(), GetIPAddressString(), "Non local sender, putting in common Inbound folder..");      

               // First copy goes to common archive folder instead
               _messageFileName = PersistentMessage::GetFileName(current_message_);
               sFileNameExclPath = FileUtilities::GetFileNameFromFullPath(_messageFileName);
               sMessageArchivePath = sArchiveDir + "\\Inbound\\" + sFileNameExclPath;

               FileUtilities::Copy(_messageFileName, sMessageArchivePath, true);
            }

            String sMessageArchivePath2;

            // Now create hardlink/copy for each *local* recipient
            std::shared_ptr<const Domain> pDomaintmp;
            bool bDomainIsLocal = false;

            const std::vector<std::shared_ptr<MessageRecipient> > vecRecipients = current_message_->GetRecipients()->GetVector();
            std::vector<std::shared_ptr<MessageRecipient> >::const_iterator iterRecipient = vecRecipients.begin();
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
                        FileUtilities::CreateDirectory(sArchiveDir + "\\" + sSenderDomain + "\\" + sSenderName);
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

            _messageFileName = PersistentMessage::GetFileName(current_message_);
            sFileNameExclPath = FileUtilities::GetFileNameFromFullPath(_messageFileName);
            sMessageArchivePath = sArchiveDir + "\\Error\\" + sFileNameExclPath;
            FileUtilities::Copy(_messageFileName, sMessageArchivePath, true);
         }
      }   

      float dTime = ((float) GetTickCount() - (float) message_start_tc_) / (float) 1000;
      double dTCDiff = Math::Round(dTime ,3);

      if (OnPreAcceptTransfer_())
      {
         // Add the message to the database.
         if (PersistentMessage::SaveObject(current_message_))
         {
            // Make sure the transmission buffer has released the handle
            // to the file.
            if (transmission_buffer_)
               transmission_buffer_.reset();

            // Add this message to the delivery queue cache. This way,
            // we won't have to read it from the database.
            MessageCache::Instance()->AddMessage(current_message_);

            // Free the message, so we don't access it the same time
            // as the SMTP delivery manager.
            current_message_.reset();

            // Tell the deliverer that a new message is pending. This
            // will cause the SMTP delivery manager to start a new delivery
            // thread and deliver the message.
            Application::Instance()->SubmitPendingEmail();

            // Reset the spam protection results.
            spam_test_results_.clear();

            // Tell the client that everything went fine. This
            // will cause the client to either disconnect or to
            // start a new message.
            String sResponse;
            sResponse.Format(_T("250 Queued (%.3f seconds)"), dTCDiff);
            EnqueueWrite_(sResponse);

            // The message delivery is complete, or
            // it has failed. Any way, we should start
            // a new message.
            current_state_ = HEADER;
         }
         else
         {
            // The delivery of the message failed. This may happen if tables are
            // corrupt in the database. We now return an error message to the sender. 
            // Hopefully, the sending server will retry later. 
            EnqueueWrite_("554 Your message was received but it could not be saved. Please retry later.");

            // Delete the file now since we could not save it in the database.
            ResetCurrentMessage_();
            
         }
      }
      else
      {
         // The message was rejected by _OnPreAcceptTransfer. For example
         // this may happen if the message was rejected by a script subscribing
         // to OnAcceptMessage.
         ResetCurrentMessage_();
      }

      SetReceiveBinary(false);
      EnqueueRead();
   }

   void
   SMTPConnection::DoPreAcceptMessageModifications_()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Make changes to the message before it's accepted for delivery. This is
   // for example where message signature and spam-headers are added.
   //---------------------------------------------------------------------------()
   {
      std::shared_ptr<MessageData> pMsgData;

      // Check if we should add a spam header.
      int iTotalSpamScore = SpamProtection::CalculateTotalSpamScore(spam_test_results_);
      int iSpamMarkThreshold = Configuration::Instance()->GetAntiSpamConfiguration().GetSpamMarkThreshold();

      bool classifiedAsSpam = iTotalSpamScore >= iSpamMarkThreshold;

      if (classifiedAsSpam) {
         pMsgData = SpamProtection::AddSpamScoreHeaders(current_message_, spam_test_results_, classifiedAsSpam);

         // Increase the spam-counter
         ServerStatus::Instance()->OnSpamMessageDetected();
      }

      SetMessageSignature_(pMsgData);

      if (pMsgData)
         pMsgData->Write(PersistentMessage::GetFileName(current_message_));
   }

   void
   SMTPConnection::SetMessageSignature_(std::shared_ptr<MessageData> &pMessageData)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Sets the signature of the message, based on the signature in the account
   // settings and domain settings.
   //---------------------------------------------------------------------------()
   {
      std::shared_ptr<SignatureAdder> pSignatureAdder = std::shared_ptr<SignatureAdder>(new SignatureAdder);
      pSignatureAdder->SetSignature(current_message_, sender_domain_, sender_account_, pMessageData);
   }

   bool
   SMTPConnection::OnPreAcceptTransfer_()
   {
      if (transmission_buffer_->GetCancelTransmission())
      {
         EnqueueWrite_("554 "  + transmission_buffer_->GetCancelMessage());
         LogAwstatsMessageRejected_();
         return false;
      }

      const String fileName = PersistentMessage::GetFileName(current_message_);

      if (!FileUtilities::Exists(fileName))
      {
         HandleUnableToSaveMessageDataFile_(fileName);
         return false;
      }


      // Check so that message isn't to big. Max message
      // size is specified in KB.
      if (max_message_size_kb_ > 0 && (transmission_buffer_->GetSize() / 1024) > max_message_size_kb_)
      {
         String sMessage;
         sMessage.Format(_T("554 Rejected - Message size exceeds fixed maximum message size. Size: %d KB, Max size: %d KB"), 
            transmission_buffer_->GetSize() / 1024, max_message_size_kb_);
         EnqueueWrite_(sMessage);
         LogAwstatsMessageRejected_();
         return false;
      }

      // Check for bare LF's.
      if (!Configuration::Instance()->GetSMTPConfiguration()->GetAllowIncorrectLineEndings())
      {
         if (!CheckLineEndings_())
         {
            String sMessage;
            sMessage.Format(_T("554 Rejected - Message containing bare LF's."));
            
            EnqueueWrite_(sMessage);
            LogAwstatsMessageRejected_();
            return false;
         }

      }

      if (Configuration::Instance()->GetUseScriptServer())
      {
         std::shared_ptr<ScriptObjectContainer> pContainer = std::shared_ptr<ScriptObjectContainer>(new ScriptObjectContainer);
         std::shared_ptr<Result> pResult = std::shared_ptr<Result>(new Result);
         std::shared_ptr<ClientInfo> pClientInfo = std::shared_ptr<ClientInfo>(new ClientInfo);

         pClientInfo->SetUsername(username_);
         pClientInfo->SetIPAddress(GetIPAddressString());
         pClientInfo->SetPort(GetLocalEndpointPort());
         pClientInfo->SetSessionID(GetSessionID());
         pClientInfo->SetHELO(helo_host_);
         pClientInfo->SetIsAuthenticated(isAuthenticated_);
         pClientInfo->SetIsEncryptedConnection(IsSSLConnection());
         if (IsSSLConnection())
         {
            auto cipher_info = GetCipherInfo();
            pClientInfo->SetCipherVersion(cipher_info.GetVersion().c_str());
            pClientInfo->SetCipherName(cipher_info.GetName().c_str());
            pClientInfo->SetCipherBits(cipher_info.GetBits());
         }

         pContainer->AddObject("HMAILSERVER_MESSAGE", current_message_, ScriptObject::OTMessage);
         pContainer->AddObject("HMAILSERVER_CLIENT", pClientInfo, ScriptObject::OTClient);
         pContainer->AddObject("Result", pResult, ScriptObject::OTResult);

         String sEventCaller = "OnAcceptMessage(HMAILSERVER_CLIENT, HMAILSERVER_MESSAGE)";
         ScriptServer::Instance()->FireEvent(ScriptServer::EventOnAcceptMessage, sEventCaller, pContainer);

         switch (pResult->GetValue())
         {
         case 1:
            {
               String sErrorMessage = "554 Rejected";
               EnqueueWrite_(sErrorMessage);
               LogAwstatsMessageRejected_();
               return false;
            }
         case 2:
            {
               String sErrorMessage = "554 " + pResult->GetMessage();
               EnqueueWrite_(sErrorMessage);
               LogAwstatsMessageRejected_();
               return false;
            }
         case 3:
            {
               String sErrorMessage = "453 " + pResult->GetMessage();
               EnqueueWrite_(sErrorMessage);
               LogAwstatsMessageRejected_();
               return false;
            }
         }
      }      

      if (GetSecurityRange()->GetVirusProtection())
      {
         current_message_->SetFlagVirusScan(true);
      }

      return true;
   }

   void 
   SMTPConnection::HandleUnableToSaveMessageDataFile_(const String &file_name)
   {
      String sErrorMsg = Formatter::Format("Rejected message because no mail data has been saved in file {0}", file_name);
      ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5019, "SMTPConnectionSMTPConnection::HandleUnableToSaveMessage_", sErrorMsg);

      EnqueueWrite_("451 Rejected - No data saved.");
      LogAwstatsMessageRejected_();
   }

   bool 
   SMTPConnection::CheckLineEndings_() const
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Checks if the message contains any CR with missing LF, or any LF with
   // missing CR.
   //---------------------------------------------------------------------------()
   {
      if (!current_message_)
         return false;

      const String fileName = PersistentMessage::GetFileName(current_message_);

      File oFile;


      std::shared_ptr<ByteBuffer> pBuffer;

      const int iChunkSize = 10000;

      try
      {
         oFile.Open(fileName, File::OTReadOnly);

         pBuffer = oFile.ReadChunk(iChunkSize);
      }
      catch (...)
      {
         return false;
      }

      while (pBuffer->GetSize() > 0)
      {
         // Check that buffer contains correct line endings.
         const char *pChar = pBuffer->GetCharBuffer();
         size_t iBufferSize = pBuffer->GetSize();

         if (iBufferSize >= 3)
         {
            for (size_t i = 3; i < iBufferSize - 3; i++)
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
         }

         // Read next chunk
         try
         {
            pBuffer = oFile.ReadChunk(iChunkSize);
         }
         catch (...)
         {
            return false;
         }
      }
      
      return true;
   }

   void 
   SMTPConnection::LogAwstatsMessageRejected_()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // If awstats logging is enabled, this function goes through all the recipients
   // of the message, and logs to the awstats log that they have been rejected.
   // This is used if a message is rejected after it has been transferred from the
   // client to the server.
   //---------------------------------------------------------------------------()
   {
      // Check that message exists, and that the awstats log is enabled.
      if (!current_message_ || !AWStats::GetEnabled())
         return;

      // Go through the recipients and log one row for each of them.
      String sFromAddress = current_message_->GetFromAddress();

      const std::vector<std::shared_ptr<MessageRecipient> > vecRecipients = current_message_->GetRecipients()->GetVector();
      std::vector<std::shared_ptr<MessageRecipient> >::const_iterator iterRecipient = vecRecipients.begin();
      while (iterRecipient != vecRecipients.end())
      {
         String sRecipientAddress = (*iterRecipient)->GetAddress();

         // Log the error message
         AWStats::LogDeliveryFailure(GetIPAddressString(), sFromAddress, sRecipientAddress, 554);

         iterRecipient++;
      }

   }

   void
   SMTPConnection::ResetCurrentMessage_()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Reset the transmission buffer to free
   // any handles it has opened to the message
   // file
   //---------------------------------------------------------------------------()
   {
      if (transmission_buffer_)
      {
         transmission_buffer_.reset();
      }

      // Reset the current message.
      if (current_message_)
      {
         // This message isn't complete, so we should delete it from disk now.
         std::shared_ptr<Account> emptyAccount;

         PersistentMessage::DeleteFile(emptyAccount, current_message_);

         // Reset message object
         current_message_.reset();
      }

      rejected_by_delayed_grey_listing_ = false;

      sender_domain_.reset();
      sender_account_.reset();

      spam_test_results_.clear();

      // Reset the number of RCPT TO's for this 
      // message.
      cur_no_of_rcptto_ = 0;

      // Switch back to normal ASCII mode and start of session, in
      // case we are in binary transmission mode.
      current_state_ = HEADER;
   }


   bool
   SMTPConnection::SendEHLOKeywords_()
   {
      String sComputerName = Utilities::ComputerName(); 

      String sData = "250-" + sComputerName;
      
      // Append size keyword
      {
         String sSizeKeyword;
         int iMaxSize = smtpconf_->GetMaxMessageSize() * 1024;
         if (iMaxSize > 0)
            sSizeKeyword.Format(_T("\r\n250-SIZE %d"), iMaxSize);
         else
            sSizeKeyword.Format(_T("\r\n250-SIZE"));
         sData += sSizeKeyword;
      }

      if (!IsSSLConnection())
      {
         if (GetConnectionSecurity() == CSSTARTTLSOptional ||
             GetConnectionSecurity() == CSSTARTTLSRequired)
         {
            sData += "\r\n250-STARTTLS";
         }
      }

      if (GetAuthIsEnabled_() && (IsSSLConnection() || GetConnectionSecurity() != CSSTARTTLSRequired))
      {
         String sAuth = "\r\n250-AUTH LOGIN";

         if (smtpconf_->GetAuthAllowPlainText())
            sAuth += " PLAIN";

         sData += sAuth;
      }

      sData += "\r\n250 HELP";

      EnqueueWrite_(sData);
   
      return true;
   }

   void
   SMTPConnection::OnConnectionTimeout()
   {
      EnqueueWrite_("421 Connection timeout.");
   }
  
   void
   SMTPConnection::OnExcessiveDataReceived()
   {
      ResetCurrentMessage_();
      EnqueueWrite_("421 Excessive amounts of data sent to server.");
   }

   void 
   SMTPConnection::EnqueueWrite_(const String &sData)
   {
      if (Logger::Instance()->GetLogSMTP())
      {
         String sLogData = "SENT: " + sData;

         sLogData.TrimRight(_T("\r\n"));

         LOG_SMTP(GetSessionID(), GetIPAddressString(), sLogData);
      }

      EnqueueWrite(sData + "\r\n");
   }

   bool
   SMTPConnection::ReadDomainAddressFromHelo_(const  String &sRequest)
   {
      int iFirstSpace = sRequest.Find(_T(" "));
      
      if (iFirstSpace == -1)
      {
         // No host name has been specified => RFC violation
         return false;
      }

      // Cut out the string after the space.
      helo_host_ = sRequest.Mid(iFirstSpace + 1);

      // Trim it incase of leading or trailing spaces.
      helo_host_ = helo_host_.Trim();

      if (helo_host_.IsEmpty())
         return false;

      return true;

   }

   void
   SMTPConnection::ProtocolEHLO_(const String &sRequest)
   {

      if (!ReadDomainAddressFromHelo_(sRequest))
      {
         // The client did not supply a parameter to
         // the helo command which is syntaxically
         // incorrect. Reject.
         SendErrorResponse_(501, "EHLO Invalid domain address.");
         return;
      }

      //
      // Event OnHELO
      //
      if (Configuration::Instance()->GetUseScriptServer())
      {
         std::shared_ptr<ScriptObjectContainer> pContainer = std::shared_ptr<ScriptObjectContainer>(new ScriptObjectContainer);
         std::shared_ptr<Result> pResult = std::shared_ptr<Result>(new Result);
         std::shared_ptr<ClientInfo> pClientInfo = std::shared_ptr<ClientInfo>(new ClientInfo);

         pClientInfo->SetIPAddress(GetIPAddressString());
         pClientInfo->SetPort(GetLocalEndpointPort());
         pClientInfo->SetSessionID(GetSessionID());
         pClientInfo->SetHELO(helo_host_);
         pClientInfo->SetIsEncryptedConnection(IsSSLConnection());
         if (IsSSLConnection())
         {
            auto cipher_info = GetCipherInfo();
            pClientInfo->SetCipherVersion(cipher_info.GetVersion().c_str());
            pClientInfo->SetCipherName(cipher_info.GetName().c_str());
            pClientInfo->SetCipherBits(cipher_info.GetBits());
         }

         pContainer->AddObject("HMAILSERVER_CLIENT", pClientInfo, ScriptObject::OTClient);
         pContainer->AddObject("Result", pResult, ScriptObject::OTResult);

         String sEventCaller = "OnHELO(HMAILSERVER_CLIENT)";
         ScriptServer::Instance()->FireEvent(ScriptServer::EventOnHELO, sEventCaller, pContainer);

         switch (pResult->GetValue())
         {
         case 1:
         {
            String sErrorMessage = "554 Rejected";
            EnqueueWrite_(sErrorMessage);
            LogAwstatsMessageRejected_();
            return;
         }
         case 2:
         {
            String sErrorMessage = "554 " + pResult->GetMessage();
            EnqueueWrite_(sErrorMessage);
            LogAwstatsMessageRejected_();
            return;
         }
         case 3:
         {
            String sErrorMessage = "453 " + pResult->GetMessage();
            EnqueueWrite_(sErrorMessage);
            LogAwstatsMessageRejected_();
            return;
         }
         }
      }

      SendEHLOKeywords_();

      if (current_state_ == INITIAL)
         current_state_ = HEADER;
   }
   
   void
   SMTPConnection::ProtocolHELO_(const String &sRequest)
   {
      if (!ReadDomainAddressFromHelo_(sRequest))
      {
         // The client did not supply a parameter to
         // the helo command which is syntaxically
         // incorrect. Reject.
         SendErrorResponse_(501, "HELO Invalid domain address.");
         return;
      }

      //
      // Event OnHELO
      //
      if (Configuration::Instance()->GetUseScriptServer())
      {
         std::shared_ptr<ScriptObjectContainer> pContainer = std::shared_ptr<ScriptObjectContainer>(new ScriptObjectContainer);
         std::shared_ptr<Result> pResult = std::shared_ptr<Result>(new Result);
         std::shared_ptr<ClientInfo> pClientInfo = std::shared_ptr<ClientInfo>(new ClientInfo);

         pClientInfo->SetIPAddress(GetIPAddressString());
         pClientInfo->SetPort(GetLocalEndpointPort());
         pClientInfo->SetSessionID(GetSessionID());
         pClientInfo->SetHELO(helo_host_);
         pClientInfo->SetIsEncryptedConnection(IsSSLConnection());
         if (IsSSLConnection())
         {
            auto cipher_info = GetCipherInfo();
            pClientInfo->SetCipherVersion(cipher_info.GetVersion().c_str());
            pClientInfo->SetCipherName(cipher_info.GetName().c_str());
            pClientInfo->SetCipherBits(cipher_info.GetBits());
         }

         pContainer->AddObject("HMAILSERVER_CLIENT", pClientInfo, ScriptObject::OTClient);
         pContainer->AddObject("Result", pResult, ScriptObject::OTResult);

         String sEventCaller = "OnHELO(HMAILSERVER_CLIENT)";
         ScriptServer::Instance()->FireEvent(ScriptServer::EventOnHELO, sEventCaller, pContainer);

         switch (pResult->GetValue())
         {
         case 1:
         {
            String sErrorMessage = "554 Rejected";
            EnqueueWrite_(sErrorMessage);
            LogAwstatsMessageRejected_();
            return;
         }
         case 2:
         {
            String sErrorMessage = "554 " + pResult->GetMessage();
            EnqueueWrite_(sErrorMessage);
            LogAwstatsMessageRejected_();
            return;
         }
         case 3:
         {
            String sErrorMessage = "453 " + pResult->GetMessage();
            EnqueueWrite_(sErrorMessage);
            LogAwstatsMessageRejected_();
            return;
         }
         }
      }

      EnqueueWrite_("250 Hello.");

      if (current_state_ == INITIAL)
         current_state_ = HEADER;

   }

   void
   SMTPConnection::ProtocolHELP_()
   {
      // 530 Must issue STARTTLS first
      // to every command other than NOOP, EHLO, STARTTLS, or QUIT.
      if (!CheckStartTlsRequired_())
         return;

      // The following code is to test the error handling in production environments.
      // Crash simulation mode can be enabled in hMailServer.ini. 
      int crash_simulation_mode = Configuration::Instance()->GetCrashSimulationMode();
      if (crash_simulation_mode > 0)
         CrashSimulation::Execute(crash_simulation_mode);

      EnqueueWrite_("211 DATA HELO EHLO MAIL NOOP QUIT RCPT RSET SAML TURN VRFY");
   }

   void
   SMTPConnection::ProtocolDATA_()
   {
      // 530 Must issue STARTTLS first
      // to every command other than NOOP, EHLO, STARTTLS, or QUIT.
      if (!CheckStartTlsRequired_())
         return;
      
      if (!current_message_)
      {
         // User tried to send a mail without specifying a correct mail from or rcpt to.
         EnqueueWrite_("503 Must have sender and recipient first.");

         return;
      }  
      else if ( current_message_->GetRecipients()->GetCount() == 0)
      {
         // User tried to send a mail without specifying a correct mail from or rcpt to.
         EnqueueWrite_("503 Must have sender and recipient first.");

         return;
      }  

      // Let's add an event call on DATA so we can act on reception during SMTP conversation..
      if (Configuration::Instance()->GetUseScriptServer())
      {
         std::shared_ptr<ScriptObjectContainer> pContainer = std::shared_ptr<ScriptObjectContainer>(new ScriptObjectContainer);
         std::shared_ptr<Result> pResult = std::shared_ptr<Result>(new Result);
         std::shared_ptr<ClientInfo> pClientInfo = std::shared_ptr<ClientInfo>(new ClientInfo);

         pClientInfo->SetUsername(username_);
         pClientInfo->SetIPAddress(GetIPAddressString());
         pClientInfo->SetPort(GetLocalEndpointPort());
         pClientInfo->SetSessionID(GetSessionID());
         pClientInfo->SetHELO(helo_host_);
         pClientInfo->SetIsAuthenticated(isAuthenticated_);
         pClientInfo->SetIsEncryptedConnection(IsSSLConnection());
         if (IsSSLConnection())
         {
            auto cipher_info = GetCipherInfo();
            pClientInfo->SetCipherVersion(cipher_info.GetVersion().c_str());
            pClientInfo->SetCipherName(cipher_info.GetName().c_str());
            pClientInfo->SetCipherBits(cipher_info.GetBits());
         }

         pContainer->AddObject("HMAILSERVER_MESSAGE", current_message_, ScriptObject::OTMessage);
         pContainer->AddObject("HMAILSERVER_CLIENT", pClientInfo, ScriptObject::OTClient);
         pContainer->AddObject("Result", pResult, ScriptObject::OTResult);

         String sEventCaller = "OnSMTPData(HMAILSERVER_CLIENT, HMAILSERVER_MESSAGE)";
         ScriptServer::Instance()->FireEvent(ScriptServer::EventOnSMTPData, sEventCaller, pContainer);

         switch (pResult->GetValue())
         {
         case 1:
            {
               String sErrorMessage = "554 Rejected";
               EnqueueWrite_(sErrorMessage);
               LogAwstatsMessageRejected_();
               return;
            }
         case 2:
            {
               String sErrorMessage = "554 " + pResult->GetMessage();
               EnqueueWrite_(sErrorMessage);
               LogAwstatsMessageRejected_();
               return;
            }
         case 3:
            {
               String sErrorMessage = "453 " + pResult->GetMessage();
               EnqueueWrite_(sErrorMessage);
               LogAwstatsMessageRejected_();
               return;
            }
         }
      }      

      current_state_ = DATA;

      transmission_buffer_ = std::shared_ptr<TransparentTransmissionBuffer>(new TransparentTransmissionBuffer(false));
      if (!transmission_buffer_->Initialize(PersistentMessage::GetFileName(current_message_)))
      {
         HandleUnableToSaveMessageDataFile_(PersistentMessage::GetFileName(current_message_));
         return;
      }

      transmission_buffer_->SetMaxSizeKB(max_message_size_kb_);

      SetReceiveBinary(true);
      trace_headers_written_ = true;
      message_start_tc_ = GetTickCount();

      EnqueueWrite_("354 OK, send.");
   }

   bool 
   SMTPConnection::CheckStartTlsRequired_()
   {
      if (GetConnectionSecurity() == CSSTARTTLSRequired &&
          !IsSSLConnection())
      {
         SendErrorResponse_(530, "Must issue STARTTLS first.");
         return false;
      }

      return true;
   }

   void 
   SMTPConnection::ProtocolAUTH_(const String &sRequest)
   {
      // 530 Must issue STARTTLS first
      // to every command other than NOOP, EHLO, STARTTLS, or QUIT.
      if (!CheckStartTlsRequired_())
         return;

      if (!GetAuthIsEnabled_())
      {
         SendErrorResponse_(504, "Authentication not enabled.");
         return;
      }

      if (GetSecurityRange()->GetRequireTLSForAuth() && !IsSSLConnection())
      {
         SendErrorResponse_(530, "A SSL/TLS-connection is required for authentication.");
         return;
      }
	  
      // rfc4954 restrictions, After a successful AUTH command completes, 
      // a server MUST reject any further AUTH commands with a 503 reply.
      if (isAuthenticated_) 
      {
         SendErrorResponse_(503, "Already authenticated.");
         return;
      }

      requestedAuthenticationType_ = AUTH_NONE;

      std::vector<String> vecParams = StringParser::SplitString(sRequest,  " ");

      if (vecParams.size() == 1)
      {
         SendErrorResponse_(504, "Authentication type not specified.");
         return;
      }
      
      String sAuthenticationType = vecParams[1];
      sAuthenticationType.MakeUpper();

      if (sAuthenticationType == _T("LOGIN"))
      {
         requestedAuthenticationType_ = AUTH_LOGIN;

         String sResponse;

         if (vecParams.size() == 3)
         {
            // Fetch username from third parameter.
            StringParser::Base64Decode(vecParams[2], username_);
            current_state_ = SMTPUPASSWORD;

            StringParser::Base64Encode("Password:", sResponse);
         }
         else
         {
            current_state_ = SMTPUSERNAME;
            StringParser::Base64Encode("Username:", sResponse);
         }

         EnqueueWrite_("334 " + sResponse);
         return;

      }
      else if (sAuthenticationType == _T("PLAIN") && 
               smtpconf_->GetAuthAllowPlainText())
      {
         requestedAuthenticationType_ = AUTH_PLAIN;

         // Stupid user has selected plain text authentication.
         if (vecParams.size() == 3)
         {
            // Fetch username and password directly from command.
            AuthenticateUsingPLAIN_(vecParams[2]);
         }
         else
         {
            EnqueueWrite_("334 Log on");
            current_state_ = SMTPUSERNAME;
         }

         return;
      }

      SendErrorResponse_(504, "Authentication mechanism not supported.");
   }

   void 
   SMTPConnection::ProtocolUsername_(const String &sRequest)
   {
      StringParser::Base64Decode(sRequest, username_);
      String sEncoded;
      StringParser::Base64Encode("Password:", sEncoded);
      EnqueueWrite_("334 " + sEncoded);
      current_state_ = SMTPUPASSWORD;

   }

   void 
   SMTPConnection::ProtocolPassword_(const String &sRequest)
   {
      if (requestedAuthenticationType_ == AUTH_LOGIN)
         StringParser::Base64Decode(sRequest, password_);
      else if (requestedAuthenticationType_ == AUTH_PLAIN)
         password_ = sRequest;

      Authenticate_();
   }

   void 
   SMTPConnection::ProtocolSTARTTLS_(const String &sRequest)
   {
      if (GetConnectionSecurity() == CSSTARTTLSOptional ||
          GetConnectionSecurity() == CSSTARTTLSRequired)
      {
         const int commandLength = 8;

         auto trimmedRequest = sRequest;
         trimmedRequest.Trim();

         bool hasParameters = trimmedRequest.GetLength() > commandLength;

         if (hasParameters)
         {
            SendErrorResponse_(501, "Syntax error (no parameters allowed)");
            return;
         }

         EnqueueWrite_("220 Ready to start TLS");

         current_state_ = STARTTLS;

         EnqueueHandshake();
      }
      else
      {
         SendErrorResponse_(503, "Bad sequence of commands");
      }
   }

   void 
   SMTPConnection::ProtocolETRN_(const String &sRequest)
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
         SendErrorResponse_(500, "Syntax Error: No domain parameter included");
         LOG_SMTP(GetSessionID(), GetIPAddressString(), "SMTPDeliverer - ETRN - No domain parameter included");      
         return;
      }
      
      String sResponse;
      String sETRNDomain = vecParams[1];
      String sETRNDomain2 = sETRNDomain.ToLower();
      String sLogData;

      bool bIsRouteDomain = false;
      std::shared_ptr<Route> route = Configuration::Instance()->GetSMTPConfiguration()->GetRoutes()->GetItemByNameWithWildcardMatch(sETRNDomain.ToLower());

      // See if sender supplied param matches one of our domains
      if (route && route->GetName() == sETRNDomain2)
      {
         LOG_SMTP(GetSessionID(), GetIPAddressString(), "SMTPDeliverer - ETRN - Route found, continuing..");      

         std::shared_ptr<Routes> pRoutes = Configuration::Instance()->GetSMTPConfiguration()->GetRoutes();
         std::shared_ptr<Route> pRoute = pRoutes->GetItemByNameWithWildcardMatch(sETRNDomain.ToLower());

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
               EnqueueWrite_("250 OK, message queuing started for " + sETRNDomain.ToLower());
               LOG_SMTP(GetSessionID(), GetIPAddressString(), "SMTPDeliverer - ETRN - 250 OK, message queuing started.");      
            }
            else
            {
               EnqueueWrite_("458 Unable to queue messages for " + sETRNDomain.ToLower());
               LOG_SMTP(GetSessionID(), GetIPAddressString(), "SMTPDeliverer - ETRN - 458 Unable to queue messages");      
            }
         return;

       }
       else
       {
          // Send that we don't accept ETRN for that domain or invalid param
          EnqueueWrite_("458 Error getting info for " + sETRNDomain.ToLower());
          LOG_SMTP(GetSessionID(), GetIPAddressString(), "SMTPDeliverer - ETRN - Could not get Route values");      
          return;
       }
     }
     else
     {
         // Send that we don't accept ETRN for that domain or invalid param
         EnqueueWrite_("501 ETRN not supported for " + sETRNDomain.ToLower());
         LOG_SMTP(GetSessionID(), GetIPAddressString(), "SMTPDeliverer - ETRN - Domain is not Route");      
         return;
     }
   }

   void
   SMTPConnection::AuthenticateUsingPLAIN_(const String &sLine)
   {
      String sAuthentication;
      StringParser::Base64Decode(sLine, sAuthentication);
      std::vector<String> plain_args = StringParser::SplitString(sAuthentication, "\t");

      if (plain_args.size() != 3) 
      {
         RestartAuthentication_();
         return;
      }

      if (plain_args[1].GetLength() == 0 || plain_args[2].GetLength() == 0)
      {
         RestartAuthentication_();
         return;
      }

      username_ = plain_args[1];
      password_ = plain_args[2];

      // Authenticate the user.
      Authenticate_();      
   }

   void
   SMTPConnection::Authenticate_()
   {
      AccountLogon accountLogon;
      bool disconnect;
	  String sUsername = username_;

      std::shared_ptr<const Account> pAccount = accountLogon.Logon(GetRemoteEndpointAddress(), username_, password_, disconnect);
         
      if (disconnect)
      {
         SendErrorResponse_(535, "Authentication failed. Too many invalid logon attempts.");
         pending_disconnect_ = true;
         EnqueueDisconnect();
         return;
      }

      isAuthenticated_ = pAccount != nullptr;

      if (Configuration::Instance()->GetUseScriptServer())
      {
         std::shared_ptr<ScriptObjectContainer> pContainer = std::shared_ptr<ScriptObjectContainer>(new ScriptObjectContainer);
         std::shared_ptr<ClientInfo> pClientInfo = std::shared_ptr<ClientInfo>(new ClientInfo);

         pClientInfo->SetUsername(sUsername);
         pClientInfo->SetIPAddress(GetIPAddressString());
         pClientInfo->SetPort(GetLocalEndpointPort());
         pClientInfo->SetSessionID(GetSessionID());
         pClientInfo->SetHELO(helo_host_);
         pClientInfo->SetIsAuthenticated(isAuthenticated_);
         pClientInfo->SetIsEncryptedConnection(IsSSLConnection());
         if (IsSSLConnection())
         {
            auto cipher_info = GetCipherInfo();
            pClientInfo->SetCipherVersion(cipher_info.GetVersion().c_str());
            pClientInfo->SetCipherName(cipher_info.GetName().c_str());
            pClientInfo->SetCipherBits(cipher_info.GetBits());
         }

         pContainer->AddObject("HMAILSERVER_CLIENT", pClientInfo, ScriptObject::OTClient);

         String sEventCaller = "OnClientLogon(HMAILSERVER_CLIENT)";
         ScriptServer::Instance()->FireEvent(ScriptServer::EventOnClientLogon, sEventCaller, pContainer);
      }
     
      if (pAccount)
      {
         EnqueueWrite_("235 authenticated.");
         current_state_ = HEADER;
      }
      else
      {
         RestartAuthentication_();
      }
   }

   void 
   SMTPConnection::RestartAuthentication_()
   {
      ResetLoginCredentials_();
      
      SendErrorResponse_(535, "Authentication failed. Restarting authentication process.");
   }

   void 
   SMTPConnection::ResetLoginCredentials_()
   {
      requestedAuthenticationType_ = AUTH_NONE;
      isAuthenticated_ = false;

      current_state_ = HEADER;
      username_ = "";
      re_authenticate_user_ = false;
   }


   int 
   SMTPConnection::GetMaxMessageSize_(std::shared_ptr<const Domain> pDomain)
   {
      int iMaxMessageSizeKB = smtpconf_->GetMaxMessageSize();
      
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
   SMTPConnection::SendErrorResponse_(int iErrorCode, const String &sResponse)
   {
      if (iErrorCode >= 500 && iErrorCode <= 599)
      {
         cur_no_of_invalid_commands_++;

         if (Configuration::Instance()->GetDisconnectInvalidClients() &&
            cur_no_of_invalid_commands_ > Configuration::Instance()->GetMaximumIncorrectCommands())
         {
            // Disconnect
            EnqueueWrite_("Too many invalid commands. Bye!");
            pending_disconnect_ = true;
            EnqueueDisconnect();

            if (Configuration::Instance()->GetUseScriptServer())
            {
               std::shared_ptr<ScriptObjectContainer> pContainer = std::shared_ptr<ScriptObjectContainer>(new ScriptObjectContainer);
               std::shared_ptr<ClientInfo> pClientInfo = std::shared_ptr<ClientInfo>(new ClientInfo);

               pClientInfo->SetUsername(username_);
               pClientInfo->SetIPAddress(GetIPAddressString());
               pClientInfo->SetPort(GetLocalEndpointPort());
               pClientInfo->SetSessionID(GetSessionID());
               pClientInfo->SetHELO(helo_host_);
               pClientInfo->SetIsAuthenticated(isAuthenticated_);
               pClientInfo->SetIsEncryptedConnection(IsSSLConnection());
               if (IsSSLConnection())
               {
                  auto cipher_info = GetCipherInfo();
                  pClientInfo->SetCipherVersion(cipher_info.GetVersion().c_str());
                  pClientInfo->SetCipherName(cipher_info.GetName().c_str());
                  pClientInfo->SetCipherBits(cipher_info.GetBits());
               }

               pContainer->AddObject("HMAILSERVER_MESSAGE", current_message_, ScriptObject::OTMessage);
               pContainer->AddObject("HMAILSERVER_CLIENT", pClientInfo, ScriptObject::OTClient);

               String sEventCaller = "OnTooManyInvalidCommands(HMAILSERVER_CLIENT, HMAILSERVER_MESSAGE)";
               ScriptServer::Instance()->FireEvent(ScriptServer::EventOnTooManyInvalidCommands, sEventCaller, pContainer);
            }

            return;
         }
         else
         {
            if (!sResponse.compare(CONST_UNKNOWN_USER))
            {
               if (Configuration::Instance()->GetUseScriptServer())
               {
                  std::shared_ptr<ScriptObjectContainer> pContainer = std::shared_ptr<ScriptObjectContainer>(new ScriptObjectContainer);
                  std::shared_ptr<ClientInfo> pClientInfo = std::shared_ptr<ClientInfo>(new ClientInfo);

                  pClientInfo->SetUsername(username_);
                  pClientInfo->SetIPAddress(GetIPAddressString());
                  pClientInfo->SetPort(GetLocalEndpointPort());
                  pClientInfo->SetSessionID(GetSessionID());
                  pClientInfo->SetHELO(helo_host_);
                  pClientInfo->SetIsAuthenticated(isAuthenticated_);
                  pClientInfo->SetIsEncryptedConnection(IsSSLConnection());
                  if (IsSSLConnection())
                  {
                     auto cipher_info = GetCipherInfo();
                     pClientInfo->SetCipherVersion(cipher_info.GetVersion().c_str());
                     pClientInfo->SetCipherName(cipher_info.GetName().c_str());
                     pClientInfo->SetCipherBits(cipher_info.GetBits());
                  }

                  pContainer->AddObject("HMAILSERVER_MESSAGE", current_message_, ScriptObject::OTMessage);
                  pContainer->AddObject("HMAILSERVER_CLIENT", pClientInfo, ScriptObject::OTClient);

                  String sEventCaller = "OnRecipientUnknown(HMAILSERVER_CLIENT, HMAILSERVER_MESSAGE)";
                  ScriptServer::Instance()->FireEvent(ScriptServer::EventOnRecipientUnknown, sEventCaller, pContainer);
               }
            }
         }
      }

      String sData;
      sData.Format(_T("%d %s"), iErrorCode, sResponse.c_str());
      
      EnqueueWrite_(sData);
     
   }

   bool
   SMTPConnection::DoPreAcceptSpamProtection_()
   {
      if (rejected_by_delayed_grey_listing_)
      {
         SendErrorResponse_(450, "Please try again later.");
         // Don't log to awstats here, since we tell the client to try again later.
         return false;
      }

      // Check if we should do pre-transmissions tests after transmission. This
      // happens if the message is delivered from a forwarding relay server.
      if (type_ == SPPostTransmission)
      {
         // Do all spam proteciton now. It has been delayed since we trust the 
         // server which has forwarded to us.
         // Retrieve the IP address from the message headers.
         IPAddress iIPAddress;
         String hostName;
         
         MessageUtilities::RetrieveOriginatingAddress(current_message_, hostName, iIPAddress);
      
         // Do spam protection now using the IP address in the header.
         if (!DoSpamProtection_(SPPreTransmission, current_message_->GetFromAddress(), hostName, iIPAddress))
         {
            // We should stop the message delivery.
            return false;
         }

         if (!DoSpamProtection_(SPPostTransmission, current_message_->GetFromAddress(), hostName, iIPAddress))
         {
            // We should stop the message delivery.
            return false;
         }
         
      }
      else
      {
         // Do normal post transmission spam protection. (typically SURBL)
         if (!DoSpamProtection_(SPPostTransmission, current_message_->GetFromAddress(), helo_host_, GetRemoteEndpointAddress()))
         {
            // We should stop message delivery
            return false;
         }
      }

      // The message should be delivered.
      return true;
   }


   bool 
   SMTPConnection::GetDoSpamProtection_()
   {
      if (isAuthenticated_)
         return false;

      if (!GetSecurityRange()->GetSpamProtection())
         return false;

      if (current_message_)
      {
         if (SpamProtection::IsWhiteListed(current_message_->GetFromAddress(), GetRemoteEndpointAddress()))
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
   SMTPConnection::GetIsLocalSender_()
   {
       if (sender_domain_ && sender_domain_->GetIsActive())
          return true;

       const String senderAddress = current_message_->GetFromAddress();

       String senderDomainName = StringParser::ExtractDomain(senderAddress);
       std::shared_ptr<Route> route = Configuration::Instance()->GetSMTPConfiguration()->GetRoutes()->GetItemByNameWithWildcardMatch(senderDomainName);

       if (route)
       {
          if (route->ToAllAddresses() || route->GetAddresses()->GetItemByName(senderAddress))
          {
             if (route->GetTreatSenderAsLocalDomain())
                return true;
          }
       }       

       // Does not match a local domain or route.
       return false;
   }

   bool
   SMTPConnection::ParseAddressWithExtensions_(String mailFrom, String &address, String &parameters)
   {
      // Variants:
      // (empty)
      // example
      // example@example.com
      // <example@example.com>
      // <example> param1=value1 param2=value2
      // example param1=value1 param2=value2
      // example@example.com param1=value1 param2=value2
      // <example@example.com> param1=value1 param2=value2
      // "a b"@example.com> param1=value1 param2=value2
      
      // Parameters always comes after the first space, except for when the mailbox part is quoted,
      // in which case it's after the first space after the last quote.

      int parameterStartPosition = 0;

      int firstQuotePosition = mailFrom.Find(_T("\""));
      if (firstQuotePosition >= 0)
      {
         int lastQuotePosition = mailFrom.ReverseFind(_T("\""));

         if (firstQuotePosition == lastQuotePosition)
            return false;

         parameterStartPosition = mailFrom.Find(_T(" "), lastQuotePosition);
      }
      else
      {
         parameterStartPosition = mailFrom.Find(_T(" "));
      }

      int emailAddressEndPosition = 0;

      if (parameterStartPosition >= 0)
      {
         emailAddressEndPosition = parameterStartPosition;
      }
      else
      {
         emailAddressEndPosition = mailFrom.GetLength();
      }

      address = mailFrom.Left(emailAddressEndPosition);

      parameters = parameterStartPosition > 0 ? mailFrom.Mid(parameterStartPosition) : _T("");

      if (address.StartsWith(_T("<")))
      {
         if (!address.EndsWith(_T(">")))
            return false;

         address.TrimLeft('<');
         address.TrimRight('>');

         address.TrimLeft();
         address.TrimRight();
      }

      parameters.TrimLeft();
      parameters.TrimRight();

      return true;
   }


   bool
   SMTPConnection::GetAuthIsEnabled_()
   {
      const auto authDisabledOnPorts = IniFileSettings::Instance()->GetAuthDisabledOnPorts();
      return authDisabledOnPorts.find(GetLocalEndpointPort()) == authDisabledOnPorts.end();
   }

   void 
   SMTPConnection::ReportUnsupportedEsmtpExtension_(const String& parameter)
   {
      SendErrorResponse_(550, Formatter::Format("Unsupported ESMTP extension: {0}", parameter));

   }
}
