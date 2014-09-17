// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class FolderManipulationLock
   {
   public:
      FolderManipulationLock(int iAccountID, int iFolderID);
      ~FolderManipulationLock(void);

      void Lock();

   private:

      static void Acquire(std::pair<int, int> lockPair);
      static void Release(std::pair<int, int> lockPair);

      static std::set<std::pair<int, int> > folders_;
      
      static boost::recursive_mutex mutex_; 

      bool has_lock_;

      std::pair<int, int> lock_pair_;
   };
}