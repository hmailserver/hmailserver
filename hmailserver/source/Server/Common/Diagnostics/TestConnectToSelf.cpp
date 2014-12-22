// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "TestConnectToSelf.h"

#include "../TCPIP/DNSResolver.h"
#include "TestConnect.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   TestConnectToSelf::TestConnectToSelf(const String &localDomainName) :
      local_domain_name_(localDomainName)
   {

   }

   TestConnectToSelf::~TestConnectToSelf()
   {

   }

   DiagnosticResult
   TestConnectToSelf::PerformTest()
   {
      DiagnosticResult diagResult;
      diagResult.SetName("Test local connect");
      diagResult.SetDescription("Confirms that it's possible to connect to the host specified in MX record");

      String result;

      String formattedString;
      formattedString.Format(_T("Connecting to TCP/IP address in MX records for local domain domain %s...\r\n"), local_domain_name_.c_str());
      result.append(formattedString);

      std::vector<String> foundNames;

      DNSResolver resolver;
      if (!resolver.GetMXRecords(local_domain_name_, foundNames) || foundNames.size() == 0)
      {
         formattedString.Format(_T("ERROR: MX records for local domain %s could not be resolved\r\n"), local_domain_name_.c_str());
         result.append(formattedString);
         
         diagResult.SetSuccess(false);
         diagResult.SetDetails(result);
         return diagResult;
      }

      IPAddress anyAddress;
      for(String foundName : foundNames)
      {
         // Test to connect
         TestConnect connTest;

         if (connTest.PerformTest(CSNone, "", foundName, 25, result))
         {
            diagResult.SetSuccess(true);
            diagResult.SetDetails(result);
            return diagResult;
         }
      }

      diagResult.SetSuccess(false);
      diagResult.SetDetails(result);
      return diagResult;
   }


   
      
}
