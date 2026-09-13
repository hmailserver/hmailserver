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

      Result Test(const String &sSenderIP, const String &sSenderEmail, const String &sHeloHost, String &sExplanation);  

   private:
      
   };

   class SPFTester
   {
   public :
      SPFTester () {};
      ~SPFTester () {};      

      void Test();
   };
}