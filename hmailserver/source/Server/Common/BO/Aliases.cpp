// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "Aliases.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   Aliases::Aliases(__int64 iDomainID) :
      domain_id_(iDomainID)
   {

   }

   Aliases::~Aliases()
   {
   }

   void
   Aliases::Refresh()
   {
      String sSQL;
      sSQL.Format(_T("select * from hm_aliases where aliasdomainid = %I64d order by aliasname asc"), domain_id_);

      DBLoad_(sSQL);
   }

   bool
   Aliases::PreSaveObject(std::shared_ptr<Alias> pAlias, XNode *node)
   {
      pAlias->SetDomainID(domain_id_);

      return true;
   }

}
