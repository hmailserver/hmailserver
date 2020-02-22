// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "SpamTest.h"

namespace HM
{
   class SpamTestData;
   class SpamTestResult;

   class SpamTestRunner
   {
   public:
      
      SpamTestRunner();
      virtual ~SpamTestRunner();

      void LoadSpamTests();

      std::set<std::shared_ptr<SpamTestResult> > RunSpamTest(std::shared_ptr<SpamTestData> pInputData, SpamTest::SpamTestType iType, int iMaxScore);

   private:

      std::vector<std::shared_ptr<SpamTest> > spam_tests_;

   };

}