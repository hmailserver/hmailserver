// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../Common/BO/RuleCriteria.h"

namespace HM
{

   class Message;
   class Rules;
   class Rule; 
   class RuleCriteria;
   class MessageData;
   class RuleAction;
   class RuleResult;
   
   class RuleApplier
   {
   public:
      RuleApplier(void);
      ~RuleApplier(void);

      void ApplyRules(std::shared_ptr<Rules> pRules, std::shared_ptr<const Account> account, std::shared_ptr<Message> pMessage, RuleResult &ruleResult);

      static bool TestMatch(String matchValue, RuleCriteria::MatchType matchType, String testValue);

      static bool IsGeneratedResponseAllowed(std::shared_ptr<MessageData> pMsgData, bool bChkAutoSubmit);

   private:

      // Apply one rule to the message.
      bool ApplyRule_(std::shared_ptr<Rule> pRule, std::shared_ptr<const Account> account, std::shared_ptr<MessageData> pMsgData, bool &bContinueRuleProcessing, RuleResult &ruleResult);

      // Do the actions for the message.
      void ApplyActions_(std::shared_ptr<Rule> pRule, std::shared_ptr<const Account> account, std::shared_ptr<MessageData> pMsgData, bool &bContinueRuleProcessing, RuleResult &ruleResult);

      // Do the action
      void ApplyAction_(std::shared_ptr<Rule> pRule, std::shared_ptr<RuleAction> pAction, std::shared_ptr<const Account> account, std::shared_ptr<MessageData> pMsgData, bool &bContinueRuleProcessing, RuleResult &ruleResult);

      // Check wether pMessage matches pCriteria.
      bool MessageMatchesCriteria_(std::shared_ptr<RuleCriteria> pCriteria, std::shared_ptr<MessageData> pMsgData) const;
      
      // Actions
      void ApplyAction_Forward(std::shared_ptr<RuleAction> pAction, std::shared_ptr<const Account> account, std::shared_ptr<MessageData> pMsgData) const;
      void ApplyAction_Copy(std::shared_ptr<Rule> rule, std::shared_ptr<const Account> account, std::shared_ptr<MessageData> pMsgData) const;
      void ApplyAction_Reply(std::shared_ptr<RuleAction> pAction, std::shared_ptr<const Account> account, std::shared_ptr<MessageData> pMsgData) const;
      void ApplyAction_ScriptFunction(std::shared_ptr<RuleAction> pAction, std::shared_ptr<const Account> account, std::shared_ptr<MessageData> pMsgData) const;
      void ApplyAction_SetHeader(std::shared_ptr<RuleAction> pAction, std::shared_ptr<const Account> account, std::shared_ptr<MessageData> pMsgData) const;

      ;

   private:

      __int64 rule_account_id_;
      
   };
}