// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "stdafx.h"

#include "ARCTrustedSealers.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   ARCTrustedSealers::ARCTrustedSealers()
   {
   }

   ARCTrustedSealers::~ARCTrustedSealers(void)
   {
   }

   void
   ARCTrustedSealers::Refresh()
   {
      String sSQL = "select * from hm_arc_trusted_sealers order by sealerdomain asc";
      DBLoad_(sSQL);
   }

   bool
   ARCTrustedSealers::IsTrusted(const String &domain) const
   {
      if (domain.IsEmpty())
         return false;

      // Compared here rather than in SQL, since databases differ in case sensitivity.
      for (std::shared_ptr<ARCTrustedSealer> sealer : vecObjects)
      {
         String trustedDomain = sealer->GetDomain();
         trustedDomain.Trim();

         if (trustedDomain.CompareNoCase(domain) == 0)
            return true;
      }

      return false;
   }
}
