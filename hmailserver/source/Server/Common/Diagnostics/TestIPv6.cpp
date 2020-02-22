// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "TestIPV6.h"

#include "../TCPIP/TCPServer.h"
#include "../../SMTP/SMTPConnection.h"



#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   TestIPv6::TestIPv6()
   {

   }

   TestIPv6::~TestIPv6()
   {

   }

   DiagnosticResult
   TestIPv6::PerformTest()
   {
      DiagnosticResult diagResult;
      diagResult.SetName("Test IPv6");
      diagResult.SetDescription("Checks whether IPv6 is available...");
     
      String result;

      if (Configuration::Instance()->IsIPv6Available())
      {
         String formattedString;
         formattedString.Format(_T("IPv6 support is available in operating system.\r\n"));
         result.append(formattedString);
      }
      else
      {
         String formattedString;
         formattedString.Format(_T("IPv6 support is not available in operating system.\r\n"));
         result.append(formattedString);
      }

      diagResult.SetDetails(result);
      diagResult.SetSuccess(true);

      return diagResult;
   }


   
      
}
