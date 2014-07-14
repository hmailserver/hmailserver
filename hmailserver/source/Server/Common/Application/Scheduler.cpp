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
         _RunTasks();
         
         // Wait one minute.
         try
         {
            boost::this_thread::sleep_for(chrono::minutes(1));
         }
         catch (thread_interrupted const&)
         {
            boost::this_thread::disable_interruption disabled;

            boost::lock_guard<boost::recursive_mutex> guard(_mutex);
            m_vecScheduledTasks.clear();

            return;
         }
      }
   }

   void
   Scheduler::_RunTasks()
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      vector<shared_ptr<ScheduledTask >>::iterator iterTask;

      for (iterTask = m_vecScheduledTasks.begin(); iterTask != m_vecScheduledTasks.end(); iterTask++)
      {
         shared_ptr<ScheduledTask > pTask = (*iterTask);

         // Check if we should run this task now.
         DateTime dtRunTime = pTask->GetNextRunTime();
         DateTime dtNow = DateTime::GetCurrentTime();

         if (dtRunTime <= dtNow)
         {
            // Yup, we should run this task now.
            pTask->DoWork();
            
            // Update run time.
            pTask->SetNextRunTime();
         }
      }
   }  

   void
   Scheduler::ScheduleTask(shared_ptr<ScheduledTask> pTask)
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      // If task should only be run once, run it now.
      if (pTask->GetReoccurance() == ScheduledTask::RunOnce)
      {
         Application::Instance()->GetMaintenanceWorkQueue()->AddTask(pTask);
         return;
      }

      m_vecScheduledTasks.push_back(pTask);
     
   }


}
