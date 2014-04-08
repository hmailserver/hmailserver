// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class SPF : public Singleton<SPF>
   {
   public:
      SPF(void);
      ~SPF(void);

      enum Result
      {
         Neutral = 0,
         Fail = 1,
         Pass = 2
      };

      Result Test(const String &sSenderIP, const String &sSenderEmail, String &sExplanation);

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