// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "SpamTest.h"

namespace HM
{
   class SpamTestSPF : public SpamTest
   {
   public:
      
      virtual SpamTestType GetTestType()
      {
         return SpamTest::PreTransmission;
      }

      virtual String GetName() const;
      virtual bool GetIsEnabled();
      virtual set<shared_ptr<SpamTestResult> > RunTest(shared_ptr<SpamTestData> pTestData);

      static String GetTestName();
   private:

   };

}