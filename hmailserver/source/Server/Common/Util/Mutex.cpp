// Mutex.cpp: implementation of the Mutex class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Mutex.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace HM
{
   Mutex::Mutex()
   {

   }

   Mutex::~Mutex()
   {

   }

   void
   Mutex::Wait()
   {
      DWORD dwWaitResult;
      dwWaitResult = ::WaitForSingleObject(m_hMutex, INFINITE); 

      if (dwWaitResult == WAIT_OBJECT_0)
      {
         //return true;
         return;
      }
      else
      {
         //return false;
         return;
      }

   }

   void
   Mutex::Release()
   {
      ::ReleaseMutex(m_hMutex);
   }
}