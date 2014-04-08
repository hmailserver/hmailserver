// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "./ExternalDelivery.h"

#include "RuleResult.h"

#include "../Common/AntiSpam/DKIM/DKIMSigner.h"

#include "../Common/BO/Message.h"
#include "../common/BO/MessageRecipient.h"
#include "../common/BO/MessageRecipients.h"
#include "../common/BO/Routes.h"

#include "../common/EventHandlers/Events.h"

#include "../common/Persistence/PersistentMessageRecipient.h"
#include "../common/Persistence/PersistentMessage.h"

#include "../common/TCPIP/DNSResolver.h"
#include "../common/TCPIP/IOCPServer.h"

#include "../Common/Util/AWstats.h"
#include "../common/Util/ServerInfo.h"

#include "ServerTargetResolver.h"
#include "SMTPConfiguration.h"
#include "SMTPClientConnection.h"


#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{ 
   ExternalDelivery::ExternalDelivery(const String &sSendersIP, shared_ptr<Message> message, const RuleResult &globalRuleResult) :
      _sendersIP(sSendersIP),
      _originalMessage(message),
      _globalRuleResult(globalRuleResult)
{
   }

   ExternalDelivery::~ExternalDelivery(void)
   {

   }

   /// Performs deliver to any external recipients. 
   /// Returns true if the message has been rescheduled for later delivery.
   bool
   ExternalDelivery::Perform(vector<String> &saErrorMessages)
   {
      // DKIM-tag the message.
      if (_originalMessage->GetNoOfRetries() == 0)
      {
         DKIMSigner signer;
         signer.Sign(_originalMessage);
      }

      map<String,String> mapFailedDueToNonFatalError;

      ServerTargetResolver serverTargetResolver(_originalMessage, _globalRuleResult);
      map<shared_ptr<ServerInfo>, std::vector<shared_ptr<MessageRecipient> > > mapRecipients = serverTargetResolver.Resolve();
      map<shared_ptr<ServerInfo>, std::vector<shared_ptr<MessageRecipient> > >::iterator iterDomain = mapRecipients.begin();
      map<shared_ptr<ServerInfo>, std::vector<shared_ptr<MessageRecipient> > >::iterator iterEnd = mapRecipients.end();

      unsigned int iMaxRecipientsInBatch = Configuration::Instance()->GetSMTPConfiguration()->GetMaxSMTPRecipientsInBatch();
      if (iMaxRecipientsInBatch == 0)
         iMaxRecipientsInBatch = UINT_MAX;

      for (; iterDomain != iterEnd; iterDomain++)
      {
         shared_ptr<ServerInfo> serverInfo = (*iterDomain).first;
         vector<shared_ptr<MessageRecipient> > vecRecipientsOnDomain = (*iterDomain).second;

         // Split up all the recipients for this server into batches of 100 or so.
         vector<shared_ptr<MessageRecipient> > batch;
         vector<shared_ptr<MessageRecipient> >::iterator iterRecipient = vecRecipientsOnDomain.begin();
         while (iterRecipient != vecRecipientsOnDomain.end())
         {
            batch.push_back(*iterRecipient);

            if (batch.size() >= iMaxRecipientsInBatch ||
               iterRecipient + 1 == vecRecipientsOnDomain.end())
            {
               // Deliver the message to the remote server.
               _DeliverToExternalDomain(batch, serverInfo);

               // Check what status we got on the external deliveries.
               _CollectDeliveryResult(serverInfo->GetHostName(), batch, saErrorMessages, mapFailedDueToNonFatalError);    

               batch.clear();
            }

            iterRecipient++;
         }

      }

      if (mapFailedDueToNonFatalError.size() > 0)
      {   
         bool messageRescheduled = _RescheduleDelivery(mapFailedDueToNonFatalError, saErrorMessages);
         return messageRescheduled;
      }
      else
         return false;
   }

   void
   ExternalDelivery::_DeliverToExternalDomain(vector<shared_ptr<MessageRecipient> > &vecRecipients, shared_ptr<ServerInfo> serverInfo)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Deliveres the message to external accounts (recipients not on this server).
   //---------------------------------------------------------------------------()
   {
      LOG_DEBUG("Performing external delivery");

      String sFirstRecipientAddress = vecRecipients[0]->GetAddress();
      if (sFirstRecipientAddress.IsEmpty())
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4226, "SMTPDeliverer::_DeliverToExternalAccounts", "Could not deliver message; Recipient address missing.");
         return;
      }

      vector<String> saMailServers;

      // Run DNS query to find the recipient servers IP addresses.
      if (!_ResolveRecipientServer(serverInfo, vecRecipients, saMailServers))
         return;

      m_iMXTriesFactor = IniFileSettings::Instance()->GetMXTriesFactor();

      // Try to connect to one server at a time. If a fatal error
      // occurs, (an exception with eFatalError), we should stop trying
      // and just return an error message.

      for (unsigned int i = 0; i < saMailServers.size(); i++)
      {
         String sHostName = saMailServers[i];
         serverInfo->SetHostName(sHostName);

         // Create a list of the remaining recipients. These are the recipients we have
         // not yet delivered to on a previous server (where i > 0). 
         vector<shared_ptr<MessageRecipient> > remainingRecipients;
         boost_foreach(shared_ptr<MessageRecipient> recipient, vecRecipients)
         {
            if (recipient->GetDeliveryResult() == MessageRecipient::ResultUndefined ||
               recipient->GetDeliveryResult() == MessageRecipient::ResultNonFatalError)
            {
               remainingRecipients.push_back(recipient);
            }
         }

         _InitiateExternalConnection(remainingRecipients, serverInfo);

         bool bTryNextServer = _RecipientWithNonFatalDeliveryErrorExists(vecRecipients);

         if (!bTryNextServer)
         {
            // All deliveries are complete or fatal. 
            LOG_DEBUG("SD::~_DeliverToExternalAccounts-1");
            return;
         }

         // Let's limit # of servers tried per retry to m_iMXTriesFactor * current number of retries to free up queue
         int iMXServerLimit = (_originalMessage->GetNoOfRetries()+1) * m_iMXTriesFactor;
         if (m_iMXTriesFactor > 0 && i + 1 >= (unsigned int) iMXServerLimit )
         {
            LOG_APPLICATION("SMTPDeliverer - Message " + StringParser::IntToString(_originalMessage->GetID()) + ": Limiting to MXTriesFactored value of " + StringParser::IntToString(iMXServerLimit) + ".");      
            break;
         }
      }

      LOG_DEBUG("SD::~_DeliverToExternalAccounts-2");


   }

   /// Resolves IP addresses for the recipient servers. This will either be a MX 
   /// lookup, or a A lookup, if SMTP relaying is used.
   bool 
   ExternalDelivery::_ResolveRecipientServer(shared_ptr<ServerInfo> &serverInfo, vector<shared_ptr<MessageRecipient> > &vecRecipients, vector<String> &saMailServers)
   {
      DNSResolver resolver;

      // Resolve the specified hosts.
      bool dnsQueryOK = false;

      if (serverInfo->GetFixed())
      {
         String relayServer = serverInfo->GetHostName();

         LOG_APPLICATION("SMTPDeliverer - Message " + StringParser::IntToString(_originalMessage->GetID()) + ": Relaying to host " + relayServer + ".");      

         vector<String> mailServerHosts;
         if (relayServer.Find(_T("|")) > 0)
            mailServerHosts = StringParser::SplitString(relayServer, "|");
         else
            mailServerHosts.push_back(relayServer);

         boost_foreach(String host, mailServerHosts)
         {
            // The actual resolution of these host names to IP addresses,
            // are taken care of by TCPConnection::Connect.
            saMailServers.push_back(host);
         }
      }
      else
      {
         // Resolve the mail server. The TCPConnection::Connect will normally do name
         // resolution, but since this is a matter of MX resolution and comparing
         // MX record preference, we have to do it manually.
         dnsQueryOK = resolver.GetEmailServers(serverInfo->GetHostName(), saMailServers);

         serverInfo = shared_ptr<ServerInfo>(new ServerInfo(false, "", 25, "", "", false));
      }

      shared_ptr<SMTPConfiguration> pSMTPConfig = Configuration::Instance()->GetSMTPConfiguration();
      const unsigned int maxNumberOfMXHosts = pSMTPConfig->GetMaxNumberOfMXHosts();

      if (maxNumberOfMXHosts > 0 && saMailServers.size() > maxNumberOfMXHosts)
      {
         LOG_DEBUG("Maximum number of MX host reached. Truncating MX server list.");
         saMailServers.resize(maxNumberOfMXHosts);
      }

      // Check if any servers exists.
      if (saMailServers.size() == 0)
      {
         _HandleNoRecipientServers(vecRecipients, dnsQueryOK, serverInfo->GetFixed());
         return false;
      }

      return true;
   }

   bool
   ExternalDelivery::_RecipientWithNonFatalDeliveryErrorExists(vector<shared_ptr<MessageRecipient> > &vecRecipients)
   {
      // If there exists an recipient with nonfatal error,
      // we should try to deliver to other servers.
      vector<shared_ptr<MessageRecipient> >::iterator iterRecipient = vecRecipients.begin();
      bool bTryNextServer = false;
      while (iterRecipient != vecRecipients.end())
      {
         shared_ptr<MessageRecipient> pRecipient (*iterRecipient);

         if (pRecipient->GetDeliveryResult() == MessageRecipient::ResultUndefined ||
            pRecipient->GetDeliveryResult() == MessageRecipient::ResultNonFatalError)
         {
            return true;
            break;
         }

         iterRecipient++;
      }

      return false;
   }

   void 
   ExternalDelivery::_HandleExternalDeliveryFailure(vector<shared_ptr<MessageRecipient> > &vecRecipients,    
                                                      bool bIsFatal,    
                                                      String &sErrorString)
   {


      vector<shared_ptr<MessageRecipient> >::iterator iterRecipient = vecRecipients.begin();
      while (iterRecipient != vecRecipients.end())
      {
         shared_ptr<MessageRecipient> pRecipient = (*iterRecipient);

         // Unless this recipient has already fatally failed, or succeeded,
         // update the state of it.

         bool bDeliveryComplete = pRecipient->GetDeliveryResult() == MessageRecipient::ResultOK ||
            pRecipient->GetDeliveryResult() == MessageRecipient::ResultFatalError;
         if (!bDeliveryComplete)
         {
            if (bIsFatal)
               pRecipient->SetDeliveryResult(MessageRecipient::ResultFatalError);
            else
               pRecipient->SetDeliveryResult(MessageRecipient::ResultNonFatalError);

            pRecipient->SetErrorMessage(sErrorString);
         }

         iterRecipient++;
      } 
   }

   void
   ExternalDelivery::_HandleNoRecipientServers(vector<shared_ptr<MessageRecipient> > &vecRecipients, bool bDNSQueryOK, bool isSpecificRelayServer)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Takes care of the situation when no valid recipient server addresses exist.
   //---------------------------------------------------------------------------()
   {
      LOG_APPLICATION("SMTPDeliverer - Message " + StringParser::IntToString(_originalMessage->GetID()) + ": No mail servers could be found for the address " + (*vecRecipients.begin())->GetAddress() + ".");

      String bounceMessageText;

      // Generate a string which will be included in the bounce message.

      if (bDNSQueryOK)
      {
         if (isSpecificRelayServer)
            bounceMessageText = _T("   Error Type: SMTP\r\n   Error Description: The host specified as SMTP relay server could not be found. Please contact your server administrator.\r\n\r\n");
         else
            bounceMessageText = _T("   Error Type: SMTP\r\n   Error Description: No mail servers appear to exists for the recipient's address.\r\n   Additional information: Please check that you have not misspelled the recipient's email address.\r\n\r\n");
      }
      else
      {
         bounceMessageText = _T("   Error Type: SMTP\r\n   Error Description: Unable to find the recipient's email server. The DNS query has failed.\r\n\r\n");
      }

      // Update the recipients with the bounce message text and delivery result.
      boost_foreach(shared_ptr<MessageRecipient> recipient, vecRecipients)
      {
         // Temp change to force non fatal no matter DNS result
         // Messages bouncing immediately due to no mail servers due to DNS issue
         recipient->SetDeliveryResult(MessageRecipient::ResultNonFatalError);
         // recipient->SetDeliveryResult(bDNSQueryOK ? MessageRecipient::ResultFatalError : MessageRecipient::ResultNonFatalError);
         recipient->SetErrorMessage(bounceMessageText);
      }  
   }

   void
   ExternalDelivery::_InitiateExternalConnection(vector<shared_ptr<MessageRecipient> > &vecRecipients,
                                                 shared_ptr<ServerInfo> serverInfo)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Connects to a remote server and delivers the message to it.
   //---------------------------------------------------------------------------()
   {
      LOG_DEBUG("SD::_InitiateExternalConnection");

      shared_ptr<SMTPClientConnection> pSMTPProtocolParser = shared_ptr<SMTPClientConnection>(new SMTPClientConnection());
      pSMTPProtocolParser->SetDelivery(_originalMessage, vecRecipients);

      shared_ptr<IOCPServer> pIOCPServer = Application::Instance()->GetIOCPServer();

      boost::asio::ssl::context ctx(pIOCPServer->GetIOService(), boost::asio::ssl::context::sslv23);

      shared_ptr<TCPConnection> pClientConnection;

      if (serverInfo->GetUseSSL())
         pClientConnection = pIOCPServer->CreateConnection(ctx);
      else
         pClientConnection = pIOCPServer->CreateConnection();

      pClientConnection->Start(pSMTPProtocolParser);

      if (!serverInfo->GetUsername().IsEmpty())
         pSMTPProtocolParser->SetAuthInfo(serverInfo->GetUsername(), serverInfo->GetPassword());


      // Determine what local IP dadress to use.
      IPAddress localAddress = _GetLocalAddress();
      
      Event disconnectEvent(pClientConnection->GetConnectionTerminationEvent());

      if (pClientConnection->Connect(serverInfo->GetHostName(), serverInfo->GetPort(), localAddress))
      {
         // Make sure we keep no references to the TCP connection so that it
         // can be terminated whenever. We're longer own the connection.
         pClientConnection.reset();

         disconnectEvent.Wait();
      }

      LOG_DEBUG("SD::~_InitiateExternalConnection-5");

   }

   IPAddress 
   ExternalDelivery::_GetLocalAddress()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Determines what local IP address to use when delivering to this host.
   //---------------------------------------------------------------------------()
   {
      IPAddress localAddress;

      shared_ptr<SMTPConfiguration> pSMTPConfig = Configuration::Instance()->GetSMTPConfiguration();

      String smtpSettingBindToIP = pSMTPConfig->GetSMTPDeliveryBindToIP();
      String ruleBindToAddress = _globalRuleResult.GetBindToAddress();

      if (!ruleBindToAddress.IsEmpty())
         localAddress.TryParse(ruleBindToAddress);
      else if (!smtpSettingBindToIP.IsEmpty())
         localAddress.TryParse(smtpSettingBindToIP);

      return localAddress;

   }

   void 
   ExternalDelivery::_CollectDeliveryResult(const String &serverHostName, 
                                             vector<shared_ptr<MessageRecipient> > &vecRecipients, 
                                             vector<String> &saErrorMessages,
                                             map<String,String> &mapFailedDueToNonFatalError)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // This function is called after delivery has ended. It goes through the recipients
   // and collects information on how the delivery went (good or bad). If delivery went
   // good, the recipient is deleted from the database.
   //---------------------------------------------------------------------------()
   {
      LOG_DEBUG("Collect delivery result");

      // Check how the delivery went.
      boost_foreach(shared_ptr<MessageRecipient> recipient, vecRecipients)
      {
         if (recipient->GetDeliveryResult() == MessageRecipient::ResultOK)
         {
            AWStats::LogDeliverySuccess(_sendersIP, serverHostName, _originalMessage, recipient->GetAddress());

            // Delete this recipient from the database.
            PersistentMessageRecipient::DeleteObject(recipient);
         }
         else if (recipient->GetDeliveryResult() == MessageRecipient::ResultNonFatalError)
         {
            mapFailedDueToNonFatalError[recipient->GetAddress()] = recipient->GetErrorMessage();
         }
         else if (recipient->GetDeliveryResult() == MessageRecipient::ResultFatalError)
         {
            // Yes, this is a permanent error.
            String sSingleErrorMsg;
            String sRecipient = recipient->GetAddress();
            sSingleErrorMsg = sRecipient + "\r\n";
            sSingleErrorMsg = sSingleErrorMsg + recipient->GetErrorMessage();
            sSingleErrorMsg = sSingleErrorMsg + "\r\n";

            saErrorMessages.push_back(sSingleErrorMsg);  

            // Delete this recipient from the database.
            PersistentMessageRecipient::DeleteObject(recipient);

            AWStats::LogDeliveryFailure(_sendersIP, _originalMessage->GetFromAddress(), recipient->GetAddress(),  550);
            Events::FireOnDeliveryFailed(_originalMessage, _sendersIP, recipient->GetAddress(), recipient->GetErrorMessage());
         }
         else
         {
            mapFailedDueToNonFatalError[recipient->GetAddress()] = "Remote server closed connection.";
         }

      }  

      LOG_DEBUG("Collect delivery result - Done");
   }

   /// Checks if we should reschedule the message for later delivery. If so, we do.
   /// Returns true if the message is rescheduled.
   bool
   ExternalDelivery::_RescheduleDelivery(map<String,String> &mapFailedDueToNonFatalError, vector<String> &saErrorMessages)
   {

      LOG_DEBUG("SD::_RescheduleDelivery");

      // We have failed recipients. Iterate over one of them at a time
      long iMaxNoOfRetries = 0;
      long lMinutesBewteen = 0;
      int iCurNoOfRetries = _originalMessage->GetNoOfRetries() ;

      m_iQuickRetries = IniFileSettings::Instance()->GetQuickRetries();
      m_iQuickRetriesMinutes = IniFileSettings::Instance()->GetQuickRetriesMinutes();
      m_iQueueRandomnessMinutes = IniFileSettings::Instance()->GetQueueRandomnessMinutes();

      // Variables used to generate randomness value for retry delay
      errno_t rnd_err;
      unsigned int tmp_rnd = 0;
      int iRandomAdjust = 0;

      // See if randomness is enabled to work around Win2k compatability issue
      // plus saves work if not enabled which is default
      if (m_iQueueRandomnessMinutes > 0)
      {

         // Get our random #
         // LOG_DEBUG("Windows 2000 does not support rand_s & pukes here");
         rnd_err = (rand_s(&tmp_rnd));

         // If no error getting random # use it
         if (rnd_err == 0)
            iRandomAdjust = (unsigned int) ((double)tmp_rnd / (double) UINT_MAX * m_iQueueRandomnessMinutes) + 1;
      }

      LOG_DEBUG("Retrieving retry options.");
      if (_GetRetryOptions(mapFailedDueToNonFatalError, iMaxNoOfRetries, lMinutesBewteen))
      {
         // so return now since no need for retry at this time

         // For now we unlock message here but might be best to do @ ETRN time..
         PersistentMessage::UnlockObject(_originalMessage);

         LOG_APPLICATION("SMTPDeliverer - Route Message: HOLD for later delivery..");
         return true; // Do not delete e-mail now
      }

      if (iCurNoOfRetries < iMaxNoOfRetries)
      {
         // We should try at least once more - reschedule the message.
         LOG_DEBUG("Starting rescheduling.");

         // First few retries should be quicker for greylisting IF enabled
         if (iCurNoOfRetries < m_iQuickRetries) 
         {
            LOG_APPLICATION("SMTPDeliverer - Message " + StringParser::IntToString(_originalMessage->GetID()) + ": Message could not be delivered. Greylisting? Scheduling it for quick retry " + StringParser::IntToString(iCurNoOfRetries + 1) + " of " + StringParser::IntToString(m_iQuickRetries) + " in " + StringParser::IntToString(m_iQuickRetriesMinutes + iRandomAdjust) + " minutes.");
            PersistentMessage::SetNextTryTime(_originalMessage->GetID(), true, m_iQuickRetriesMinutes + iRandomAdjust);
         
            // Unlock the message now so that a future delivery thread can pick it up.
            PersistentMessage::UnlockObject(_originalMessage);
         
            LOG_DEBUG("Message rescheduled for later quick delivery. (Greylisting?)");
            return true; // Do not delete e-mail now
         }
         else
         {
            LOG_APPLICATION("SMTPDeliverer - Message " + StringParser::IntToString(_originalMessage->GetID()) + ": Message could not be delivered. Scheduling it for later delivery in " + StringParser::IntToString(lMinutesBewteen + iRandomAdjust) + " minutes.");
            PersistentMessage::SetNextTryTime(_originalMessage->GetID(), true, lMinutesBewteen + iRandomAdjust);
         
            // Unlock the message now so that a future delivery thread can pick it up.
            PersistentMessage::UnlockObject(_originalMessage);
         
            LOG_DEBUG("Message rescheduled for later delivery.");
            return true; // Do not delete e-mail now
         }
      }
      else
      {
         LOG_DEBUG("Aborting delivery.");

         // We are finished trying. Let's give up!
         LOG_APPLICATION("SMTPDeliverer - Message " + StringParser::IntToString(_originalMessage->GetID()) + ": Message could not be delivered. Returning error log to sender.");

         // Delivery failed the last time.
         String sErrorMessage;

         map<String,String>::iterator iterFailed = mapFailedDueToNonFatalError.begin();
         while (iterFailed != mapFailedDueToNonFatalError.end())
         {
            if (!sErrorMessage.IsEmpty())
               sErrorMessage += "\r\n";

            String sEmailAddress = (*iterFailed).first;
            String sFailed = (*iterFailed).second;
            sErrorMessage += sEmailAddress + "\r\n" + sFailed;

            // Delivery has failed for the last time.
            AWStats::LogDeliveryFailure(_sendersIP, _originalMessage->GetFromAddress(), sEmailAddress,  550);
            Events::FireOnDeliveryFailed(_originalMessage, _sendersIP, sEmailAddress, sFailed);

            iterFailed++;
         }

         String sMsg;
         sMsg.Format(_T("Tried %d time(s)"), iMaxNoOfRetries+ 1);

         sErrorMessage += "\r\n";
         sErrorMessage += sMsg;
         sErrorMessage += "\r\n\r\n";
         saErrorMessages.push_back(sErrorMessage);

         LOG_DEBUG("Message not rescheduled for later delivery.")
         
        return false;
      }
   }

   /// Returns the retry options for a list of address.
   /// The maximum number of retries and the maximum number of mintues between
   /// every try.
   // Type changed to bool for use in ETRN's
   bool 
   ExternalDelivery::_GetRetryOptions(map<String,String> &mapFailedDueToNonFatalError, long &lNoOfRetries, long &lMinutesBetween)
   {
      shared_ptr<SMTPConfiguration> pSMTPConfig = Configuration::Instance()->GetSMTPConfiguration();
      shared_ptr<Routes> pRoutes = Configuration::Instance()->GetSMTPConfiguration()->GetRoutes();

      bool bFirstMatchingRoute = true;

      // First fetch the default values. Theese are used
      // if we can't find a route for any of the domains.
      lNoOfRetries = pSMTPConfig->GetNoOfRetries();
      lMinutesBetween  = pSMTPConfig->GetMinutesBetweenTry();

      map<String,String>::iterator iterAddress = mapFailedDueToNonFatalError.begin();
      map<String, shared_ptr<Route> > matchingRoutes;

      bool recipientsFoundNotMatchingRoute = false;

      while (iterAddress != mapFailedDueToNonFatalError.end())
      {
         String sAddress = (*iterAddress).first;
         String sDomainName = StringParser::ExtractDomain (sAddress).ToLower();
         
         shared_ptr<Route> pRoute = pRoutes->GetItemByName(sDomainName);

         if (pRoute)
         {
            int lTmpNoOfRetries = pRoute->NumberOfTries() - 1;
            int lTmpMinutesBetween = pRoute->MinutesBetweenTry();

            if (matchingRoutes.size() == 0)
            {
               lNoOfRetries = lTmpNoOfRetries;
               lMinutesBetween = lTmpMinutesBetween;
            }
            else
            {
               if (lTmpNoOfRetries > lNoOfRetries)
                  lNoOfRetries = lTmpNoOfRetries;

               if (lTmpMinutesBetween > lMinutesBetween)
                  lMinutesBetween = lTmpMinutesBetween;
            }

            matchingRoutes[sDomainName] = pRoute;
         }
         else
            recipientsFoundNotMatchingRoute = true;

         iterAddress++;
      }

      // If ONLY 1 route was found & not any non routes say we HOLD message otherwise don't.
      // HOLD when non-route recipient would be BAD. :D
      if (matchingRoutes.size() == 1 && !recipientsFoundNotMatchingRoute)
      {
         shared_ptr<Route> route = (*matchingRoutes.begin()).second;
         String routeDescription = route->GetDescription();

         if (routeDescription.ToUpper().StartsWith(_T("ETRN")))
         {
            __int64 iRouteID = route->GetID();

            // Here we change ID, type to 3 for HOLD. Retries reset to ensure it doesn't
            // bounce yet. NOT 0 though to stop mirror account copy over & over
            SQLCommand command("update hm_messages set messageaccountid = @ROUTEID, messagetype = 3, messagecurnooftries =  1,  messagenexttrytime = '1901-01-01 00:00:01' where messageid = @MESSAGEID");
            
            command.AddParameter("@ROUTEID", iRouteID);
            command.AddParameter("@MESSAGEID", _originalMessage->GetID());

            if (Application::Instance()->GetDBManager()->Execute(command))
            {
               // Execute OK - Should do some error checking & logging here..
            }

            return true;  // Say we HELD message
         }
         
         return false;
      }
      else
         return false;  // Continue as normal, no HOLD         
   }
}