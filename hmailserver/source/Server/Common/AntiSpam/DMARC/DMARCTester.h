// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

#include "DMARCRecord.h"

namespace HM
{
   class DMARCTester
   {
   public:

      void Test();

   private:

      void TestRecordParsing_();
      void TestOrganizationalDomain_();
      void TestAlignment_();
      void TestApplicablePolicy_();
      void TestPolicyLocator_();

      void AssertInvalidRecord_(const AnsiString &textRecord);
      DMARCRecord ParseValidRecord_(const AnsiString &textRecord);
   };
}
