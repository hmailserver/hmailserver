// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "ServerTargetResolver.h"

#include "SMTPConfiguration.h"

#include "../Common/BO/Message.h"
#include "../Common/BO/Route.h"
#include "../Common/BO/Routes.h"
#include "../Common/BO/MessageRecipient.h"
#include "../Common/BO/MessageRecipients.h"
#include "../Common/Util/ServerInfo.h"

#include "RuleResult.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   ServerTargetResolver::ServerTargetResolver(std::shared_ptr<Message> message, const RuleResult& globalRuleResult) :
      _globalRuleResult(globalRuleResult),
      message_(message)
   {

   }

   ServerTargetResolver::~ServerTargetResolver(void)
   {
   }

   std::map<std::shared_ptr<ServerInfo>, std::vector<std::shared_ptr<MessageRecipient> > >
   ServerTargetResolver::Resolve() 
   {  
      std::map<std::shared_ptr<ServerInfo>, std::vector<std::shared_ptr<MessageRecipient> > > serverInfos;

      // first check if all recipients should be delivered via a specific route. 
      // if this is the case there's no point in doing any further resolving.
      __int64 iFixedRouteID = _globalRuleResult.GetSendUsingRoute();
      if (iFixedRouteID > 0)
      {
         std::shared_ptr<Route> pRoute = HM::Configuration::Instance()->GetSMTPConfiguration()->GetRoutes()->GetItemByDBID(iFixedRouteID);
         if (pRoute)
         {
            String domainName = pRoute->DomainName();
            std::shared_ptr<ServerInfo> serverInfo = GetFixedSMTPHostForDomain_(domainName);

            if (serverInfo)
            {
               // All recipients should go into the same SMTP server
               std::vector<std::shared_ptr<MessageRecipient> > recipients;
               for(std::shared_ptr<MessageRecipient> recipient : message_->GetRecipients()->GetVector())
               {
                  recipients.push_back(recipient); 
               }

               serverInfos.insert(std::make_pair(serverInfo, recipients));
               return serverInfos;
            }
         }
      }

      // sort all recipients per domain, domain in lower case. this is done
      // so that we only need to look for routes for every domain once.
      std::map<String, std::vector<std::shared_ptr<MessageRecipient> > > sortedRecipients;
      for(std::shared_ptr<MessageRecipient> recipient : message_->GetRecipients()->GetVector())
      {
         String domainName = StringParser::ExtractDomain(recipient->GetAddress()).ToLower();

         if (sortedRecipients.find(domainName) == sortedRecipients.end())
         {
            std::vector<std::shared_ptr<MessageRecipient> > recipientsOnDomain;
            recipientsOnDomain.push_back(recipient);

            sortedRecipients[domainName] = recipientsOnDomain;
         }
         else
         {
            sortedRecipients[domainName].push_back(recipient);
         }
      }      

      // For every domain, determine where to deliver the message for the recipients.
      auto iter = sortedRecipients.begin();
      auto iterEnd = sortedRecipients.end();

      std::map<String, std::shared_ptr<ServerInfo>> domainServerInfoMap;

      for (; iter != iterEnd; iter++)
      {
         String domainName = (*iter).first;
         domainName.ToLower();

         std::vector<std::shared_ptr<MessageRecipient> > vecRecipients = (*iter).second;

         std::shared_ptr<ServerInfo> serverInfo = GetFixedSMTPHostForDomain_(domainName);

         if (!serverInfo)
         {
            std::shared_ptr<SMTPConfiguration> pSMTPConfig = Configuration::Instance()->GetSMTPConfiguration();
            serverInfo = std::shared_ptr<ServerInfo>(new ServerInfo(false, domainName, "", 25, "", "", pSMTPConfig->GetSMTPConnectionSecurity()));
         }

         serverInfos.insert(std::make_pair(serverInfo, vecRecipients));
      }

      
      std::map<std::shared_ptr<ServerInfo>, std::vector<std::shared_ptr<MessageRecipient> > > result = CreateDistinctMap(serverInfos);

      return result;
   }

   std::map<std::shared_ptr<ServerInfo>, std::vector<std::shared_ptr<MessageRecipient> > >
   ServerTargetResolver::CreateDistinctMap(std::map<std::shared_ptr<ServerInfo>, std::vector<std::shared_ptr<MessageRecipient> > > serverInfos)
   {
      // Try to merge recipient lists for different serverinfo's. If we have two server info's with the same target
      // host / port / credentials, we should merge the recipient lists. This may be the same for example if you are
      // using a SMTP relayer. The email message may contain recipients for 4 different domains, but we only want to
      // open one connection to the SMTP relay server.
      auto iterServerInfo = serverInfos.begin();
      auto iterServerInfoEnd = serverInfos.end();

      std::map<std::shared_ptr<ServerInfo>, std::vector<std::shared_ptr<MessageRecipient> > > result;

      for (; iterServerInfo != iterServerInfoEnd; iterServerInfo++)
      {
         auto iterResultInfos = result.begin();
         auto iterResultInfosEnd = result.end();

         bool foundExisting = false;

         for (; iterResultInfos != iterResultInfosEnd; iterResultInfos++)
         {
            ServerInfo& newServerInfo = *(*iterServerInfo).first.get();
            ServerInfo& resultServerInfo = *(*iterResultInfos).first.get();

            if (newServerInfo == resultServerInfo)
            {
               // Add all recipients on this server info to the existing server info.
               std::vector<std::shared_ptr<MessageRecipient> >& vecRecipients = (*iterServerInfo).second;

               auto iterRecipient = vecRecipients.begin();
               auto iterRecipientEnd = vecRecipients.end();

               // Copy all recipients to this server info
               for (; iterRecipient != iterRecipientEnd; iterRecipient++)
                  (*iterResultInfos).second.push_back((*iterRecipient));

               foundExisting = true;
               break;
            }

         }

         if (!foundExisting)
         {
            result.insert(*iterServerInfo);
         }

      }
      
      return result;
   }

   std::shared_ptr<ServerInfo>
   ServerTargetResolver::GetFixedSMTPHostForDomain_(const String &sDomain)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Check if there exists a fixed SMTP host for the domain given, and in that
   // case it returns it. May be used for example for routes or when a SMTP
   // relayer is used.
   //---------------------------------------------------------------------------()
   {
      String sSMTPHost;
      long lPort = 0;
      String sUsername;
      String sPassword;
      ConnectionSecurity connection_security = CSNone;

      std::shared_ptr<SMTPConfiguration> pSMTPConfig = Configuration::Instance()->GetSMTPConfiguration();

      // Check if we have any route for this domain.
      std::shared_ptr<Route> pRoute = pSMTPConfig->GetRoutes()->GetItemByNameWithWildcardMatch(sDomain);

      if (pRoute)
      {
         sSMTPHost = pRoute->TargetSMTPHost();
         lPort = pRoute->TargetSMTPPort();
         connection_security = pRoute->GetConnectionSecurity();

         if (pRoute->GetRelayerRequiresAuth())
         {
            sUsername = pRoute->GetRelayerAuthUsername();
            sPassword = pRoute->GetRelayerAuthPassword();
         }
      }
      else
      {
         // Do we have a fixed SMTP relayer?
         String sRelayer = pSMTPConfig->GetSMTPRelayer();

         if (!sRelayer.IsEmpty())
         {
            sSMTPHost = sRelayer;
            lPort = pSMTPConfig->GetSMTPRelayerPort();
            if (lPort == 0)
               lPort = 25;

            if (pSMTPConfig->GetSMTPRelayerRequiresAuthentication())
            {
               sUsername = pSMTPConfig->GetSMTPRelayerUsername();
               sPassword = pSMTPConfig->GetSMTPRelayerPassword();
            }

            connection_security = pSMTPConfig->GetSMTPRelayerConnectionSecurity();
         }
      }

      if (sSMTPHost.IsEmpty())
      {
         return std::shared_ptr<ServerInfo>();
      }

      bool is_ipaddress = StringParser::IsValidIPAddress(sSMTPHost);

      String host_name = is_ipaddress ? "" : sSMTPHost;
      String ip_address = is_ipaddress ? sSMTPHost : "";

      std::shared_ptr<ServerInfo> serverInfo(new ServerInfo(true, host_name, ip_address, lPort, sUsername, sPassword, connection_security));
      return serverInfo;

   }

}