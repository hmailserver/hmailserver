// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com
// Created 2005-07-21

#pragma once

#include "../Util/Event.h"

#include <boost/asio.hpp>
#include <boost/thread.hpp>

namespace HM
{
   class Task;
   class Thread;

   class WorkQueue
   {
   public:

      WorkQueue(unsigned int iMaxSimultaneous, const String &sName);
      ~WorkQueue(void);

      void SetMaxSimultaneous(int iMaxSimultaneous);

      void AddTask(std::shared_ptr<Task> pTask);
      void Start();

      void Stop();

      const String &GetName() const;

   private:

      void WorkQueue::RemoveRunningTask_(std::shared_ptr<Task> task);

      void IoServiceRunWorker();
      void ExecuteTask(std::shared_ptr<Task> pTask);

      boost::asio::io_service io_service_;
      boost::asio::io_service::work work_;

      std::set<std::shared_ptr<Task>> runningTasks_;
      boost::recursive_mutex runningTasksMutex_;

      unsigned int max_simultaneous_;

      String queue_name_;

      std::set<std::shared_ptr<boost::thread>> workerThreads_;
   };

}