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
   WorkQueueManager::CreateWorkQueue(int iMaxSimultaneous, const String &sQueueName)
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // Creates a new work queue and adds it to the list of queues. Returns the ID
   // of the new queue.
   //---------------------------------------------------------------------------
   {
      // Create the work queue
      shared_ptr<WorkQueue> pWorkQueue = shared_ptr<WorkQueue>(new WorkQueue(iMaxSimultaneous, sQueueName));
      pWorkQueue->Start();

      boost::lock_guard<boost::recursive_mutex> guard(_mutex);
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
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

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
      LOG_DEBUG(Formatter::Format("WorkQueueManager::RemoveQueue - {0}", sQueueName));

      // Locate the work queue
      shared_ptr<WorkQueue> pQueue;
      std::map<int, shared_ptr<WorkQueue> >::iterator iterQueue;

      {
         boost::lock_guard<boost::recursive_mutex> guard(_mutex);

         iterQueue = _GetQueueIterator(sQueueName);
         if (iterQueue == m_mapWorkQueues.end())
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
      
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);
      m_mapWorkQueues.erase(iterQueue);

      LOG_DEBUG(Formatter::Format("WorkQueueManager::RemoveQueue - Erased {0}", sQueueName));

   }

   shared_ptr<WorkQueue> 
   WorkQueueManager::GetQueue(const String &sQueueName)
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // Returns the queue with a specific name. 
   //---------------------------------------------------------------------------
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

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