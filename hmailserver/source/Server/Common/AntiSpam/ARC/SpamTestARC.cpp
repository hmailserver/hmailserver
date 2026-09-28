// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "stdafx.h"

#include "SpamTestARC.h"

#include "../AntiSpamConfiguration.h"
#include "../SenderAuthentication.h"
#include "../SpamTestData.h"
#include "../SpamTestResult.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   String
   SpamTestARC::GetName() const
   {
      return "SpamTestARC";
   }

   bool
   SpamTestARC::GetIsEnabled()
   {
      return Configuration::Instance()->GetAntiSpamConfiguration().GetARCEnabled();
   }

   std::set<std::shared_ptr<SpamTestResult> >
   SpamTestARC::RunTest(std::shared_ptr<SpamTestData> pTestData)
   {
      // No-op if DMARC has already evaluated the chain for this message.
      pTestData->GetSenderAuthentication()->EvaluateARC(pTestData);

      return std::set<std::shared_ptr<SpamTestResult> >();
   }
}
