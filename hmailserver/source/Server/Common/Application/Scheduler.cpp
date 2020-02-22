// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "Scheduler.h"

#include "../Threading/WorkQueue.h"

#include "../BO/ScheduledTask.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   Scheduler::Scheduler()
      : Task("Scheduler")
   {  
       
   }

   Scheduler::~Scheduler()
   {

   }

   void 
   Scheduler::DoWork()
   {
      SetIsStarted();

      while (true)
      {
         // Check if there are any tasks to run.
         RunTasks_();
         
         // Wait one minute.
         try
         {
            boost::this_thread::sleep_for(boost::chrono::minutes(1));
         }
         catch (const boost::thread_interrupted&)
         {
            boost::this_thread::disable_interruption disabled;

            boost::lock_guard<boost::recursive_mutex> guard(mutex_);
            scheduled_tasks_.clear();

            return;
         }
      }
   }

   void
   Scheduler::RunTasks_()
   {
      boost::lock_guard<boost::recursive_mutex> guard(mutex_);

      for (auto iterTask = scheduled_tasks_.begin(); iterTask != scheduled_tasks_.end(); iterTask++)
      {
         std::shared_ptr<ScheduledTask > pTask = (*iterTask);

         // Check if we should run this task now.
         DateTime dtRunTime = pTask->GetNextRunTime();
         DateTime dtNow = DateTime::GetCurrentTime();

         if (dtRunTime <= dtNow)
         {
            // Yup, we should run this task now.
            pTask->Run();
            
            // Update run time.
            pTask->SetNextRunTime();
         }
      }
   }  

   void
   Scheduler::ScheduleTask(std::shared_ptr<ScheduledTask> pTask)
   {
      boost::lock_guard<boost::recursive_mutex> guard(mutex_);

      // If task should only be run once, run it now.
      if (pTask->GetReoccurance() == ScheduledTask::RunOnce)
      {
         Application::Instance()->GetMaintenanceWorkQueue()->AddTask(pTask);
         return;
      }

      scheduled_tasks_.push_back(pTask);
     
   }


}
