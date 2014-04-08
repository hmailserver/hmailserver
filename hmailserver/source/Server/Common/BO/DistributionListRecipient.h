// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

 #pragma once


namespace HM
{
   class DistributionListRecipient : public BusinessObject<DistributionListRecipient>
   {
   public:
      DistributionListRecipient(void);
      ~DistributionListRecipient(void);

      String GetName() const {return m_sAddress; }

      void SetAddress(const String & sAddress);
      String GetAddress(void) const {return m_sAddress;}

      __int64 GetListID() const  {return m_iListID;}
      void SetListID(__int64 newVal) {m_iListID = newVal; }


      bool XMLStore(XNode *pParentNode, int iOptions);
      bool XMLLoad(XNode *pParentNode, int iOptions);
      bool XMLLoadSubItems(XNode *pParentNode, int iOptions) {return true; }

   protected:

      String m_sAddress;
      __int64 m_iListID;
   };
}
