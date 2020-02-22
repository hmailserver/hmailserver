// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Message;
   class MessageRecipient;
   class RuleResult;
   class ServerInfo;
   class HostNameAndIpAddress;

   class ExternalDelivery
   {
   public:
      ExternalDelivery(const String &sSendersIP, std::shared_ptr<Message> message, const RuleResult &globalRuleResult);
      ~ExternalDelivery(void);

      bool Perform(std::vector<String> &saErrorMessages);

   private:



      void DeliverToSingleDomain_(std::vector<std::shared_ptr<MessageRecipient> > &vecRecipients, std::shared_ptr<ServerInfo> serverInfo);
      void DeliverToSingleServer_(std::vector<std::shared_ptr<MessageRecipient> > &vecRecipients, std::shared_ptr<ServerInfo> serverInfo);

      bool ResolveRecipientServers_(std::shared_ptr<ServerInfo> &serverInfo, std::vector<std::shared_ptr<MessageRecipient> > &vecRecipients, std::vector<HostNameAndIpAddress> &saMailServers);
      bool RecipientWithNonFatalDeliveryErrorExists_(std::vector<std::shared_ptr<MessageRecipient> > &vecRecipients);
      void HandleExternalDeliveryFailure_(std::vector<std::shared_ptr<MessageRecipient> > &vecRecipients, bool bIsFatal, String &sErrorString);
      void HandleNoRecipientServers_(std::vector<std::shared_ptr<MessageRecipient> > &vecRecipients, bool bDNSQueryOK, bool isSpecificRelayServer);
      
      void CollectDeliveryResult_(const String &serverHostName, std::vector<std::shared_ptr<MessageRecipient> > &vecRecipients, std::vector<String> &saErrorMessages, std::map<String,String> &mapFailedDueToNonFatalError);
      bool RescheduleDelivery_(std::map<String,String> &mapFailedDueToNonFatalError, std::vector<String> &saErrorMessages);
      // Type changed from void to bool for use with ETRN.
      // Function not called anywhere else to matter
      bool GetRetryOptions_(std::map<String,String> &mapFailedDueToNonFatalError, long &lNoOfRetries, long &lMinutesBetween);

      IPAddress GetLocalAddress_();

      const String &_sendersIP;
      const std::shared_ptr<Message> original_message_;
      const RuleResult &_globalRuleResult;   

      int quick_retries_;      
      int quick_retries_Minutes;      
      int queue_randomness_minutes_;
      int mxtries_factor_; 
   };
}