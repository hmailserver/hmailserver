// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "SPF.h"
#include "RMSPF.H"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   namespace
   {
      // Translate a result from the SPF library into our own result. The
      // library ORs a reason onto the result it returns and leaves it to the
      // caller to mask it off again, so anything but SPF_ResultMask must be
      // removed before the result is looked at.
      SPFResult TranslateResult(int libraryResult)
      {
         switch (libraryResult & SPF_ResultMask)
         {
         case SPF_Pass:
            return SPFResult::Pass;
         case SPF_SoftFail:
            return SPFResult::SoftFail;
         case SPF_Fail:
            return SPFResult::Fail;
         case SPF_Neutral:
            return SPFResult::Neutral;
         case SPF_None:
            return SPFResult::None;
         case SPF_TempError:
            return SPFResult::TempError;
         case SPF_PermError:
            return SPFResult::PermError;
         }

         // The library documents no other result. Report that nothing was
         // learned rather than blaming the sender for a value we do not know.
         return SPFResult::None;
      }
   }

   SPF::SPF(void)
   {
      // Initialize. This is only done once.
      SPFInit(NULL,0, SPF_Multithread);
   }

   SPF::~SPF(void)
   {

   }

   SPFResult
   SPF::Test(const String &sSenderIP, const String &sSenderEmail, const String &sHeloHost, String &sExplanation)
   {
      USES_CONVERSION;
      String sDomain = StringParser::ExtractDomain(sSenderEmail);

      int family;
      if (sSenderIP.Find(_T(":")) > 0)
         family=AF_INET6;
      else
         family=AF_INET;

      // Convert the IP address from a dotted string
      // to a binary form. We use the SPF library to
      // do this.

      char BinaryIP[100];
      if (SPFStringToAddr(T2A(sSenderIP),family,BinaryIP)==NULL)
         return SPFResult::Neutral;

      const char* explain;
      int libraryResult=SPFQuery(family,BinaryIP,T2A(sSenderEmail),NULL,T2A(sHeloHost),NULL,&explain);

      if (explain != NULL)
      {
         sExplanation = explain;
         SPFFree(explain);
      }

      SPFResult result = TranslateResult(libraryResult);

      // Only the results this function has always reported are passed on. The
      // remaining ones are reported once the evaluator behind them is the one
      // which determines them, so that callers see a single change rather than
      // one now and another later.
      if (result == SPFResult::Pass || result == SPFResult::Fail)
         return result;

      return SPFResult::Neutral;
   }

   void SPFTester::Test()
   {
      String sExplanation;
      
      if (SPF::Instance()->Test("185.216.75.37", "example@hmailserver.com", "mail.hmailserver.com", sExplanation) != SPFResult::Pass)
      {
         // Should be allowed. 
         throw;
      }

      if (SPF::Instance()->Test("1.2.3.4", "example@hmailserver.com", "mail.hmailserver.com", sExplanation) != SPFResult::Fail)
      {
         // Should not be allowed.
         throw;
      }
   }


}