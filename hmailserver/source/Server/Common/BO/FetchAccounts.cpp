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
      m_iAccountID(iAccountID)         
   {

   }

   FetchAccounts::~FetchAccounts(void)
   {
   }

   void 
   FetchAccounts::RefreshPendingList()
   {
      String sSQL;
      sSQL.Format(_T("select * from hm_fetchaccounts where fanexttry <= %s and falocked = 0 and faactive = 1 order by faid asc"), SQLStatement::GetCurrentTimestamp());

      _DBLoad(sSQL);
   }


   void 
   FetchAccounts::Refresh()
   {
      String sSQL;
      sSQL.Format(_T("select * from hm_fetchaccounts where faaccountid = %I64d")
                        _T(" order by faid asc"), m_iAccountID);

      _DBLoad(sSQL);
   }

   bool
   FetchAccounts::PreSaveObject(shared_ptr<FetchAccount> pFA, XNode *node)
   {
      pFA->SetAccountID(m_iAccountID);
      return true;
   }


}