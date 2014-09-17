// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "TestMXRecords.h"

#include "../TCPIP/DNSResolver.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   TestMXRecords::TestMXRecords(const String &localDomainName) :
      local_domain_name_ (localDomainName)
   {

   }

   TestMXRecords::~TestMXRecords()
   {

   }

   DiagnosticResult
   TestMXRecords::PerformTest()
   {

      DiagnosticResult diagResult;
      diagResult.SetName("Test MX records");
      diagResult.SetDescription("Confirms that MX records for the domain can be looked up.");
     
      String result;

      String formattedString;
      formattedString.Format(_T("Trying to resolve MX records for %s...\r\n"), local_domain_name_.c_str());
      result.append(formattedString);

      std::vector<String> foundNames;

      DNSResolver resolver;
      if (!resolver.GetMXRecords(local_domain_name_, foundNames) || foundNames.size() == 0)
      {
         formattedString.Format(_T("ERROR: MX records for domain %s could not be resolved\r\n"), local_domain_name_.c_str());
         result.append(formattedString);
         
         diagResult.SetDetails(result);
         diagResult.SetSuccess(false);

         return diagResult;
      }

      for(String foundName : foundNames)
      {
         formattedString.Format(_T("Host name found: %s\r\n"), foundName.c_str());
         result.append(formattedString);
      }

      diagResult.SetDetails(result);
      diagResult.SetSuccess(true);

      return diagResult;
   }


   
      
}
