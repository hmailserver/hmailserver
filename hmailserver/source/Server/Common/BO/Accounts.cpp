// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "Accounts.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   Accounts::Accounts(__int64 iDomainID) :
      m_iDomainID(iDomainID),
      m_iAccountID(0)
   {
   
   }

   Accounts::Accounts(__int64 iDomainID, __int64 iAccountID) :
      m_iDomainID(iDomainID),
      m_iAccountID(iAccountID)
   {

   }

   Accounts::~Accounts  ()
   {
   }


   void
   Accounts::Refresh()
   {
      SQLCommand command;

      String sSQL;
      if (m_iDomainID == 0)
      {
         command.SetQueryString(_T("select * from hm_accounts order by accountaddress asc"));
      }
      else if (m_iAccountID == 0)
      {
         command.SetQueryString("select * from hm_accounts where accountdomainid = @DOMAINID order by accountaddress asc");
         command.AddParameter("@DOMAINID", m_iDomainID);
      }
      else
      {
         command.SetQueryString("select * from hm_accounts where accountdomainid = @DOMAINID and accountid = @ACCOUNTID order by accountaddress asc");
         command.AddParameter("@DOMAINID", m_iDomainID);
         command.AddParameter("@ACCOUNTID", m_iAccountID);
      }

      _DBLoad(command);
   }

   bool
   Accounts::PreSaveObject(shared_ptr<Account> pAccount, XNode *node)
   {
      pAccount->SetDomainID(m_iDomainID);
      return true;
   }

}
