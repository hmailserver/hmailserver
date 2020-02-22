// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "distributionlistrecipients.h"

#include "../Persistence/PersistentDistributionListRecipient.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   DistributionListRecipients::DistributionListRecipients(__int64 iListID) :
      list_id_(iListID)   
   {

   }

   DistributionListRecipients::~DistributionListRecipients(void)
   {

   }

   void
   DistributionListRecipients::Refresh()
   {
      String sSQL;
      sSQL.Format(_T("select * from hm_distributionlistsrecipients where distributionlistrecipientlistid = %I64d order by distributionlistrecipientaddress asc "), list_id_);
     
      DBLoad_(sSQL);
   }

   bool
   DistributionListRecipients::PreSaveObject(std::shared_ptr<DistributionListRecipient> pListRecipient, XNode *node)
   {
      pListRecipient->SetListID(list_id_);

      return true;
   }
}
