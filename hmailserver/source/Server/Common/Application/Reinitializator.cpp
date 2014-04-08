// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com
// This class is a bit whacky. The single purpose of it is to
// reintialize the entire server. Sometimes, for example when
// doing a restore, this needs to be done asynchroniously.

#include "StdAfx.h"
#include ".\reinitializator.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif


DWORD WINAPI _Start(LPVOID vd)
{
   HM::Reinitializator *pReinit = static_cast<HM::Reinitializator*>(vd);
   pReinit->ThreadFunc();
   return 0;
}

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{

   Reinitializator::Reinitializator(void)
   {
      m_hThreadHandle = 0;
   }

   Reinitializator::~Reinitializator(void)
   {
      if (m_hThreadHandle)
      {
         // Tell the thread to stop.
         m_evtStopRequest.Set();
         
         // Wait for it to stop.
         WaitForSingleObject(m_hThreadHandle, 5000);

         // Close the handle to it.
         CloseHandle(m_hThreadHandle);
      }
   }

   void 
   Reinitializator::ReInitialize()
   {
      if (!m_hThreadHandle)
      {
         // Create the reinitialization thread
         m_hThreadHandle = CreateThread(NULL, 0, _Start, this, 0, 0);
      }

      // Set the event that does reinitialization.
      m_evtReinitRequest.Set();
   }

   void
   Reinitializator::ThreadFunc()
   {
      while (1)
      {
         // Wait for a new task.
         const int iSize = 2;
         HANDLE handles[iSize];

         handles[0] = m_evtStopRequest.GetHandle();
         handles[1] = m_evtReinitRequest.GetHandle();

         DWORD dwWaitResult = WaitForMultipleObjects(iSize, handles, FALSE, INFINITE);

         int iEvent = dwWaitResult - WAIT_OBJECT_0;

         switch (iEvent)
         {
         case 0:
            return;
         }

         // Reset the event so that we don't call it over
         // and over again.
         m_evtReinitRequest.Reset();

         // Reinit now.
         Application::Instance()->Reinitialize();

      }

   }
}