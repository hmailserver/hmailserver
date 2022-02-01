// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "SpamTest.h"

namespace HM
{
   class IPAddress;

   class SpamTestPTR : public SpamTest
   {
   public:
      
      virtual SpamTestType GetTestType()
      {
         return SpamTest::PreTransmission;
      }

      virtual String GetName() const;
      virtual bool GetIsEnabled();

      virtual std::set<std::shared_ptr<SpamTestResult> > RunTest(std::shared_ptr<SpamTestData> pTestData);

   private:

      bool CheckPTR_(const IPAddress &iIPAddress);

   };

}