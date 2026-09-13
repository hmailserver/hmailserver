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

      // The results an SPF evaluation can produce, RFC 7208 section 2.6. The
      // values are internal only; they are never persisted or passed over COM.
      enum Result
      {
         // The domain publishes no SPF record, or no record could be looked up
         // because the domain is malformed or does not exist.
         None = 0,

         // A record was found but it makes no assertion about the client.
         Neutral = 1,

         // The client is authorized to send mail on behalf of the domain.
         Pass = 2,

         // The client is not authorized to send mail on behalf of the domain.
         Fail = 3,

         // The client is not authorized, but the domain asks that the message
         // is accepted rather than rejected.
         SoftFail = 4,

         // The evaluation could not be completed, typically because a DNS
         // lookup failed. Evaluating the same message later may give another
         // result.
         TempError = 5,

         // The record was found but could not be evaluated, because it is
         // syntactically invalid or exceeds a processing limit.
         PermError = 6
      };

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