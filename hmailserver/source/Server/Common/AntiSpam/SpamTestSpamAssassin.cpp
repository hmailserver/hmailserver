// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "SpamTestSpamAssassin.h"

#include "SpamTestData.h"
#include "SpamTestResult.h"

#include "AntiSpamConfiguration.h"

#include "SpamAssassin/SpamAssassinClient.h"

#include "../TCPIP/IOCPServer.h"
#include "../TCPIP/TCPConnection.h"
#include "../BO/MessageData.h"
#include "../BO/Message.h"
#include "../Util/event.h"
#include "../Persistence/PersistentMessage.h"


#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   String 
   SpamTestSpamAssassin::GetName() const
   {
      return "SpamTestSpamAssassin";
   }

   bool 
   SpamTestSpamAssassin::GetIsEnabled()
   {
      AntiSpamConfiguration &config = Configuration::Instance()->GetAntiSpamConfiguration();
      if (config.GetSpamAssassinEnabled())
         return true;
      else
         return false;
   }

   set<shared_ptr<SpamTestResult> >
   SpamTestSpamAssassin::RunTest(shared_ptr<SpamTestData> pTestData)
   {
      set<shared_ptr<SpamTestResult> > setSpamTestResults;
      
      AntiSpamConfiguration& config = Configuration::Instance()->GetAntiSpamConfiguration();

      shared_ptr<Message> pMessage = pTestData->GetMessageData()->GetMessage();
      const String sFilename = PersistentMessage::GetFileName(pMessage);
      
      shared_ptr<IOCPServer> pIOCPServer = Application::Instance()->GetIOCPServer();
      boost::asio::ssl::context ctx(pIOCPServer->GetIOService(), boost::asio::ssl::context::sslv23);

      String message;
      bool testCompleted;

      shared_ptr<Event> disconnectEvent = shared_ptr<Event>(new Event());
      shared_ptr<SpamAssassinClient> pSAClient = shared_ptr<SpamAssassinClient>(new SpamAssassinClient(sFilename, pIOCPServer->GetIOService(), ctx, disconnectEvent, message, testCompleted));
      
      String sHost = config.GetSpamAssassinHost();
      int iPort = config.GetSpamAssassinPort();

      // Here we handle of the ownership to the TCPIP-connection layer.
      if (pSAClient->Connect(sHost, iPort, IPAddress()))
      {
         // Make sure we keep no references to the TCP connection so that it
         // can be terminated whenever. We're longer own the connection.
         pSAClient.reset();

         disconnectEvent->Wait();
      }
     
      // Check if the message is tagged as spam.
      shared_ptr<MessageData> pMessageData = pTestData->GetMessageData();
      pMessageData->RefreshFromMessage();

      bool bIsSpam = false;
      AnsiString sSpamStatus = pMessageData->GetFieldValue("X-Spam-Status");
      if (sSpamStatus.Mid(0, 3).ToUpper() == "YES")
         bIsSpam = true;

      if (bIsSpam)
      {
         int iScore = 0;
         if (config.GetSpamAssassinMergeScore())
            iScore = _ParseSpamAssassinScore(sSpamStatus);
         else
            iScore = config.GetSpamAssassinScore();

         String sMessage = "Tagged as Spam by SpamAssassin";
         shared_ptr<SpamTestResult> pResult = shared_ptr<SpamTestResult>(new SpamTestResult(GetName(), SpamTestResult::Fail, iScore, sMessage));
         setSpamTestResults.insert(pResult);   
      }
      
      return setSpamTestResults;
   }

   int 
   SpamTestSpamAssassin::_ParseSpamAssassinScore(const AnsiString &sHeader)
   {
      int iStartPos = sHeader.FindNoCase("score=");
      if (iStartPos < 0)
         return 0;

      iStartPos += 6;

      int iScoreEnd = sHeader.Find(".", iStartPos);
      if (iScoreEnd < 0)
         return 0;

      int iScoreLength = iScoreEnd - iStartPos;

      if (iScoreLength <= 0)
         return 0;

      AnsiString sScore = sHeader.Mid(iStartPos, iScoreLength);

      int iRetVal = atoi(sScore);
      return iRetVal;

   }




}