// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com
// Created 2005-07-21

#include "StdAfx.h"

#include "WorkQueue.h"
#include "Thread.h"
#include "Task.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   DWORD WINAPI _StartThread(LPVOID vd)
   {
      HM::WorkQueue *pWQ = static_cast<HM::WorkQueue*>(vd);
      pWQ->ThreadFunc();
      return 0;

   }

   WorkQueue::WorkQueue(unsigned int iMaxSimultaneous, QueueType qtType , const String &sQueueName) :
      m_sQueueName (sQueueName),
      m_bPause (false),
      m_qtType(qtType),
      m_iMaxSimultaneous(0),
      m_iBaseLineThreadCount(0)
   {
      SetMaxSimultaneous(iMaxSimultaneous);

      m_hThread = 0;
   }

   void 
   WorkQueue::SetMaxSimultaneous(int iMaxSimultaneous)
   {
      m_iMaxSimultaneous = iMaxSimultaneous;
      m_iBaseLineThreadCount = 0;

      if (m_qtType == eQTFixedSize)
      {
         // A fixed number of threads should
         // be run.
         m_iBaseLineThreadCount = iMaxSimultaneous;
      }
      else if (m_qtType == eQTPreLoad)
      {
         // Estimate number of required threads
         if (m_iMaxSimultaneous == 0)
            m_iBaseLineThreadCount = 5;
         else
         {
            // Calculate number of threads to pre-create
            m_iBaseLineThreadCount = m_iMaxSimultaneous / 3;
            m_iBaseLineThreadCount = max(m_iBaseLineThreadCount, 10);
            m_iBaseLineThreadCount = min(m_iBaseLineThreadCount, 50);

            // Make sure it's never more than the max defined.
            m_iBaseLineThreadCount = min(m_iBaseLineThreadCount, m_iMaxSimultaneous);

         }
      }
      else if (m_qtType == eQTRandom)
      {
         if (m_iMaxSimultaneous == 0)
         {
            // If no maximum simultaneous threads have been specified, fall back to 100.
            m_iMaxSimultaneous = 100;
         }
      }

      // Hard code limit to 100. Everything over this won't be good for stability.
      if (m_iMaxSimultaneous > 100)
         m_iMaxSimultaneous = 100;


   }


   WorkQueue::~WorkQueue(void)
   {
      if (m_hThread != 0)
         CloseHandle(m_hThread);
   }

   void 
   WorkQueue::AddTask(shared_ptr<Task> pTask)
   {
      CriticalSectionScope scope(m_csPendingTasks);
      m_qPendingTasks.push(pTask);

      m_hCheckForTask.Set();
   }

   void 
   WorkQueue::Start()
   {
      // Should we start worker threads now?
      if (m_qtType == eQTPreLoad || 
          m_qtType == eQTFixedSize)
      {
         CriticalSectionScope scope(m_csThreads);
         for (unsigned int i = 0; i < m_iBaseLineThreadCount; i++)
         {
            shared_ptr<Thread> pThread = shared_ptr<Thread>(new Thread(this));
            pThread->Start();
            m_mapThreads[pThread->GetHandle()] = pThread;
         }
      }

      // Start thread that should delegate tasks to queues
      m_hThread = CreateThread(NULL, 0, _StartThread, this, 0, 0);
   }

   void 
   WorkQueue::Stop(bool bPreKillWarning)
   {
      try
      {
         // First make sure no new threads are started
         m_hStopQueue.Set();

         // Stop all running threads
         CriticalSectionScope scope(m_csThreads);

         // Create a copy of the map. Since pThread->Stop() may cause threads
         // being removed from the map, it's important that we work on a copy
         // of it.
         
         map<HANDLE, shared_ptr<Thread> > mapThreads = m_mapThreads;
         map<HANDLE, shared_ptr<Thread> >::iterator iterThread = mapThreads.begin();

         while (iterThread != mapThreads.end())
         {
            // Tell thread to stop.
            shared_ptr<Thread> pThread = (*iterThread).second;
            pThread->Stop(bPreKillWarning);

            iterThread++;
         }

      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4213, "WorkQueue::Stop", "An unknown error occurred while stopping queue.");
         throw;
      }

   }

   bool
   WorkQueue::GetRunningThreadsExists()
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // Returns true if any running threads exists. False otherwise.
   //---------------------------------------------------------------------------
   {
      CriticalSectionScope scope(m_csThreads);

      return !m_mapThreads.empty();
   }

   bool 
   WorkQueue::OnThreadReady(Thread *pThread)
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // Called by a thread when it's ready for a new task. If this function
   // returns false, the thread will stop.
   //---------------------------------------------------------------------------
   {
      bool result = false;

      int stage = 0;

      try
      {
         stage = 1;   

         // Check if we should stop this thread.
         if (m_qtType == eQTRandom)
         {
            stage = 2;
            // Threads should always be terminated
            // when the queue is of type random.
            result = false;
            stage = 3;
         }
         else
         {
            stage = 4;
            CriticalSectionScope scope(m_csThreads);
            stage = 5;

            if (m_mapThreads.size() <= m_iBaseLineThreadCount)
            {
               stage = 6;
               // Switch back to ready state so that 
               // other task can use this thread. This must be
               // done after the WorkQueue has decided that
               // the thread should continue to run.
               // 
               pThread->SetStateReady();
               stage = 7;
               result = true;
               stage = 8;
            }
            else
            {
               stage = 9;
               result = false;
            }

            stage = 10;
         }

         // The thread is ready. Signal that this queue may check
         // for new tasks to execute. This may cause pThread to start 
         // a new task immediately unless we're terminating it now.
         stage = 11;
         m_hCheckForTask.Set();
         stage = 12;
      }
      catch (...)
      {
         String error = Formatter::Format("Failed to mark thread as ready. Stage: {0}", stage);
         ErrorManager::Instance()->ReportError(ErrorManager::High, 4211, "WorkQueue::OnThreadReady", error);
         throw;
      }

      return result;
   }

   void
   WorkQueue::OnThreadExited(Thread *pThread)
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // The thread has exited. Do clean up
   //---------------------------------------------------------------------------
   {
      // The thread has exited running state. It should
      // be removed from our list of threads.
      CriticalSectionScope scope(m_csThreads);

      // Close the thread handle and remove it
      // from our list. The thread is dead.
      HANDLE hThread = pThread->GetHandle();
      pThread->CloseThread();

      map<HANDLE, shared_ptr<Thread> >::iterator iterThread = m_mapThreads.find(hThread);
      if (iterThread != m_mapThreads.end())
         m_mapThreads.erase(iterThread);
   }


   void 
   WorkQueue::ThreadFunc()
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // This function is responsible for handing out tasks to threads. It sits and
   // waits until a task arrives, and then tries to delegate the task to one of
   // its threads.
   //---------------------------------------------------------------------------
   {
      try
      {
         while (1)
         {
            // Wait for a new task that should be run.
            int iSize = 2;
            HANDLE handles[2];
            handles[0] = m_hCheckForTask.GetHandle();
            handles[1] = m_hStopQueue.GetHandle();

            // Wait for a new task, a thread to be freed or for us to stop.
            DWORD dwWaitResult = WaitForMultipleObjects(iSize, handles, FALSE, INFINITE);

            int iEventIndex = dwWaitResult - WAIT_OBJECT_0;
            switch (iEventIndex)
            {
            case 0:
               m_hCheckForTask.Reset();
               break;
            case 1:
               m_hStopQueue.Reset();
               return;
            }

            CriticalSectionScope scopePendingTasks(m_csPendingTasks);

            if (m_bPause || m_qPendingTasks.size() == 0)
            {
               // We're in a pause, or there's nothing
               // to do at the moment
               continue;
            }

            // We have got something to do. Try to 
            // find a free thread for the task.

            CriticalSectionScope scopeThreads(m_csThreads);

            while (!m_qPendingTasks.empty())
            {
               map<HANDLE, shared_ptr<Thread> >::iterator iterThread = m_mapThreads.begin();
               shared_ptr<Thread> pThread;
               while (iterThread != m_mapThreads.end())
               {
                  if ((*iterThread).second->GetState() == Thread::Ready)
                  {
                     // The thread is free.
                     pThread = (*iterThread).second;
                     break;
                  }
                  iterThread++;
               }
               
               if (!pThread)
               {
                  // No free thread found.
                  if (m_iMaxSimultaneous > 0 && m_mapThreads.size() >= m_iMaxSimultaneous)
                     break;

                  // Create a new thread.
                  pThread = shared_ptr<Thread>(new Thread(this));
                  pThread->Start();

                  m_mapThreads[pThread->GetHandle()] = pThread;
               }

               CriticalSectionScope scope (m_csPendingTasks);
               // Pick the next task from the queue
               shared_ptr<Task> pTask = m_qPendingTasks.front();

               // Remove task from pending queue
               m_qPendingTasks.pop();
   
               pThread->AssignTask(pTask);
            }

            // No free thread found.
            if (m_mapThreads.size() >= m_iMaxSimultaneous)
               continue;
         }
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 4211, "WorkQueue::ThreadFunc", "An unknown error occurred while running thread.");
      }
   }

   int 
   WorkQueue::GetQueueSize()
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // Thread safe
   //---------------------------------------------------------------------------
   {
      CriticalSectionScope scope(m_csPendingTasks);
      int size = m_qPendingTasks.size();

      return size;
   }

   void 
   WorkQueue::Clear()
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // Clears the queue of tasks.
   //---------------------------------------------------------------------------
   {
      try
      {
         // Clears the queue of tasks. 
         CriticalSectionScope scope(m_csPendingTasks);

         // There's no std::queue::clear(). Am I the 
         // first one who needs to  clear a queue?! 
         while (!m_qPendingTasks.empty())
            m_qPendingTasks.pop();
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4212, "WorkQueue::Clear", "An unknown error occurred while clearing queue.");
         throw;
      }
   }

   const String&
   WorkQueue::GetName() const
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // Returns the name of this queue.
   //---------------------------------------------------------------------------
   {
      return m_sQueueName;
   }

   void
   WorkQueue::Pause()
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // Tells the queue to stop.
   //---------------------------------------------------------------------------
   {
      // Pause the work queue
      m_bPause = true;
   }

   void
   WorkQueue::Continue()
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // Tells the queue to continue to run.
   //---------------------------------------------------------------------------
   {
      // Continue to run the work queue.
      m_bPause = false;

      // Signal that we should check for new tasks.
      m_hCheckForTask.Set();
   }

   void 
   WorkQueue::StopTask(const String &name)
   {
      CriticalSectionScope scopeTasks(m_csPendingTasks);
      CriticalSectionScope scopeThreads(m_csThreads);

      bool isQueued = false;
      shared_ptr<Task> task = GetTaskByName(name, isQueued);

      if (!task)
         return;

      if (isQueued)
      {
         // Defense. If something screws up, we'll quit looping..
         int maxIterations = 10000;

         // Copy all tasks except for the given one to a new list.
         queue<shared_ptr<Task> > tempQueue;
         while (m_qPendingTasks.size() > 0 && maxIterations > 0)
         {
            shared_ptr<Task> pTask = m_qPendingTasks.front();
            m_qPendingTasks.pop();

            if (pTask->GetName() != name)
               tempQueue.push(pTask);

            maxIterations--;
         }

         // Move back all items from the temporary queue to the real one...
         maxIterations = 10000;
         while (tempQueue.size() > 0 && maxIterations > 0) 
         {
            shared_ptr<Task> pTask = tempQueue.front();
            tempQueue.pop();

            m_qPendingTasks.push(pTask);
            maxIterations--;
         }
      }
      else
      {
         task->StopWork();
      }

   }

   shared_ptr<Task> 
   WorkQueue::GetTaskByName(const String &name, bool &isQueued)
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // Returns a task by its given name. Very costful operation.
   //---------------------------------------------------------------------------
   {
      CriticalSectionScope scopeTasks(m_csPendingTasks);
      CriticalSectionScope scopeThreads(m_csThreads);

      isQueued = false;

      /*
         We can't iterate over a queue. We need to copy the
         entire queue to another list temporarily just to be
         able to find the correct item. Yack.
      */
      shared_ptr<Task> foundTask;

      // Defense. If something screws up, we'll quit looping..
      int maxIterations = 10000;

      queue<shared_ptr<Task> > tempQueue;
      while (m_qPendingTasks.size() > 0 && maxIterations > 0)
      {
         shared_ptr<Task> pTask = m_qPendingTasks.front();
         m_qPendingTasks.pop();

         if (pTask->GetName() == name)
            foundTask = pTask;

         tempQueue.push(pTask);
         
         maxIterations--;
      }

      // Move back all items from the temporary queue to the real one...
      maxIterations = 10000;
      while (tempQueue.size() > 0 && maxIterations > 0) 
      {
         shared_ptr<Task> pTask = tempQueue.front();
         tempQueue.pop();

         m_qPendingTasks.push(pTask);
         maxIterations--;
      }

      if (foundTask)
      {
         isQueued = true;
         return foundTask;
      }

      // The task was not found in the queue. Check if it's running...


      map<HANDLE, shared_ptr<Thread> >::iterator iter = m_mapThreads.begin();
      map<HANDLE, shared_ptr<Thread> >::iterator iterEnd = m_mapThreads.end();

      for (; iter != iterEnd; iter++)
      {
         shared_ptr<Task> currentTask = ((*iter).second)->GetCurrentTask();
         if (currentTask && currentTask->GetName() == name)
            return currentTask;
      }


      shared_ptr<Task> nothing;
      return nothing;
   }
}
