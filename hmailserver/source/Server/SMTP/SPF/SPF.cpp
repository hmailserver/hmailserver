// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "SPF.h"
#include "rmspf.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   SPF::SPF(void)
   {
      // Initialize. This is only done once.
      SPFInit(NULL,0, SPF_Multithread);
   }

   SPF::~SPF(void)
   {

   }

   SPF::Result
   SPF::Test(const String &sSenderIP, const String &sSenderEmail, String &sExplanation)
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

      char BinaryIP[16];
      if (SPFStringToAddr(T2A(sSenderIP),family,BinaryIP)==NULL)
         return Neutral;

      const char* explain;
      int result=SPFQuery(family,BinaryIP,T2A(sSenderEmail),NULL,NULL,NULL,&explain);

      if (explain != NULL)
      {
         sExplanation = explain;
         SPFFree(explain);
      }

      if (result == SPF_Fail)
      {
         // FAIL
         return Fail;
      }
      else if (result == SPF_Pass)
      {
         return Pass;
      }

      return Neutral;
   }

   void SPFTester::Test()
   {
      String sExplanation;

      if (SPF::Instance()->Test("140.211.11.3", "users-return-12950-webmaster=domain1.de@httpd.apache.org", sExplanation) != SPF::Pass)
      {
         // Should be allowed.
         throw;
      }

      OutputDebugStringA("Starting test...\r\n");
      if (SPF::Instance()->Test("141.123.120.33", "example@advantagepayroll.com", sExplanation) != SPF::Pass)
      {
         // Should be allowed. advantagepayroll.com has SPF record mx ~all which means that
         // their MX servers are allowed to send.
         throw;
      }

      if (SPF::Instance()->Test("1.2.3.4", "example@instantpayroll.advantagepayroll.com", sExplanation) != SPF::Neutral)
      {
         // Should be allowed. The sub domain instantpayroll.advantagepayroll.com does not have a SPF record.
         throw;
      }

      if (SPF::Instance()->Test("1.2.3.4", "example@advantagepayroll.com", sExplanation) != SPF::Fail)
      {
         // Should not be allowed. advantagepayroll.com has SPF records.
         throw;
      }

 
     



   }


}