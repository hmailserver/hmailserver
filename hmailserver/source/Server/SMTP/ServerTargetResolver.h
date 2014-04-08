// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Message;
   class RuleResult;
   class ServerInfo;
   class MessageRecipient;

   class ServerTargetResolver
   {
   public:
      ServerTargetResolver(shared_ptr<Message> message, const RuleResult &globalRuleResult);
      ~ServerTargetResolver(void);
      
      map<shared_ptr<ServerInfo>, std::vector<shared_ptr<MessageRecipient> > > Resolve();

      

   private:

      map<shared_ptr<ServerInfo>, std::vector<shared_ptr<MessageRecipient> > > CreateDistinctMap(map<shared_ptr<ServerInfo>, std::vector<shared_ptr<MessageRecipient> > > serverInfos);

      static shared_ptr<ServerInfo> _GetFixedSMTPHostForDomain(const String &sDomain);

      shared_ptr<Message> _message;
      const RuleResult &_globalRuleResult;
   };
}