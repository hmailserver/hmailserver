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

      static void DeliverMessage(std::shared_ptr<Message> pMessage);

   private:

      static bool PreprocessMessage_(std::shared_ptr<Message> pMessage, RuleResult& globalRuleResult, String& preprocessingFailureReason);

      static bool RunVirusProtection_(std::shared_ptr<Message> pMessage);
      static bool RunGlobalRules_(std::shared_ptr<Message> pMessage, RuleResult &ruleResult);

      static void SubmitErrorLog_(std::shared_ptr<Message> pOrigMessage, std::vector<String> &saErrorMessages);

      static bool HandleInfectedMessage_(std::shared_ptr<Message> pMessage, const String &virusName);
      
      

      static void LogAwstatsMessageRejected_(const String &sSendersIP, std::shared_ptr<Message> pMessage, const String &sReason);

      static std::shared_ptr<Message> CreateAccountLevelMessage(std::shared_ptr<Message> pOriginalMessage, std::shared_ptr<Account> pRecipientAccount, bool reuseMessage, const String &sOriginalAddress);

      
   };
}