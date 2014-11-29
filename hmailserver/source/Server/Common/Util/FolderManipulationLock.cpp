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
   std::set<std::pair<int, int> > FolderManipulationLock::folders_;
   boost::recursive_mutex FolderManipulationLock::mutex_;

   FolderManipulationLock::FolderManipulationLock(int iAccountID, int iFolderID) :
      has_lock_(false)
   {
      lock_pair_ = std::make_pair(iAccountID, iFolderID);
   }

   FolderManipulationLock::~FolderManipulationLock(void)
   {
      try
      {
         if (has_lock_)
         {
            Release(lock_pair_);
         }
      }
      catch (...)
      {

      }

   }

   void
   FolderManipulationLock::Lock()
   {
      try
      {
         has_lock_ = true;

         Acquire(lock_pair_);
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
            boost::lock_guard<boost::recursive_mutex> guard(mutex_);

            if (folders_.find(lockPair) == folders_.end())
            {
               folders_.insert(lockPair);
               return;
            }
         }


         Sleep(20);
      }
   }

   void
   FolderManipulationLock::Release(std::pair<int, int> lockPair)
   {
      boost::lock_guard<boost::recursive_mutex> guard(mutex_);

      auto iterPos = folders_.find(lockPair);
      if (iterPos != folders_.end())
      {
         try
         {
            folders_.erase(iterPos);
         }
         catch (...)
         {
            ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5052, "FolderManipulationLock::Lock", "Folders::Erase threw an exception.");
            throw;
         }
      }
   }
}