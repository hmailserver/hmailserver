// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "SMTPDeliveryManager.h"
#include "SMTPConfiguration.h"
#include "StatisticsSender.h"

#include "DeliveryTask.h"

#include "../common/Application/IniFileSettings.h"
#include "../common/Application/Property.h"
#include "../common/Util/ServerStatus.h"
#include "../common/Persistence/PersistentMessage.h"

#include "../common/Persistence/PersistentGreyList.h"
#include "../common/BO/Message.h"


#include "../common/Cache/MessageCache.h"

#include "../Common/Threading/WorkQueueManager.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   SMTPDeliveryManager::SMTPDeliveryManager() :
      m_sQueueName("SMTP delivery queue"),
      m_bUncachePendingMessages(false)
   {
      m_evtTimer = CreateWaitableTimer(NULL, FALSE, _T(""));

      m_lCurNumberOfSent = 0;

      int iMaxNumberOfThreads = Configuration::Instance()->GetSMTPConfiguration()->GetMaxNoOfDeliveryThreads();
      m_iQueueID = WorkQueueManager::Instance()->CreateWorkQueue(iMaxNumberOfThreads, m_sQueueName, WorkQueue::eQTPreLoad);
   }  

   SMTPDeliveryManager::~SMTPDeliveryManager()
   {
      CloseHandle(m_evtTimer);

      WorkQueueManager::Instance()->RemoveQueue(m_sQueueName);
   }

   void 
   SMTPDeliveryManager::SetDeliverMessage()
   {
      m_evtDeliverMessage.Set();
   }

   const String &
   SMTPDeliveryManager::GetQueueName() const
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // Return the name of the delivery queue.
   //---------------------------------------------------------------------------
   {
      return m_sQueueName;
   }

   void
   SMTPDeliveryManager::DoWork()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Responsible for creating threads to deliver messages.
   //---------------------------------------------------------------------------()
   {
      LOG_DEBUG("SMTPDeliveryManager::Start()");

      // Unlock all messages
      PersistentMessage::UnlockAll();

      shared_ptr<WorkQueue> pQueue = WorkQueueManager::Instance()->GetQueue(GetQueueName());

      while (1)
      {
         // Deliver all pending messages
         shared_ptr<Message> pMessage;
         while (pMessage = _GetNextMessage())
         {
            // Lock this message
            if (!PersistentMessage::LockObject(pMessage))
            {
               // Failed to lock the message.
               ErrorManager::Instance()->ReportError(ErrorManager::Critical, 4216, "SMTPDeliveryManager::DoWork", "Failed to lock message.");
               continue;
            }

            shared_ptr<DeliveryTask> pDeliveryTask = shared_ptr<DeliveryTask>(new DeliveryTask(pMessage));
            WorkQueueManager::Instance()->AddTask(m_iQueueID, pDeliveryTask);
            
            m_lCurNumberOfSent++;

            _SendStatistics();

            ServerStatus::Instance()->OnMessageProcessed();
         }

         _StartTimer();

         const int iSize = 3;
         HANDLE handles[iSize];

         handles[0] = m_hStopRequest.GetHandle();
         handles[1] = m_evtDeliverMessage.GetHandle();
         handles[2] = m_evtTimer;
      
         DWORD dwWaitResult = WaitForMultipleObjects(iSize, handles, FALSE, INFINITE);

         int iEvent = dwWaitResult - WAIT_OBJECT_0;
      
         switch (iEvent)
         {
         case 0:
            // We should stop now
            _SendStatistics(true);
            return;
         case 1:
            // --- Reset the event to give someone else the chance to 
            //     sending emails.
            m_evtDeliverMessage.Reset();
            break;
         }

      }

      _SendStatistics(true);
      
      
      return;
   }

   void
   SMTPDeliveryManager::_SendStatistics(bool bIgnoreMessageCount)
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // Sends statistics to hMailServer.com
   //---------------------------------------------------------------------------
   {
      if (m_lCurNumberOfSent > 1000 || (bIgnoreMessageCount && m_lCurNumberOfSent > 5))
      {
         if (Configuration::Instance()->GetSendStatistics())
         {
            // Send statistics to server.
            StatisticsSender Sender;
            
            Sender.SendStatistics(m_lCurNumberOfSent);

            // Always reset the number. If the statistics sender can't connect
            // to hMailServer for some reason, we should not try immediately again
            // and instead just give up.
            m_lCurNumberOfSent = 0;
         }
         else
            m_lCurNumberOfSent = 0;
      }

   }

   void
   SMTPDeliveryManager::_LoadPendingMessageList()
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // Loads a list of messages that should be delivered from the database
   //---------------------------------------------------------------------------
   {
      HM::DatabaseSettings::SQLDBType DBType = IniFileSettings::Instance()->GetDatabaseType();

      // Tweak to prioritize small & newer emails in queue delivery order
      // Assumption is that large emails will take longer to deliver tying up queue
      // vs just getting rid of smaller emails 1st and messages with high # of
      // tries are not likely to go anyway so why tie up newer emails in queue
      String sql = Formatter::Format("select * from hm_messages where messagetype = 1 and messagelocked = 0 and messagenexttrytime <= {0} order by messagesize, messagecurnooftries, messageid asc", SQLStatement::GetCurrentTimestamp());
      // String sql = Formatter::Format("select * from hm_messages where messagetype = 1 and messagelocked = 0 and messagenexttrytime <= {0} order by messageid asc", SQLStatement::GetCurrentTimestamp());
      SQLCommand command(sql);

      m_pPendingMessages = Application::Instance()->GetDBManager()->OpenRecordset(command);
   }

   shared_ptr<Message>
   SMTPDeliveryManager::_GetNextMessage()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Retrives the first unlcoked message from the database and tries to lock it.
   // 
   // 0 is returned if no messages exists.
   //---------------------------------------------------------------------------()
   {
      if (!m_pPendingMessages || m_pPendingMessages->IsEOF() || m_bUncachePendingMessages)
      {
         m_bUncachePendingMessages = false;

         _LoadPendingMessageList();
      }

      shared_ptr<Message> pRetMessage;
      if (!m_pPendingMessages || m_pPendingMessages->IsEOF())
         return pRetMessage;
      
      // Fetch the ID of the first message
      __int64 iMessageID = m_pPendingMessages->GetInt64Value("messageid");

      // Try to read this message from the message cache. Might fail.
      pRetMessage = MessageCache::Instance()->GetMessage(iMessageID);
         
      if (!pRetMessage) 
      {
         // Message was not found in cache. Read from database. Will
         // require 1 extra statement towards the database, since we
         // need to read recipients 
         pRetMessage = shared_ptr<Message> (new Message(false));
         PersistentMessage::ReadObject(m_pPendingMessages, pRetMessage);
      }

      // Move to the next message in the cache.
      m_pPendingMessages->MoveNext();

      return pRetMessage;
   }


   void
   SMTPDeliveryManager::_StartTimer() 
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // Create a timer that sets the event in one minut.
   //---------------------------------------------------------------------------
   {

      LARGE_INTEGER liDueTime;
      liDueTime.QuadPart=-600000000; // every minute!

      // Set the timer.
      BOOL bResult = SetWaitableTimer(m_evtTimer, &liDueTime, NULL, NULL, NULL, FALSE);

      if (bResult == 0)
      {
         assert(0); // error
      }
   }
  
   void 
   SMTPDeliveryManager::StopWork()
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // The thread should be stopped.
   //---------------------------------------------------------------------------
   {
      // We should exit DoWork
      m_hStopRequest.Set();
   }

   void 
   SMTPDeliveryManager::UncachePendingMessages()
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // Tells the delivery manager to uncache it's list of messages. This is 
   // normally only required if the delivery queue should be clear
   //---------------------------------------------------------------------------
   {
      m_bUncachePendingMessages = true;
   }

   void
   SMTPDeliveryManager::OnPropertyChanged(shared_ptr<Property> pProperty)
   {
      String sPropertyName = pProperty->GetName();
      
      if (sPropertyName == PROPERTY_MAXDELIVERYTHREADS)
      {
         shared_ptr<WorkQueue> pWorkQueue = WorkQueueManager::Instance()->GetQueue(GetQueueName());
         
         if (!pWorkQueue)
            return;

         pWorkQueue->SetMaxSimultaneous(pProperty->GetLongValue());
      }
   }

}
