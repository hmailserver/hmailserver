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
      m_iDomainID(iDomainID)
   {

   }

   Aliases::~Aliases()
   {
   }

   void
   Aliases::Refresh()
   {
      String sSQL;
      sSQL.Format(_T("select * from hm_aliases where aliasdomainid = %I64d order by aliasname asc"), m_iDomainID);

      _DBLoad(sSQL);
   }

   bool
   Aliases::PreSaveObject(shared_ptr<Alias> pAlias, XNode *node)
   {
      pAlias->SetDomainID(m_iDomainID);

      return true;
   }

}
