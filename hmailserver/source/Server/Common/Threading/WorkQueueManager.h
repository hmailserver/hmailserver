// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com
// Created 2005-07-21

#pragma once

#include "WorkQueue.h"

namespace HM
{
   class WorkQueue;
   class Task;

   class WorkQueueManager : public Singleton<WorkQueueManager>
   {
   public:
      WorkQueueManager(void);
      ~WorkQueueManager(void);

      size_t CreateWorkQueue(int iMaxSimultaneous, const String &sQueueName);
      void RemoveQueue(const String &sQueueName);

      void AddTask(size_t iQueueID, std::shared_ptr<Task> pTask);

      std::shared_ptr<WorkQueue> GetQueue(const String &sQueueName);

   private:

      std::map<size_t, std::shared_ptr<WorkQueue> >::iterator GetQueueIterator_(const String &sQueueName);

      boost::recursive_mutex mutex_;
      std::map<size_t, std::shared_ptr<WorkQueue> > work_queues_;
     
   };
}