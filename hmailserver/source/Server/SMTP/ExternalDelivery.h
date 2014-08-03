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
      ExternalDelivery(const String &sSendersIP, shared_ptr<Message> message, const RuleResult &globalRuleResult);
      ~ExternalDelivery(void);

      bool Perform(vector<String> &saErrorMessages);

   private:



      void DeliverToSingleDomain_(vector<shared_ptr<MessageRecipient> > &vecRecipients, shared_ptr<ServerInfo> serverInfo);
      void DeliverToSingleServer_(vector<shared_ptr<MessageRecipient> > &vecRecipients, shared_ptr<ServerInfo> serverInfo);

      bool ResolveRecipientServers_(shared_ptr<ServerInfo> &serverInfo, vector<shared_ptr<MessageRecipient> > &vecRecipients, vector<HostNameAndIpAddress> &saMailServers);
      bool RecipientWithNonFatalDeliveryErrorExists_(vector<shared_ptr<MessageRecipient> > &vecRecipients);
      void HandleExternalDeliveryFailure_(vector<shared_ptr<MessageRecipient> > &vecRecipients, bool bIsFatal, String &sErrorString);
      void HandleNoRecipientServers_(vector<shared_ptr<MessageRecipient> > &vecRecipients, bool bDNSQueryOK, bool isSpecificRelayServer);
      
      void CollectDeliveryResult_(const String &serverHostName, vector<shared_ptr<MessageRecipient> > &vecRecipients, vector<String> &saErrorMessages, map<String,String> &mapFailedDueToNonFatalError);
      bool RescheduleDelivery_(map<String,String> &mapFailedDueToNonFatalError,vector<String> &saErrorMessages);
      // Type changed from void to bool for use with ETRN.
      // Function not called anywhere else to matter
      bool GetRetryOptions_(map<String,String> &mapFailedDueToNonFatalError, long &lNoOfRetries, long &lMinutesBetween);

      IPAddress GetLocalAddress_();

      const String &_sendersIP;
      const shared_ptr<Message> _originalMessage;
      const RuleResult &_globalRuleResult;   

      int quick_retries_;      
      int quick_retries_Minutes;      
      int queue_randomness_minutes_;
      int mxtries_factor_; 
   };
}