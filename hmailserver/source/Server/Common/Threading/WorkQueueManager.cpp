// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com
// Created 2005-07-21

#include "StdAfx.h"

#include "WorkQueueManager.h"


#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   WorkQueueManager::WorkQueueManager(void)
   {
   }

   WorkQueueManager::~WorkQueueManager(void)
   {
   }

   int 
   WorkQueueManager::CreateWorkQueue(int iMaxSimultaneous, const String &sQueueName, WorkQueue::QueueType qtType)
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // Creates a new work queue and adds it to the list of queues. Returns the ID
   // of the new queue.
   //---------------------------------------------------------------------------
   {
      // Create the work queue
      shared_ptr<WorkQueue> pWorkQueue = shared_ptr<WorkQueue>(new WorkQueue(iMaxSimultaneous, qtType, sQueueName));
      pWorkQueue->Start();

      CriticalSectionScope scope(m_csWorkQueues);
      int iQueueID = m_mapWorkQueues.size() + 1;
      m_mapWorkQueues[iQueueID] = pWorkQueue;

      return iQueueID;
   }

   void 
   WorkQueueManager::AddTask(int iQueueID, shared_ptr<Task> pTask)
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // Adds a task to a worker queue.
   //---------------------------------------------------------------------------
   {
      // Add the task to the work queue
      CriticalSectionScope scope(m_csWorkQueues);

      std::map<int, shared_ptr<WorkQueue> >::iterator iterQueue = m_mapWorkQueues.find(iQueueID);

      if (iterQueue == m_mapWorkQueues.end())
      {
         // Someone is trying to add a task to a
         // queue that does not exist.
         assert(0);  
      }

      shared_ptr<WorkQueue> pWorkQueue = (*iterQueue).second;

      pWorkQueue->AddTask(pTask);
   }

   void 
   WorkQueueManager::RemoveQueue(const String &sQueueName)
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // Stops and removes a queue
   //---------------------------------------------------------------------------
   {
      // Locate the work queue
      shared_ptr<WorkQueue> pQueue;
      std::map<int, shared_ptr<WorkQueue> >::iterator iterQueue;

      {
         CriticalSectionScope scope(m_csWorkQueues);

         iterQueue = _GetQueueIterator(sQueueName);
         if (iterQueue == m_mapWorkQueues.end())
            return;

         pQueue = (*iterQueue).second;
         if (!pQueue)
            return;
      }

      // This queue should be stopped
      int iNumberOfStopAttempts = 10;
      for (int i = 0; i < iNumberOfStopAttempts; i++)
      {
         // If it's the last stop attempt, it's no longer
         // a warning and we should just terminate the thread.
         bool bPreKillWarning = true;
         if (i == iNumberOfStopAttempts-1)
            bPreKillWarning = false;

         // Issue the warning or the termination.
         pQueue->Stop(bPreKillWarning);

         if (!pQueue->GetRunningThreadsExists())      
         {
            // Wait for the queue to be stopped.
            WaitForSingleObject(pQueue->GetQueueThreadHandle(), 3000);

            CriticalSectionScope scope(m_csWorkQueues);
            m_mapWorkQueues.erase(iterQueue);

            return;
         }

         // The queue hasn't stoped just yet. 
         // Wait a few more seconds.
         Sleep(250);

      }

      
      CriticalSectionScope workQueueScope(m_csWorkQueues);
      m_mapWorkQueues.erase(iterQueue);

   }

   shared_ptr<WorkQueue> 
   WorkQueueManager::GetQueue(const String &sQueueName)
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // Returns the queue with a specific name. 
   //---------------------------------------------------------------------------
   {
      CriticalSectionScope scope(m_csWorkQueues);

      std::map<int, shared_ptr<WorkQueue> >::iterator iterQueue = _GetQueueIterator(sQueueName);
      if (iterQueue != m_mapWorkQueues.end())
      {
         shared_ptr<WorkQueue> pQueue = (*iterQueue).second;
         if (pQueue->GetName().CompareNoCase(sQueueName) == 0)
            return pQueue;

         iterQueue++;
      }

      shared_ptr<WorkQueue> pEmpty;
      return pEmpty;
         
   }

   std::map<int, shared_ptr<WorkQueue> >::iterator 
   WorkQueueManager::_GetQueueIterator(const String &sQueueName)
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // Returns a iterator to a queue with the specified name.
   //---------------------------------------------------------------------------
   {
      std::map<int, shared_ptr<WorkQueue> >::iterator iterQueue = m_mapWorkQueues.begin();
      while (iterQueue != m_mapWorkQueues.end())
      {
         shared_ptr<WorkQueue> pQueue = (*iterQueue).second;
         if (pQueue->GetName().CompareNoCase(sQueueName) == 0)
            return iterQueue;

         iterQueue++;
      }

      return m_mapWorkQueues.end();
   }
}