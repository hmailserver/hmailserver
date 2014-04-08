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

      String GetName() const {return m_sDNSHost; }

      bool GetIsActive() const  {return m_bActive; }
      void SetIsActive(bool bNewVal) {m_bActive = bNewVal;}
   
      int GetScore() {return m_iScore; }
      void SetScore(int iNewVal) {m_iScore = iNewVal; }

      String GetRejectMessage() const  {return m_sRejectMessage; }
      void SetRejectMessage(const String &sNewVal) {m_sRejectMessage = sNewVal;}

      String GetDNSHost() const  {return m_sDNSHost; }
      void SetDNSHost(const String &sNewVal) {m_sDNSHost = sNewVal;}

      String GetExpectedResult() const  {return m_sExpectedResult; }
      void SetExpectedResult(const String &sNewVal) {m_sExpectedResult = sNewVal;}

      bool XMLStore(XNode *pNode, int iOptions);
      bool XMLLoad(XNode *pNode, int iOptions);
      bool XMLLoadSubItems(XNode *pNode, int iOptions) {return true;}; 

   private:
      bool m_bActive;

      String m_sDNSHost;
      String m_sExpectedResult;
      String m_sRejectMessage;

      int m_iScore;
   };
}