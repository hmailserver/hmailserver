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
      _localDomainName = sDomainName;
   }

   String
   Diagnostic::GetLocalDomain() const
   {
      return _localDomainName;
   }

   void
   Diagnostic::SetTestDomain(String &sTestDomainName)
   {
      _localTestDomainName = sTestDomainName;
   }

   String
   Diagnostic::GetTestDomain() const
   {
      return _localTestDomainName;
   }

   vector<DiagnosticResult>
   Diagnostic::PerformTests()
   {
      vector<DiagnosticResult> results;

      TestInformationGatherer informationGatherer;
      results.push_back(informationGatherer.PerformTest());

      TestIPv6 ipv6Test; 
      results.push_back(ipv6Test.PerformTest());

      if (_localTestDomainName.GetLength() > 0)
      {
      TestOutboundPort outboundPortTest(_localTestDomainName);
      results.push_back(outboundPortTest.PerformTest());
      }
      
      TestBackupDirectory backupDirectoryTest;
      results.push_back(backupDirectoryTest.PerformTest());

      if (_localDomainName.GetLength() > 0)
      {
         TestMXRecords mxRecordsTest(_localDomainName);
         results.push_back(mxRecordsTest.PerformTest());

         TestConnectToSelf selfConnect(_localDomainName);
         results.push_back(selfConnect.PerformTest());
      }

      TestDataDirectory testDataDirectory;
      results.push_back(testDataDirectory.PerformTest());

      TestIPRanges testIPRanges;
      results.push_back(testIPRanges.PerformTest());

      return results;
   }  
}
