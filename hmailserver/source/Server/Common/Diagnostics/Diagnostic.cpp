// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "Diagnostic.h"
#include "TestInformationGatherer.h"
#include "TestOutboundPort.h"
#include "TestMXRecords.h"
#include "TestConnectToSelf.h"
#include "TestIPv6.h"
#include "TestBackupDirectory.h"
#include "TestDataDirectory.h"
#include "TestIPRanges.h"
#include "TestErrorLogs.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   Diagnostic::Diagnostic()
   {

   }

   Diagnostic::~Diagnostic()
   {

   }

   void
   Diagnostic::SetLocalDomain(String &sDomainName)
   {
      local_domain_name_ = sDomainName;
   }

   String
   Diagnostic::GetLocalDomain() const
   {
      return local_domain_name_;
   }

   void
   Diagnostic::SetTestDomain(String &sTestDomainName)
   {
      local_test_domain_name_ = sTestDomainName;
   }

   String
   Diagnostic::GetTestDomain() const
   {
      return local_test_domain_name_;
   }

   std::vector<DiagnosticResult>
   Diagnostic::PerformTests()
   {
      std::vector<DiagnosticResult> results;

      TestInformationGatherer informationGatherer;
      results.push_back(informationGatherer.PerformTest());

      TestIPv6 ipv6Test; 
      results.push_back(ipv6Test.PerformTest());

      if (local_test_domain_name_.GetLength() > 0)
      {
         TestOutboundPort outboundPortTest(local_test_domain_name_);
         results.push_back(outboundPortTest.PerformTest());
      }
      
      TestBackupDirectory backupDirectoryTest;
      results.push_back(backupDirectoryTest.PerformTest());

      if (local_domain_name_.GetLength() > 0)
      {
         TestMXRecords mxRecordsTest(local_domain_name_);
         results.push_back(mxRecordsTest.PerformTest());

         TestConnectToSelf selfConnect(local_domain_name_);
         results.push_back(selfConnect.PerformTest());
      }

      TestDataDirectory testDataDirectory;
      results.push_back(testDataDirectory.PerformTest());

      TestIPRanges testIPRanges;
      results.push_back(testIPRanges.PerformTest());

      TestErrorLogs test_error_logs;
      results.push_back(test_error_logs.PerformTest());


      return results;
   }  
}
