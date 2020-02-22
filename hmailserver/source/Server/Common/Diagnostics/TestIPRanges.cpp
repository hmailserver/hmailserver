// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "TestIPRanges.h"

#include "../BO/SecurityRange.h"
#include "../BO/SecurityRanges.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   TestIPRanges::TestIPRanges()
   {

   }

   TestIPRanges::~TestIPRanges()
   {

   }

   DiagnosticResult
   TestIPRanges::PerformTest()
   {
      DiagnosticResult diagResult;
      diagResult.SetName("Test IP range configuration");
      diagResult.SetDescription("Perform basic tests on IP ranges.");

      String result;
      String formattedString;
      diagResult.SetSuccess(true);

      SecurityRanges ranges;
      ranges.Refresh();

      int totalBanRanges = 0;
      bool localhostBanned = false;

      for(std::shared_ptr<SecurityRange> securityRange : ranges.GetVector())
      {
         if (securityRange->GetExpires())
         {
            totalBanRanges++;

            if (securityRange->GetLowerIPString() == _T("127.0.0.1") && 
                securityRange->GetUpperIPString() == _T("127.0.0.1"))
            {
               localhostBanned = true;
            }
         }
        
      }

      if (localhostBanned)
      {
         result.append(_T("ERROR: Localhost is currently banned in the IP ranges.\r\n"));
         diagResult.SetSuccess(false);
      }

      if (totalBanRanges > 0)
      {
         result.append(Formatter::Format("There is a total of {0} auto-ban IP ranges.\r\n", totalBanRanges));
      }
      else
      {
         result.append(_T("No problems were found in the IP range configuration.\r\n"));

      }
      

      diagResult.SetDetails(result);

      return diagResult;
   }


   
      
}
