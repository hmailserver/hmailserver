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
      ServerTargetResolver(std::shared_ptr<Message> message, const RuleResult &globalRuleResult);
      ~ServerTargetResolver(void);
      
      std::map<std::shared_ptr<ServerInfo>, std::vector<std::shared_ptr<MessageRecipient> > > Resolve();

      

   private:

      std::map<std::shared_ptr<ServerInfo>, std::vector<std::shared_ptr<MessageRecipient> > > CreateDistinctMap(std::map<std::shared_ptr<ServerInfo>, std::vector<std::shared_ptr<MessageRecipient> > > serverInfos);

      static std::shared_ptr<ServerInfo> GetFixedSMTPHostForDomain_(const String &sDomain);

      std::shared_ptr<Message> message_;
      const RuleResult &_globalRuleResult;
   };
}