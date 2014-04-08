// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "SpamTestRunner.h"

#include "SpamTestData.h"
#include "SpamTestResult.h"

#include "SpamTestDNSBlackLists.h"
#include "SpamTestHeloHost.h"
#include "SpamTestMXRecords.h"
#include "SpamTestSPF.h"
#include "SpamTestSURBL.h"
#include "SpamTestSpamAssassin.h"
#include "DKIM/SpamTestDKIM.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   SpamTestRunner::SpamTestRunner(void)
   {

   }

   SpamTestRunner::~SpamTestRunner(void)
   {

   }

   void 
   SpamTestRunner::LoadSpamTests()
   {
      m_setSpamTests.push_back(shared_ptr<SpamTestDNSBlackLists> (new SpamTestDNSBlackLists));
      m_setSpamTests.push_back(shared_ptr<SpamTestHeloHost> (new SpamTestHeloHost));
      m_setSpamTests.push_back(shared_ptr<SpamTestMXRecords> (new SpamTestMXRecords));
      m_setSpamTests.push_back(shared_ptr<SpamTestSPF> (new SpamTestSPF));
      m_setSpamTests.push_back(shared_ptr<SpamTestSURBL> (new SpamTestSURBL));
      m_setSpamTests.push_back(shared_ptr<SpamTestDKIM> (new SpamTestDKIM));
      m_setSpamTests.push_back(shared_ptr<SpamTestSpamAssassin> (new SpamTestSpamAssassin));
   }

   set<shared_ptr<SpamTestResult> >
   SpamTestRunner::RunSpamTest(shared_ptr<SpamTestData> pInputData, SpamTest::SpamTestType iType, int iMaxScore)
   {
      std::vector<shared_ptr<SpamTest> >::iterator iter = m_setSpamTests.begin(); 
      std::vector<shared_ptr<SpamTest> >::iterator iterEnd = m_setSpamTests.end();

      set<shared_ptr<SpamTestResult> > setTotalResult;

      int iTotalScore = 0;

      for (; iter != iterEnd; iter++)
      {
         shared_ptr<SpamTest> pSpamTest = (*iter);

         if (!pSpamTest->GetIsEnabled())
            continue;

         // Pre or post transmission?

         if (pSpamTest->GetTestType() != iType)
            continue;
         
         String sName = pSpamTest->GetName();

         set<shared_ptr<SpamTestResult> > setResult = pSpamTest->RunTest(pInputData);

         set<shared_ptr<SpamTestResult> >::iterator iter = setResult.begin();
         set<shared_ptr<SpamTestResult> >::iterator iterEnd = setResult.end();

         int totalScoreBefore = iTotalScore;
         for (; iter != iterEnd; iter++)
         {
            shared_ptr<SpamTestResult> pResult = (*iter);
            setTotalResult.insert(pResult);

            iTotalScore += pResult->GetSpamScore();
         }

         int totalDiff = iTotalScore - totalScoreBefore;

         String sSpamTestResult;
         sSpamTestResult.Format(_T("Spam test: %s, Score: %d"), sName, totalDiff);
         LOG_DEBUG(sSpamTestResult);

         if (iTotalScore >= iMaxScore)
         {
            // Threshold has been reached. No point in running any more tests.
            break;
         }

      }

      String sSpamTestResult;
      sSpamTestResult.Format(_T("Total spam score: %d"), iTotalScore);
      LOG_DEBUG(sSpamTestResult);
      
      return setTotalResult;
   }
}