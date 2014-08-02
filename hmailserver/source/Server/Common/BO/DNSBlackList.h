// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class DNSBlackList : public BusinessObject<DNSBlackList>
   {
   public:
      DNSBlackList(void);
      ~DNSBlackList(void);

      String GetName() const {return dnshost_; }

      bool GetIsActive() const  {return active_; }
      void SetIsActive(bool bNewVal) {active_ = bNewVal;}
   
      int GetScore() {return score_; }
      void SetScore(int iNewVal) {score_ = iNewVal; }

      String GetRejectMessage() const  {return reject_message_; }
      void SetRejectMessage(const String &sNewVal) {reject_message_ = sNewVal;}

      String GetDNSHost() const  {return dnshost_; }
      void SetDNSHost(const String &sNewVal) {dnshost_ = sNewVal;}

      String GetExpectedResult() const  {return expected_result_; }
      void SetExpectedResult(const String &sNewVal) {expected_result_ = sNewVal;}

      bool XMLStore(XNode *pNode, int iOptions);
      bool XMLLoad(XNode *pNode, int iOptions);
      bool XMLLoadSubItems(XNode *pNode, int iOptions) {return true;}; 

   private:
      bool active_;

      String dnshost_;
      String expected_result_;
      String reject_message_;

      int score_;
   };
}