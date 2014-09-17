// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "distributionlists.h"


#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   DistributionLists::DistributionLists(__int64 iDomainID) :
      domain_id_(iDomainID)
   {

   }

   DistributionLists::~DistributionLists(void)
   {
   }

   std::shared_ptr<DistributionList>
   DistributionLists::GetItemByAddress(const String & sAddress)
   {

      for(std::shared_ptr<DistributionList> Item : vecObjects)
      {
         if (Item->GetAddress().CompareNoCase(sAddress) == 0)
            return Item;
      }

      std::shared_ptr<DistributionList> pEmpty;
      return pEmpty;
   }


   void
   DistributionLists::Refresh()
   {
      String sSQL;
      sSQL.Format(_T("select * from hm_distributionlists where distributionlistdomainid = %I64d order by distributionlistaddress asc"), domain_id_);

      DBLoad_(sSQL);
   }


   bool
   DistributionLists::PreSaveObject(std::shared_ptr<DistributionList> pList, XNode *node)
   {
      pList->SetDomainID(domain_id_);

      return true;
   }
}
