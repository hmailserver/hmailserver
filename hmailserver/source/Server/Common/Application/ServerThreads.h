// ServerThreads.h: interface for the ServerThreads class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <set>

namespace HM
{
   class ThreadIdentifier;

   class ServerThreads  
   {
   public:
      ServerThreads();
      virtual ~ServerThreads();

      bool AddThread(shared_ptr<ThreadIdentifier> pThread);
      bool CloseThread(shared_ptr<ThreadIdentifier> pThread);
      bool ExitAllThreads();

      int GetThreadCount();

      shared_ptr<ThreadIdentifier> GetThreadByThreadID(int iID);

   private:
	   
      std::set<shared_ptr<ThreadIdentifier> > current_connections_;

      CriticalSection crit_sec_;

      bool is_exiting_all_;
      
   };

}
