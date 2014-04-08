// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../Common/BO/RuleCriteria.h"
#include "RuleResult.h"

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

      void ApplyRules(shared_ptr<Rules> pRules, shared_ptr<const Account> account, shared_ptr<Message> pMessage, RuleResult &ruleResult);

      static bool TestMatch(String matchValue, RuleCriteria::MatchType matchType, String testValue);

      static bool IsGeneratedResponseAllowed(shared_ptr<MessageData> pMsgData, bool bChkAutoSubmit);

   private:

      // Apply one rule to the message.
      bool _ApplyRule(shared_ptr<Rule> pRule, shared_ptr<const Account> account, shared_ptr<MessageData> pMsgData, bool &bContinueRuleProcessing, RuleResult &ruleResult);

      // Do the actions for the message.
      void _ApplyActions(shared_ptr<Rule> pRule, shared_ptr<const Account> account, shared_ptr<MessageData> pMsgData, bool &bContinueRuleProcessing, RuleResult &ruleResult);

      // Do the action
      void _ApplyAction(shared_ptr<Rule> pRule, shared_ptr<RuleAction> pAction, shared_ptr<const Account> account, shared_ptr<MessageData> pMsgData, bool &bContinueRuleProcessing, RuleResult &ruleResult);

      // Check wether pMessage matches pCriteria.
      bool _MessageMatchesCriteria(shared_ptr<RuleCriteria> pCriteria, shared_ptr<MessageData> pMsgData) const;
      
      // Actions
      void _ApplyActionForward(shared_ptr<RuleAction> pAction, shared_ptr<const Account> account, shared_ptr<MessageData> pMsgData) const;
      void _ApplyActionCopy(shared_ptr<Rule> rule, shared_ptr<const Account> account, shared_ptr<MessageData> pMsgData) const;
      void _ApplyActionReply(shared_ptr<RuleAction> pAction, shared_ptr<MessageData> pMsgData) const;
      void _ApplyActionScriptFunction(shared_ptr<RuleAction> pAction, shared_ptr<const Account> account, shared_ptr<MessageData> pMsgData) const;
      void _ApplyActionSetHeader(shared_ptr<RuleAction> pAction, shared_ptr<const Account> account, shared_ptr<MessageData> pMsgData) const;

      ;

   private:

      __int64 m_iRuleAccountID;
      
   };
}