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

      String GetName() const {return address_; }

      void SetAddress(const String & sAddress);
      String GetAddress(void) const {return address_;}

      __int64 GetListID() const  {return list_id_;}
      void SetListID(__int64 newVal) {list_id_ = newVal; }


      bool XMLStore(XNode *pParentNode, int iOptions);
      bool XMLLoad(XNode *pParentNode, int iOptions);
      bool XMLLoadSubItems(XNode *pParentNode, int iOptions) {return true; }

   protected:

      String address_;
      __int64 list_id_;
   };
}
