// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

#include "SPFResult.h"

namespace HM
{
   class SPF : public Singleton<SPF>
   {
   public:
      SPF(void);
      ~SPF(void);

      // The results of RFC 7208 section 2.6, which SPFResult spells.
      typedef SPFResult Result;

      // check_host() of RFC 7208 section 4, over real DNS. The explanation is the
      // text of the record's exp modifier, expanded, and is set only for a Fail
      // which the record explained.
      Result Test(const String &sSenderIP, const String &sSenderEmail, const String &sHeloHost, String &sExplanation);

      // The domain a check is made against, RFC 7208 section 4.3: the sender's domain,
      // or the HELO argument where the sender has none, section 2.4. Public because
      // Authentication-Results names it and the two must not be able to disagree.
      static String GetCheckedDomain(const String &senderEmail, const String &heloHost);

   private:

   };
}
