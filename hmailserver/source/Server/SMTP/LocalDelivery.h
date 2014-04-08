// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Message;
   class RuleResult;

   class LocalDelivery
   {
   public:
      LocalDelivery(const String &sSendersIP, shared_ptr<Message> message, const RuleResult &globalRuleResult);
      ~LocalDelivery(void);

      bool Perform(vector<String> &saErrorMessages);

   private:
      
      void _DeliverToLocalAccount(shared_ptr<const Account> account, int iNoOfRecipients, vector<String> &saErrorMessages, const String &sOriginalAddress, bool &messageReused);
      bool _LocalDeliveryPreProcess(shared_ptr<const Account> account, shared_ptr<Message> accountLevelMessage, const String &sOriginalAddress, vector<String> &saErrorMessages);
      bool _AddTraceHeaders(shared_ptr<const Account> account, shared_ptr<Message> pMessage, const String &sOriginalAddress);
      void _SendAutoReplyMessage(shared_ptr<const Account> pAccount, shared_ptr<Message> pMessage);
      bool _RunAccountRules(shared_ptr<const Account> pAccount, shared_ptr<Message> pMessage, RuleResult &accountRuleResult);
      bool _CheckAccountQuotas(shared_ptr<const Account> pAccount, vector<String> &saErrorMessages);

      shared_ptr<Message>  _CreateAccountLevelMessage(shared_ptr<Message> pOriginalMessage, shared_ptr<const Account> pRecipientAccount, bool reuseMessage, const String &sOriginalAddress);

      const String &_sendersIP;
      const shared_ptr<Message> _originalMessage;
      const RuleResult &_globalRuleResult;

   };
}