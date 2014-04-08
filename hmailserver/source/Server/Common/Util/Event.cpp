// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include "Event.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   Event::Event(void) :
      _closeWhenWaited(true)
   {
      m_hEvent = CreateEvent(0, TRUE, FALSE, 0);
   }

   // Copy constructor. Don't close the even when we're done waiting
   Event::Event(const Event& p)
   {
      _closeWhenWaited = false;
      m_hEvent = p.GetHandle();
   }

   Event::~Event(void)
   {
      if (_closeWhenWaited)
         CloseHandle(m_hEvent);
   }

   void 
   Event::Wait()
   {
      WaitForSingleObject(m_hEvent, INFINITE);
   }

   bool 
   Event::IsSet()
   {
      if (WaitForSingleObject(m_hEvent, 1) == WAIT_OBJECT_0)
         return true;
      else
         return false;
   }

   void 
   Event::Set()
   {  
      SetEvent(m_hEvent);
   }

   void
   Event::Reset()
   {     
      ResetEvent(m_hEvent);
   }

   HANDLE 
   Event::GetHandle() const
   {
      return m_hEvent;
   }

}