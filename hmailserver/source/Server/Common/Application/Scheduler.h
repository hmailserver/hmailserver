// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../Threading/Task.h"
#include "../Util/Event.h"

namespace HM
{
   class ScheduledTask;

   class Scheduler : public Task
   {
   public:
      Scheduler();
      virtual ~Scheduler();


      void ScheduleTask(std::shared_ptr<ScheduledTask> pTask);

      void DoWork();

   private:

      void RunTasks_();

      boost::recursive_mutex mutex_;
      std::vector<std::shared_ptr<ScheduledTask >> scheduled_tasks_;

   };

}
