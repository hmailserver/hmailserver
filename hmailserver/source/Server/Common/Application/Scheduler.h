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


      void ScheduleTask(shared_ptr<ScheduledTask> pTask);

      void DoWork();
      void StopWork();

   private:

      void _RunTasks();

      Event m_hStopTask;      

      CriticalSection m_oVecCritSec;
      vector<shared_ptr<ScheduledTask >> m_vecScheduledTasks;

   };

}
