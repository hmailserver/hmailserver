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

      static set<std::pair<int, int> > folders_;
      
      static boost::recursive_mutex _mutex; 

      bool _hasLock;

      std::pair<int, int> _lockPair;
   };
}