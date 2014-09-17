// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "SpamTestDKIM.h"

#include "../SpamTestData.h"
#include "../SpamTestResult.h"
#include "../../BO/MessageData.h"
#include "../AntiSpamConfiguration.h"
#include "DKIM.h"
#include "../../Persistence/PersistentMessage.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   String 
   SpamTestDKIM::GetName() const
   {
      return "SpamTestDKIM";
   }

   bool 
   SpamTestDKIM::GetIsEnabled()
   {
      AntiSpamConfiguration &config = Configuration::Instance()->GetAntiSpamConfiguration();
      return config.GetDKIMVerificationEnabled();
   }

   std::set<std::shared_ptr<SpamTestResult> >
   SpamTestDKIM::RunTest(std::shared_ptr<SpamTestData> pTestData)
   {
      std::shared_ptr<Message> pMessage = pTestData->GetMessageData()->GetMessage();

      std::set<std::shared_ptr<SpamTestResult> > setSpamTestResults;

      const String fileName = PersistentMessage::GetFileName(pMessage);

      DKIM dkim;
      DKIM::Result result = dkim.Verify(fileName);
      if (result == DKIM::PermFail)
      {
         // Blocked
         AntiSpamConfiguration &config= Configuration::Instance()->GetAntiSpamConfiguration();
         int iSomeScore = config.GetDKIMVerificationFailureScore();
         std::shared_ptr<SpamTestResult> pResult = std::shared_ptr<SpamTestResult>(new SpamTestResult(GetName(), SpamTestResult::Fail, iSomeScore, "Rejected by DKIM."));
         setSpamTestResults.insert(pResult);
      }
      else if (result == DKIM::Pass)
      {
         std::shared_ptr<SpamTestResult> pResult = std::shared_ptr<SpamTestResult>(new SpamTestResult(GetName(), SpamTestResult::Pass, 0, ""));
         setSpamTestResults.insert(pResult);
      }

      return setSpamTestResults;
   }

}