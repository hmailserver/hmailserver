// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

#include "../SpamTest.h"

namespace HM
{
   // Validates the ARC chain so that the result can be reported in Authentication-Results
   // and used by DMARC. Adds no score: a broken chain alone says nothing about spam.
   class SpamTestARC : public SpamTest
   {
   public:

      virtual String GetName() const;
      virtual bool GetIsEnabled();
      virtual SpamTestType GetTestType() {return PostTransmission; }

      // The result is reported even when the spam score threshold has been reached.
      virtual bool GetAlwaysRun() {return true; }

      virtual std::set<std::shared_ptr<SpamTestResult> > RunTest(std::shared_ptr<SpamTestData> pTestData);
   };
}
