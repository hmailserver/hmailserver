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
#include "../Common/BO/Rule.h"
#include "../Common/Util/ServerInfo.h"

#include "RuleResult.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   ServerTargetResolver::ServerTargetResolver(shared_ptr<Message> message, const RuleResult& globalRuleResult) :
      _globalRuleResult(globalRuleResult),
      _message(message)
   {

   }

   ServerTargetResolver::~ServerTargetResolver(void)
   {
   }

   map<shared_ptr<ServerInfo>, std::vector<shared_ptr<MessageRecipient> > >
   ServerTargetResolver::Resolve() 
   {  
      map<shared_ptr<ServerInfo>, vector<shared_ptr<MessageRecipient> > > serverInfos;

      // first check if all recipients should be delivered via a specific route. 
      // if this is the case there's no point in doing any further resolving.
      __int64 iFixedRouteID = _globalRuleResult.GetSendUsingRoute();
      if (iFixedRouteID > 0)
      {
         shared_ptr<Route> pRoute = HM::Configuration::Instance()->GetSMTPConfiguration()->GetRoutes()->GetItemByDBID(iFixedRouteID);
         if (pRoute)
         {
            String domainName = pRoute->DomainName();
            shared_ptr<ServerInfo> serverInfo = _GetFixedSMTPHostForDomain(domainName);

            if (serverInfo)
            {
               // All recipients should go into the same SMTP server
               std::vector<shared_ptr<MessageRecipient> > recipients;
               boost_foreach(shared_ptr<MessageRecipient> recipient, _message->GetRecipients()->GetVector())
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
      map<String, vector<shared_ptr<MessageRecipient> > > sortedRecipients;
      boost_foreach(shared_ptr<MessageRecipient> recipient, _message->GetRecipients()->GetVector())
      {
         String domainName = StringParser::ExtractDomain(recipient->GetAddress()).ToLower();

         if (sortedRecipients.find(domainName) == sortedRecipients.end())
         {
            vector<shared_ptr<MessageRecipient> > recipientsOnDomain;
            recipientsOnDomain.push_back(recipient);

            sortedRecipients[domainName] = recipientsOnDomain;
         }
         else
         {
            sortedRecipients[domainName].push_back(recipient);
         }
      }      

      // For every domain, determine where to deliver the message for the recipients.
      map<String, vector<shared_ptr<MessageRecipient> > >::iterator iter = sortedRecipients.begin();
      map<String, vector<shared_ptr<MessageRecipient> > >::iterator iterEnd = sortedRecipients.end();

      map<String, shared_ptr<ServerInfo>> domainServerInfoMap;

      for (; iter != iterEnd; iter++)
      {
         String domainName = (*iter).first;
         domainName.ToLower();

         vector<shared_ptr<MessageRecipient> > vecRecipients = (*iter).second;

         shared_ptr<ServerInfo> serverInfo = _GetFixedSMTPHostForDomain(domainName);

         if (!serverInfo)
            serverInfo = shared_ptr<ServerInfo>(new ServerInfo(false, domainName, 0, "", "", false));

         serverInfos.insert(std::make_pair(serverInfo, vecRecipients));
      }

      
      map<shared_ptr<ServerInfo>, vector<shared_ptr<MessageRecipient> > > result = CreateDistinctMap(serverInfos);

      return result;
   }

   map<shared_ptr<ServerInfo>, std::vector<shared_ptr<MessageRecipient> > >
   ServerTargetResolver::CreateDistinctMap(map<shared_ptr<ServerInfo>, std::vector<shared_ptr<MessageRecipient> > > serverInfos)
   {
      // Try to merge recipient lists for different serverinfo's. If we have two server info's with the same target
      // host / port / credentials, we should merge the recipient lists. This may be the same for example if you are
      // using a SMTP relayer. The email message may contain recipients for 4 different domains, but we only want to
      // open one connection to the SMTP relay server.
      map<shared_ptr<ServerInfo>, std::vector<shared_ptr<MessageRecipient> > >::iterator iterServerInfo = serverInfos.begin();
      map<shared_ptr<ServerInfo>, std::vector<shared_ptr<MessageRecipient> > >::iterator iterServerInfoEnd = serverInfos.end();

      map<shared_ptr<ServerInfo>, vector<shared_ptr<MessageRecipient> > > result;

      for (; iterServerInfo != iterServerInfoEnd; iterServerInfo++)
      {
         map<shared_ptr<ServerInfo>, std::vector<shared_ptr<MessageRecipient> > >::iterator iterResultInfos = result.begin();
         map<shared_ptr<ServerInfo>, std::vector<shared_ptr<MessageRecipient> > >::iterator iterResultInfosEnd = result.end();

         bool foundExisting = false;

         for (; iterResultInfos != iterResultInfosEnd; iterResultInfos++)
         {
            ServerInfo& newServerInfo = *(*iterServerInfo).first.get();
            ServerInfo& resultServerInfo = *(*iterResultInfos).first.get();

            if (newServerInfo == resultServerInfo)
            {
               // Add all recipients on this server info to the existing server info.
               vector<shared_ptr<MessageRecipient> >& vecRecipients = (*iterServerInfo).second;

               vector<shared_ptr<MessageRecipient> >::iterator iterRecipient = vecRecipients.begin();
               vector<shared_ptr<MessageRecipient> >::iterator iterRecipientEnd = vecRecipients.end();

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

   shared_ptr<ServerInfo>
   ServerTargetResolver::_GetFixedSMTPHostForDomain(const String &sDomain)
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
      bool bUseSSL = false;

      shared_ptr<SMTPConfiguration> pSMTPConfig = Configuration::Instance()->GetSMTPConfiguration();

      // Check if we have any route for this domain.
      shared_ptr<Route> pRoute = pSMTPConfig->GetRoutes()->GetItemByName(sDomain);

      if (pRoute)
      {
         sSMTPHost = pRoute->TargetSMTPHost();
         lPort = pRoute->TargetSMTPPort();
         bUseSSL = pRoute->GetUseSSL();

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

            bUseSSL = pSMTPConfig->GetSMTPRelayerUseSSL();
         }
      }

      if (sSMTPHost.IsEmpty())
      {
         return shared_ptr<ServerInfo>();
      }

      shared_ptr<ServerInfo> serverInfo(new ServerInfo(true, sSMTPHost, lPort, sUsername, sPassword, bUseSSL));
      return serverInfo;

   }

}