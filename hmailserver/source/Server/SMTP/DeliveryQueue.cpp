// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com
// Created: 2005-08-02
// Purpose: To offer queue manipiulation to the COM API.

#include "StdAfx.h"

#include "DeliveryQueue.h"
#include "SMTPDeliveryManager.h"

#include "../Common/Threading/WorkQueueManager.h"
#include "../Common/BO/Messages.h"
#include "../Common/BO/Message.h"
#include "../Common/Persistence/PersistentMessage.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{

   bool DeliveryQueue::is_clearing_ = false;

   DeliveryQueueClearer::DeliveryQueueClearer(void) :
       Task("DeliveryQueueClearer")
   {
   }

   DeliveryQueueClearer::~DeliveryQueueClearer(void)
   {
   }

   void
   DeliveryQueueClearer::DoWork()
      //---------------------------------------------------------------------------()
      // DESCRIPTION:
      // This function does the actual clearing of the queue.
      //---------------------------------------------------------------------------()
   {
      try
      {
         LOG_DEBUG("Clearing delivery queue");

         // First stop the delivery queue.
         const String& sQueueName = Application::Instance()->GetSMTPDeliveryManager()->GetQueueName();
         shared_ptr<WorkQueue> pWQ = WorkQueueManager::Instance()->GetQueue(sQueueName);
         if (!pWQ)
         {
            ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4210, "DeliveryQueueClearer::DoWork", "Could not fetch SMTP delivery queue.");

            return;
         }
         
         pWQ->Stop();

         // Load the delivery queue from the database
         Messages oMessages(-1,-1);
         oMessages.Refresh();

         // Iterate over messages to deliver.
         std::vector<shared_ptr<Message> > vecMessages = oMessages.GetVector();
         std::vector<shared_ptr<Message> >::iterator iterMessage = vecMessages.begin();
         while (iterMessage != vecMessages.end())
         {
            // Delete the message from the database
            PersistentMessage::DeleteObject(*iterMessage);

            // Next message in queue
            iterMessage++;
         }

         // Tell the delivery queue to clear it's pending messages list.
         Application::Instance()->GetSMTPDeliveryManager()->UncachePendingMessages();

         // Make sure there doesn't exist any delivery tasks.
         pWQ->Start();

         DeliveryQueue::OnDeliveryQueueCleared();

         LOG_DEBUG("Delivery queue cleared.");

      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4209, "DeliveryQueueClearer::DoWork()", "An unknown error occurred while clearing the delivery queue.");
         throw;
      }

   }

   DeliveryQueue::DeliveryQueue(void)
   {
   }

   DeliveryQueue::~DeliveryQueue(void)
   {
   }

   void
   DeliveryQueue::Clear()
   {
      if (is_clearing_)
         return;

      is_clearing_ = true;

      // Use the random work queue to run the task.
      shared_ptr<WorkQueue> pQueue = Application::Instance()->GetMaintenanceWorkQueue();

      if (!pQueue)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5117, "DeliveryQueue::Clear()", "WorkQueue was not started. Queue could not be cleared.");

         is_clearing_ = false;
         return;
      }

      // Launch a thread that can clear the delivery queue.
      shared_ptr<DeliveryQueueClearer> pClearer = shared_ptr<DeliveryQueueClearer>(new DeliveryQueueClearer);

      pQueue->AddTask(pClearer);
   }

   void 
   DeliveryQueue::ResetDeliveryTime(__int64 iMessageID)
   {
      PersistentMessage::SetNextTryTime(iMessageID, false, -1);
   }


   void 
   DeliveryQueue::Remove(__int64 iMessageID)
   {
      shared_ptr<Message> pMessage = shared_ptr<Message>(new Message());
      if (!PersistentMessage::ReadObject(pMessage, iMessageID))
         return;

      // Lock the message before trying to delete it.
      if (!PersistentMessage::LockObject(pMessage))
         return;

      // Managed to lock it. Delete it now.
      PersistentMessage::DeleteObject(pMessage);
   }


   void 
   DeliveryQueue::OnDeliveryQueueCleared()
   {
      is_clearing_ = false;
   }

   void 
   DeliveryQueue::StartDelivery()
   {
      shared_ptr<SMTPDeliveryManager> pDeliverer = Application::Instance()->GetSMTPDeliveryManager();
      if (!pDeliverer)
         return;

      // Tell the deliverer to look for messages to deliver.
         pDeliverer->SetDeliverMessage();
   }
}