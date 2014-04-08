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

      set<shared_ptr<SpamTestResult> > RunSpamTest(shared_ptr<SpamTestData> pInputData, SpamTest::SpamTestType iType, int iMaxScore);

   private:

      std::vector<shared_ptr<SpamTest> > m_setSpamTests;

   };

}