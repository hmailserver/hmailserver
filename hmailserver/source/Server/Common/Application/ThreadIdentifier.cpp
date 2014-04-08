// ThreadIdentifier.cpp: implementation of the ThreadIdentifier class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ThreadIdentifier.h"
#include "IThread.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace HM
{
   ThreadIdentifier::ThreadIdentifier() :
      m_hThreadHandle(0),
      m_iThreadID(0)
   {
      m_pThread.reset();

      // Create the start event.
      m_StartEvent = CreateEvent(0,TRUE, FALSE,0);
     
   }

   ThreadIdentifier::~ThreadIdentifier()
   {
      CloseHandle(m_StartEvent);
   }

   void
   ThreadIdentifier::Run()
   {
      SetEvent(m_StartEvent);
   }

   void
   ThreadIdentifier::Close()
   {
      CloseHandle(m_hThreadHandle);
      m_hThreadHandle = 0;

      m_pThread.reset();
   }

   void
   ThreadIdentifier::Stop()
   {
      // Stop the work
      if (m_pThread)
         m_pThread->Stop();

      if (m_hThreadHandle > 0)
      {
         // Wait for the thread to exit for a few seconds.
         if (WaitForSingleObject(m_hThreadHandle, 5000) == WAIT_TIMEOUT)
         {
            // The thread hasn't stopped eventhough it said that
            // it was about to stop. Kill it.

            assert(0);

            // Stop the thread.
            int iResult = ::TerminateThread(m_hThreadHandle, 0);
         }
         
      }

      // And close the thread handle.
      Close();
   }

   void
   ThreadIdentifier::WaitForStart()
   {
      WaitForSingleObject(m_StartEvent, INFINITE);

      m_iThreadID = GetCurrentThreadId();

      if (m_hThreadHandle == 0)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 4353, "ThreadIdentifier::WaitForStart()", "Thread started without a valid handle.");
      }
   }

   int
   ThreadIdentifier::GetThreadID() const
   {
      return m_iThreadID; 
   }

   HANDLE
   ThreadIdentifier::GetThreadHandle() const
   {
      return m_hThreadHandle; 
   }

   void
   ThreadIdentifier::SetThreadHandle(HANDLE h)
   {
      m_hThreadHandle = h;
   }

   shared_ptr<IThreadTask>
   ThreadIdentifier::GetThread() const
   {
      return m_pThread; 
   }

   void
   ThreadIdentifier::SetThread(shared_ptr<IThreadTask> pThread)
   {
      m_pThread = pThread;
   }
}