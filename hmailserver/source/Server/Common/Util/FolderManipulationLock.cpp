// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "FolderManipulationLock.h"


#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   std::set<std::pair<int, int> > FolderManipulationLock::m_setFolders;
   CriticalSection FolderManipulationLock::m_CriticalSection;

   FolderManipulationLock::FolderManipulationLock(int iAccountID, int iFolderID) :
      _hasLock(false)
   {
      _lockPair = std::make_pair(iAccountID, iFolderID);
   }

   FolderManipulationLock::~FolderManipulationLock(void)
   {
      if (_hasLock)
      {
         Release(_lockPair);
      }
   }

   void
   FolderManipulationLock::Lock()
   {
      try
      {
         _hasLock = true;

         Acquire(_lockPair);
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5051, "FolderManipulationLock::Lock", "Acquire threw an exception.");
         throw;
      }
     
   }


   void 
   FolderManipulationLock::Acquire(std::pair<int, int> lockPair)
   {
      while (1)
      {
         // Try to locak the folder. We want to sleep 20 milliseconds
         // between each attempt, but we don't want to lock the critical
         // section meanwhile. Hence the inner scope here:
         {
            CriticalSectionScope scope(m_CriticalSection);

            if (m_setFolders.find(lockPair) == m_setFolders.end())
            {
               m_setFolders.insert(lockPair);
               return;
            }
         }


         Sleep(20);
      }
   }

   void
   FolderManipulationLock::Release(std::pair<int, int> lockPair)
   {
      CriticalSectionScope scope(m_CriticalSection);

      std::set<std::pair<int, int>>::iterator iterPos = m_setFolders.find(lockPair);
      if (iterPos != m_setFolders.end())
      {
         try
         {
            m_setFolders.erase(iterPos);
         }
         catch (...)
         {
            ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5052, "FolderManipulationLock::Lock", "Folders::Erase threw an exception.");
            throw;
         }
      }
   }
}