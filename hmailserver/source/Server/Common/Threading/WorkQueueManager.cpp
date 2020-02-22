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

   size_t 
   WorkQueueManager::CreateWorkQueue(int iMaxSimultaneous, const String &sQueueName)
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // Creates a new work queue and adds it to the list of queues. Returns the ID
   // of the new queue.
   //---------------------------------------------------------------------------
   {
      // Create the work queue
      std::shared_ptr<WorkQueue> pWorkQueue = std::shared_ptr<WorkQueue>(new WorkQueue(iMaxSimultaneous, sQueueName));
      pWorkQueue->Start();

      boost::lock_guard<boost::recursive_mutex> guard(mutex_);
      size_t iQueueID = work_queues_.size() + 1;
      work_queues_[iQueueID] = pWorkQueue;

      return iQueueID;
   }

   void 
   WorkQueueManager::AddTask(size_t iQueueID, std::shared_ptr<Task> pTask)
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // Adds a task to a worker queue.
   //---------------------------------------------------------------------------
   {
      // Add the task to the work queue
      boost::lock_guard<boost::recursive_mutex> guard(mutex_);

      auto iterQueue = work_queues_.find(iQueueID);

      if (iterQueue == work_queues_.end())
      {
         // Someone is trying to add a task to a
         // queue that does not exist.
         assert(0);  
      }

      std::shared_ptr<WorkQueue> pWorkQueue = (*iterQueue).second;

      pWorkQueue->AddTask(pTask);
   }

   void 
   WorkQueueManager::RemoveQueue(const String &sQueueName)
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // Stops and removes a queue
   //---------------------------------------------------------------------------
   {
      LOG_DEBUG(Formatter::Format("WorkQueueManager::RemoveQueue - {0}", sQueueName));

      // Locate the work queue
      std::shared_ptr<WorkQueue> pQueue;
      std::map<size_t, std::shared_ptr<WorkQueue> >::iterator iterQueue;

      {
         boost::lock_guard<boost::recursive_mutex> guard(mutex_);

         iterQueue = GetQueueIterator_(sQueueName);
         if (iterQueue == work_queues_.end())
         {
            LOG_DEBUG(Formatter::Format("WorkQueueManager::RemoveQueue - Work quue not found {0}", sQueueName));
            return;
         }

         pQueue = (*iterQueue).second;
         if (!pQueue)
         {
            LOG_DEBUG(Formatter::Format("WorkQueueManager::RemoveQueue - Work quue not found {0}", sQueueName));
            return;
         }
      }

      pQueue->Stop();

      LOG_DEBUG(Formatter::Format("WorkQueueManager::RemoveQueue - Stopped {0}", sQueueName));
      
      boost::lock_guard<boost::recursive_mutex> guard(mutex_);
      work_queues_.erase(iterQueue);

      LOG_DEBUG(Formatter::Format("WorkQueueManager::RemoveQueue - Erased {0}", sQueueName));

   }

   std::shared_ptr<WorkQueue> 
   WorkQueueManager::GetQueue(const String &sQueueName)
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // Returns the queue with a specific name. 
   //---------------------------------------------------------------------------
   {
      boost::lock_guard<boost::recursive_mutex> guard(mutex_);

      auto iterQueue = GetQueueIterator_(sQueueName);
      if (iterQueue != work_queues_.end())
      {
         std::shared_ptr<WorkQueue> pQueue = (*iterQueue).second;
         if (pQueue->GetName().CompareNoCase(sQueueName) == 0)
            return pQueue;

         iterQueue++;
      }

      std::shared_ptr<WorkQueue> pEmpty;
      return pEmpty;
         
   }

   std::map<size_t, std::shared_ptr<WorkQueue> >::iterator
   WorkQueueManager::GetQueueIterator_(const String &sQueueName)
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // Returns a iterator to a queue with the specified name.
   //---------------------------------------------------------------------------
   {
      auto iterQueue = work_queues_.begin();
      while (iterQueue != work_queues_.end())
      {
         std::shared_ptr<WorkQueue> pQueue = (*iterQueue).second;
         if (pQueue->GetName().CompareNoCase(sQueueName) == 0)
            return iterQueue;

         iterQueue++;
      }

      return work_queues_.end();
   }
}