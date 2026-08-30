// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

#include "../SpamTest.h"
#include "DMARCRecord.h"

namespace HM
{
   class AuthenticationResult;

   // Checks that a domain authenticated by SPF or DKIM aligns with the From header
   // domain, using the policy the From domain publishes. See RFC 7489.
   class SpamTestDMARC : public SpamTest
   {
   public:

      virtual String GetName() const;
      virtual bool GetIsEnabled();
      virtual SpamTestType GetTestType() {return PostTransmission; }

      // DMARC must run even when the spam score threshold has been reached, since
      // it can reject the message on its own.
      virtual bool GetAlwaysRun() {return true; }

      virtual std::set<std::shared_ptr<SpamTestResult> > RunTest(std::shared_ptr<SpamTestData> pTestData);

   private:

      static String GetHeaderFromDomain_(std::shared_ptr<SpamTestData> pTestData);

      // The SPF and DKIM results, calculated now if the tests themselves didn't run.
      void EnsureSPFResult_(std::shared_ptr<SpamTestData> pTestData, std::shared_ptr<AuthenticationResult> authenticationResult);
      void EnsureDKIMResults_(std::shared_ptr<SpamTestData> pTestData, std::shared_ptr<AuthenticationResult> authenticationResult);

      bool IsAuthenticated_(std::shared_ptr<AuthenticationResult> authenticationResult, const DMARCRecord &record, const String &headerFromDomain);

      // The policy to enforce, after applying the pct tag. See RFC 7489, section 6.6.4.
      static DMARCRecord::Policy GetPolicyToApply_(const DMARCRecord &record, DMARCRecord::Policy policy);
      static DMARCRecord::Policy DegradePolicy_(DMARCRecord::Policy policy);
   };
}
