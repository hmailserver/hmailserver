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

      static bool PreprocessMessage_(shared_ptr<Message> pMessage, String &sendersIP, RuleResult &ruleResult);

      static bool RunVirusProtection_(shared_ptr<Message> pMessage);
      static bool RunGlobalRules_(shared_ptr<Message> pMessage, RuleResult &ruleResult);

      static void SubmitErrorLog_(shared_ptr<Message> pOrigMessage, vector<String> &saErrorMessages);

      static bool HandleInfectedMessage_(shared_ptr<Message> pMessage, const String &virusName);
      
      

      static void LogAwstatsMessageRejected_(const String &sSendersIP, shared_ptr<Message> pMessage, const String &sReason);

      static shared_ptr<Message> CreateAccountLevelMessage(shared_ptr<Message> pOriginalMessage, shared_ptr<Account> pRecipientAccount, bool reuseMessage, const String &sOriginalAddress);

      
   };
}