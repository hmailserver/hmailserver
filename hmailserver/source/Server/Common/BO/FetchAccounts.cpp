// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include ".\fetchaccounts.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{

   FetchAccounts::FetchAccounts(__int64 iAccountID) :
      account_id_(iAccountID)         
   {

   }

   FetchAccounts::~FetchAccounts(void)
   {
   }

   void 
   FetchAccounts::RefreshPendingList()
   {
      String sSQL;
      sSQL.Format(_T("select * from hm_fetchaccounts where fanexttry <= %s and falocked = 0 and faactive = 1 order by faid asc"), SQLStatement::GetCurrentTimestamp().c_str());

      DBLoad_(sSQL);
   }


   void 
   FetchAccounts::Refresh()
   {
      String sSQL;
      sSQL.Format(_T("select * from hm_fetchaccounts where faaccountid = %I64d")
                        _T(" order by faid asc"), account_id_);

      DBLoad_(sSQL);
   }

   bool
   FetchAccounts::PreSaveObject(std::shared_ptr<FetchAccount> pFA, XNode *node)
   {
      pFA->SetAccountID(account_id_);
      return true;
   }


}