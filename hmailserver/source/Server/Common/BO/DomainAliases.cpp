// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "DomainAliases.h"
#include "DomainAlias.h"

#include "Domain.h"

#include "../Persistence/PersistentDomain.h"
#include "../Cache/CacheContainer.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   DomainAliases::DomainAliases(__int64 iDomainID) :
      domain_id_(iDomainID)
   {
   }

   DomainAliases::~DomainAliases(void)
   {
   }

   void 
   DomainAliases::Refresh()
   {
      String sSQL;

      if (domain_id_)
         sSQL.Format(_T("select * from hm_domain_aliases where dadomainid = %I64d order by daid asc"), domain_id_);
      else
         sSQL.Format(_T("select * from hm_domain_aliases order by daid asc"), domain_id_);

      DBLoad_(sSQL);
   }

   String 
   DomainAliases::ApplyAliasesOnAddress(const String &sAddress)
   {
      const String sDomainName = StringParser::ExtractDomain(sAddress);
      const String sMailbox = StringParser::ExtractAddress(sAddress);

      // Iterate over the domains to find a match.
      auto iterAccount = vecObjects.begin();
      
      for(std::shared_ptr<DomainAlias> pFA : vecObjects)
      {
         if (pFA->GetAlias().CompareNoCase(sDomainName) == 0)
         {
            // We found the domain ID
            __int64 iDomainID = pFA->GetDomainID();
         
            std::shared_ptr<const Domain> pDomain = CacheContainer::Instance()->GetDomain(iDomainID);
            
            if (!pDomain)
               return sAddress;

            String sRetVal = sMailbox + "@" + pDomain->GetName();
         
            return sRetVal; 
         }
      }

      return sAddress;
   }

   bool
   DomainAliases::PreSaveObject(std::shared_ptr<DomainAlias> pDA, XNode *node)
   {
      pDA->SetDomainID(domain_id_);
      return true;
   }
}