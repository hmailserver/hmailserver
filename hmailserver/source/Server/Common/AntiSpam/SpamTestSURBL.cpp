// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "SpamTestSURBL.h"

#include "SpamTestData.h"
#include "SpamTestResult.h"

#include "SURBL.h"

#include "../BO/SURBLServers.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   String 
   SpamTestSURBL::GetName() const
   {
      return "SpamTestSURBL";
   }

   bool 
   SpamTestSURBL::GetIsEnabled()
   {
      std::shared_ptr<SURBLServers> pSURBLServers = Configuration::Instance()->GetAntiSpamConfiguration().GetSURBLServers();

      std::vector<std::shared_ptr<SURBLServer> > vec = pSURBLServers->GetVector();
      auto iter = vec.begin();
      auto iterEnd = vec.end();

      for (; iter != iterEnd; iter++)
      {
         std::shared_ptr<SURBLServer> pSURBLServer = (*iter);

         if (pSURBLServer->GetIsActive()) 
         {
            return true;
         }
      }

      return false;
   }

   std::set<std::shared_ptr<SpamTestResult> >
   SpamTestSURBL::RunTest(std::shared_ptr<SpamTestData> pTestData)
   {
      std::set<std::shared_ptr<SpamTestResult> > setSpamTestResults;

      std::shared_ptr<MessageData> pMessageData = pTestData->GetMessageData();
      std::shared_ptr<SURBLServers> pSURBLServers = Configuration::Instance()->GetAntiSpamConfiguration().GetSURBLServers();

      SURBL surblTester;

      for(std::shared_ptr<SURBLServer> pSURBLServer : pSURBLServers->GetVector())
      {
         if (pSURBLServer->GetIsActive()) 
         {
            if (!surblTester.Run(pSURBLServer, pMessageData))
            {
               // Blocked
               int iSomeScore = pSURBLServer->GetScore();
               std::shared_ptr<SpamTestResult> pResult = std::shared_ptr<SpamTestResult>(new SpamTestResult(GetName(), SpamTestResult::Fail, iSomeScore, pSURBLServer->GetRejectMessage()));

               setSpamTestResults.insert(pResult);
            }
         }

      }      


      return setSpamTestResults;
   }

}