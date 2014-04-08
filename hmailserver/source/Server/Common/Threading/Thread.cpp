// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com
// Created 2005-07-21

#include "StdAfx.h"
#include "Thread.h"
#include "Task.h"
#include "WorkQueue.h"

#include <boost/system/system_error.hpp>

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{

   DWORD WINAPI _Start(LPVOID vd)
   {
      HM::Thread *pThread = static_cast<HM::Thread*>(vd);
      pThread->ThreadFunc();
      return 0;
   }

   Thread::Thread(WorkQueue *pParentQueue) :
      m_pParentQueue(pParentQueue),
      m_eThreadState(Ready)
   {
      assert(m_pParentQueue);

      m_hThreadHandle = 0;

   }

   void 
   Thread::Start()
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // Starts the thread by calling the windows API.
   //---------------------------------------------------------------------------
   {
      if (m_hThreadHandle)
      {
         assert(0);
         CloseHandle(m_hThreadHandle);
         m_hThreadHandle  = 0;
      }

      m_hThreadHandle = CreateThread(NULL, 0, _Start, this, 0, 0);

      if (m_hThreadHandle == 0)
      {
         String sErrorMessage;
         sErrorMessage.Format(_T("Worker thread could not be created. Error: %d"), GetLastError());

         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4310, "Thread::Start", sErrorMessage);

         return;
      }

      _SetPriority();

   }

   void 
   Thread::_SetPriority()
   {
      int iPriority = Configuration::Instance()->GetWorkerThreadPriority();

      if (iPriority == THREAD_PRIORITY_NORMAL || iPriority < -15 || iPriority > 5)
         return;

      if (!SetThreadPriority (m_hThreadHandle, iPriority))
      {
         assert(0);

         String sErrorMessage;
         sErrorMessage.Format(_T("Worker thread priority could not be set to %d. Error: %d"), iPriority, GetLastError());

         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4309, "Thread::_SetPriority", sErrorMessage);
      }
   }

   Thread::~Thread(void)
   {
      if (m_hThreadHandle)
      {
         assert(0); // deleting this, even though thread is running.
      }

      // Close the thread handle.
      CloseThread();

   }

   void 
   Thread::CloseThread()
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // Closes the thread handle. If we don't call this, the handle will leak.
   //---------------------------------------------------------------------------
   {
      CloseHandle(m_hThreadHandle);
      m_hThreadHandle = 0;
   }


   void 
   Thread::Stop(bool bBeKind)
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // Stops the thread. If bBeKind is true, we try to stop it by setting the
   // stop event. If bBeKind is false, we terminate the thread. 
   //---------------------------------------------------------------------------
   {
      // Tell the thread that it should stop running. 
      m_evtStopRequest.Set();

      if (m_eThreadState == Ready)
      {
         // No task is runny, but it hasn't stopped.
         // The SetEvent a few lines up should make
         // this thread stop very soon.
         if (!bBeKind)
         {
            _TerminateThread();
         }
         
      }
      else if (m_eThreadState == Running)
      {
         // We need to try to stop the task.
         if (bBeKind)
         {
            // Tell the task to stop it work. If we're
            // a bit lucky, this will cause DoWork()
            // above to exit. If we're unlucky, the task
            // will disappear here. That's why we're 
            // creating a local copy of the pointer to 
            // ensure that it won't disappear.

            try
            {
               shared_ptr<Task> pTask = m_pTask;
               if (pTask)
                  pTask->StopWork();
            }
            catch (...)
            {
               ErrorManager::Instance()->ReportError(ErrorManager::Low, 4215, "Thread::ThreadFunc", "An unknown error occurred while stopping a tasks.");
            }
         }

         else
         {
            _TerminateThread();
         }
      }

   }

   void 
   Thread::_TerminateThread()
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // Terminates the thread using Windows API, and closes the thread handle
   //---------------------------------------------------------------------------
   {
      // Looks like we'll have to kill the thread.
      if (!::TerminateThread(m_hThreadHandle, 0))
      {
         String sErrorMessage;
         sErrorMessage.Format(_T("Worker thread could not be terminated. Error: %d"), GetLastError());

         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4311, "Thread::Start", sErrorMessage);
      }

      // Tell the queue that we're exiting so that it
      // can remove us from the list of threads.
      m_pParentQueue->OnThreadExited(this);
   }
   
   void 
   Thread::AssignTask(shared_ptr<Task> pTask)
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // Assigns a task to this thread. This should only be done by the work queue.
   //---------------------------------------------------------------------------

   {
      if (m_eThreadState != Ready)
      {
         // This thread can't be assigned a task, if it's already
         // working with another.
         assert(0);
         return;
      }
      
      if (m_pTask)
      {
         assert(0);
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5318, "Thread::AssignTask", "Assigning task to thread even though thread already has task.");
      }

      // Switch over to Running state so that
      // no-one else tries to use us.
      m_eThreadState = Running;
      
      m_pTask = pTask;

      // Signal that a new task has arrived.
      m_evtNewTask.Set();
   }

   // The below is run in the thread. The above code is 
   // normally run outside the thread.
   
   void
   Thread::ThreadFunc()
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // This function sits and waits for the WorkQueue to delegate a new task to
   // this thread. As soon as a task has been received, it's run immediately.
   //---------------------------------------------------------------------------
   {
      try
      {
         while (1)
         {
            // Wait for a new task.
            const int iSize = 2;
            HANDLE handles[iSize];

            handles[0] = m_evtStopRequest.GetHandle();
            handles[1] = m_evtNewTask.GetHandle();

            DWORD dwWaitResult = WaitForMultipleObjects(iSize, handles, FALSE, INFINITE);

            int iEvent = dwWaitResult - WAIT_OBJECT_0;

            switch (iEvent)
            {
            case 0:
               // We should stop now
               m_evtStopRequest.Reset();
               m_pParentQueue->OnThreadExited(this);
               return;
            case 1:
               m_evtNewTask.Reset();
               break;
            }

            // Do the work for the task
            if (m_pTask)
            {
               // Changed so that if an exception is thrown by a task, we will still notify the
               // parent queue when we've done running...

               try
               {
                  m_pTask->DoWork();
               }
               catch (boost::system::system_error error)
               {
                  String sErrorMessage;
                  sErrorMessage.Format(_T("An error occurred while running thread. Returning thread to pool. Error number: %d, Description: %s"), error.code().value(), String(error.what()));
                  ErrorManager::Instance()->ReportError(ErrorManager::High, 4208, "Thread::ThreadFunc", sErrorMessage);
               }
               catch (std::exception const& e)
               {
                  String sErrorMessage = Formatter::Format("An error occurred while running thread. Returning thread to pool. Error number: {0}", e.what());
                  ErrorManager::Instance()->ReportError(ErrorManager::High, 4208, "Thread::ThreadFunc", sErrorMessage);
               }
               catch (...)
               {
                  ErrorManager::Instance()->ReportError(ErrorManager::High, 4208, "Thread::ThreadFunc", "An unknown error occurred while running thread. Returning thread to pool.");
               }

               if (!m_pTask)
               {
                  ErrorManager::Instance()->ReportError(ErrorManager::High, 5320, "Thread::ThreadFunc", "The task is unavailable.");
               }

               m_pTask.reset();
            }

            try
            {
               if (!m_pParentQueue->OnThreadReady(this))
               {
                  // Tell the queue that we're exiting so that it
                  // can remove us from the list of threads.
                  m_pParentQueue->OnThreadExited(this);
                  return;
               }
            }
            catch (...)
            {
               ErrorManager::Instance()->ReportError(ErrorManager::High, 5319, "Thread::ThreadFunc", "Unable to notify queue that thread is ready.");
            }

         }

      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 4208, "Thread::ThreadFunc", "An unknown error occurred while running thread.");
      }
   }

   void 
   Thread::SetStateReady()
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // Sets the thread state to Ready. This means that new tasks can be assigned to it.
   //--------------------------------------------------------------------------- 
   {
      m_eThreadState = Ready;
   }

}