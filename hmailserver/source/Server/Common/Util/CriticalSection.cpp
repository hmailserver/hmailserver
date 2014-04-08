// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include ".\CriticalSection.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   CriticalSection::CriticalSection() :
      _initialized(false)
   {
      try
      {
         InitializeCriticalSection(&m_CriticalSection);
         
         _initialized = true;
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5313, "CriticalSection::CriticalSection", "Initialization of critical section failed. Out of memory?");
         throw;
      }
   }

   CriticalSection::~CriticalSection(void)
   {
      DeleteCriticalSection(&m_CriticalSection);
   }

   bool
   CriticalSection::Enter() const
   {
      if (_initialized)
      {
         try
         {
            EnterCriticalSection(&m_CriticalSection); 
            return true;
         }
         catch(...)
         {
            ErrorManager::Instance()->ReportError(ErrorManager::High, 5313, "CriticalSection::Enter", "EnterCriticalSection gave an exception. Out of memory?");
            throw;
         }
      }
      else
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5314, "CriticalSection::Enter", "Trying to enter uninitialied critical section. Out of memory?");
      }

      return false;
   }  

   void
   CriticalSection::Leave() const
   {
      if (_initialized)
         LeaveCriticalSection(&m_CriticalSection);
   }


   CriticalSectionScope::CriticalSectionScope(const CriticalSection &InputSec) :
      m_Section(InputSec)
   {
      m_Section.Enter();
   }
   
   CriticalSectionScope::~CriticalSectionScope(void)
   {
      m_Section.Leave();
   }
}