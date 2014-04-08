// Copyright (c) 2006 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Message;
   class MessageRecipient;
   class Account;
   class MessageData;
   class RuleResult;
   class ServerInfo;

   class SMTPDeliverer
   {
   public:
	   SMTPDeliverer();
	   virtual ~SMTPDeliverer();

      static void DeliverMessage(shared_ptr<Message> pMessage);

   private:

      static bool _PreprocessMessage(shared_ptr<Message> pMessage, String &sendersIP, RuleResult &ruleResult);

      static bool _RunVirusProtection(shared_ptr<Message> pMessage);
      static bool _RunGlobalRules(shared_ptr<Message> pMessage, RuleResult &ruleResult);

      static void _DeliverToLocalAccounts(const String &sSendersIP, shared_ptr<Message> pMessage, vector<String> &saErrorMessages, const RuleResult &globalRuleResult, bool &messageReused);
      
      
      
      
      static void _SubmitErrorLog(shared_ptr<Message> pOrigMessage, vector<String> &saErrorMessages);

      static bool _HandleInfectedMessage(shared_ptr<Message> pMessage, const String &virusName);
      
      

      static void _LogAwstatsMessageRejected(const String &sSendersIP, shared_ptr<Message> pMessage, const String &sReason);

      // Account-level tasks
      static void _DeliverToLocalAccount(shared_ptr<Account> pAccount, shared_ptr<Message> pMessage, int iNoOfRecipients, const RuleResult &globalRuleResult, vector<String> &saErrorMessages, const String &sSendersIP, const String &sOriginalAddress, bool &messageReused);

      static bool _RunAccountRules(shared_ptr<Account> pAccount, shared_ptr<Message> pMessage, RuleResult &accountRuleResult);
      static shared_ptr<Message> CreateAccountLevelMessage(shared_ptr<Message> pOriginalMessage, shared_ptr<Account> pRecipientAccount, bool reuseMessage, const String &sOriginalAddress);

      
      
      
      

      static bool _LocalDeliveryPreProcess(shared_ptr<Message> originalMessage, shared_ptr<Account> account, shared_ptr<Message> pMessage, const String &sOriginalAddress, vector<String> &saErrorMessages, const RuleResult &globalRuleResult);
   };
}