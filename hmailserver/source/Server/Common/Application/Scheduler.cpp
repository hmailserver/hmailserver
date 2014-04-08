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
   {  
       
   }

   Scheduler::~Scheduler()
   {

   }

   void 
   Scheduler::DoWork()
   {
      while (true)
      {
         // Check if there are any tasks to run.
         _RunTasks();
         
         // Wait one minute.
         DWORD dwWaitResult = WaitForSingleObject(m_hStopTask.GetHandle(), 60000);

         if (dwWaitResult == WAIT_OBJECT_0)
         {
            CriticalSectionScope scope (m_oVecCritSec);

            m_vecScheduledTasks.clear();

            m_hStopTask.Reset();
            return;
         }
      }
   }

   void
   Scheduler::_RunTasks()
   {
      CriticalSectionScope scope(m_oVecCritSec); 

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
   Scheduler::StopWork()
   {
      m_hStopTask.Set();
   }

   void
   Scheduler::ScheduleTask(shared_ptr<ScheduledTask> pTask)
   {
      CriticalSectionScope scope(m_oVecCritSec);

      // If task should only be run once, run it now.
      if (pTask->GetReoccurance() == ScheduledTask::RunOnce)
      {
         Application::Instance()->GetRandomWorkQueue()->AddTask(pTask);
         return;
      }

      m_vecScheduledTasks.push_back(pTask);
     
   }


}
