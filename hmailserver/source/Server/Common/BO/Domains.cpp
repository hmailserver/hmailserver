// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "Domains.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   Domains::Domains()
   {
      
   }

   Domains::~Domains()
   {
   
   }

   void
   Domains::Refresh()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Reads all domains from the database
   //---------------------------------------------------------------------------()
   {
      String sSQL = "select * from hm_domains order by domainname asc";
      _DBLoad(sSQL);
   }

   void 
   Domains::Refresh(__int64 iDomainID)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Reads a specific domain from the database.
   //---------------------------------------------------------------------------()
   {
      String sSQL;
      sSQL.Format(_T("select * from hm_domains where domainid = %I64d"), iDomainID);

      _DBLoad(sSQL);
   }

   String 
   Domains::GetNames()
   {
      bool first = true;
      String result;
      boost_foreach(shared_ptr<Domain> domain, vecObjects)
      {
         if (first)
            first = false;

         String append;
         append.Format(_T("%I64d\t%s\t%d\r\n"), domain->GetID(), domain->GetName(), domain->GetIsActive() ? 1 :0);

         result += append;
      }

      return result;
   }
}
