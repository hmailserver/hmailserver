// ServerThreads.cpp: implementation of the ServerThreads class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ServerThreads.h"
#include "ThreadIdentifier.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace HM
{



   ServerThreads::ServerThreads()
   {  
      is_exiting_all_ = false; 
   }

   ServerThreads::~ServerThreads()
   {

   }

   bool 
   ServerThreads::AddThread(shared_ptr<ThreadIdentifier> pThread)
   {
      crit_sec_.Enter();

      if (current_connections_.find(pThread) == current_connections_.end())
      {
         current_connections_.insert(pThread);
         crit_sec_.Leave();
         return true;
      }
      else
      {
         crit_sec_.Leave();
         return false;
      }
   }

   bool
   ServerThreads::CloseThread(shared_ptr<ThreadIdentifier> pThread)
   {
      if (is_exiting_all_)
      {
         // We are stopping the server right now.
         return true;
      }

      crit_sec_.Enter();

      std::set<shared_ptr<ThreadIdentifier> >::iterator iter =  current_connections_.begin();;
      while (iter != current_connections_.end())
      {
         shared_ptr<ThreadIdentifier> pCurThread = (*iter);
         if (pCurThread == pThread)
         {
            pThread->Close();
            
            current_connections_.erase(iter);
            break;
         }

         iter++;
      }

      crit_sec_.Leave();  

      return true;
   }
   
   bool
   ServerThreads::ExitAllThreads()
   {
      is_exiting_all_ = true;

      crit_sec_.Enter();

      std::set<shared_ptr<ThreadIdentifier> >::iterator iter =  current_connections_.begin();;
      while (iter != current_connections_.end())
      {
         // Actually stop the thread from running.
         shared_ptr<ThreadIdentifier> pThread = (*iter);
         pThread->Stop();
         iter++;
      }
      current_connections_.clear();

      crit_sec_.Leave();     

      is_exiting_all_ = false; 

      return true;
   }


   shared_ptr<ThreadIdentifier> 
   ServerThreads::GetThreadByThreadID(int iThreadID) 
   {
      crit_sec_.Enter();

      std::set<shared_ptr<ThreadIdentifier> >::const_iterator iter =  current_connections_.begin();;
      while (iter != current_connections_.end())
      {
         shared_ptr<ThreadIdentifier> pThread = (*iter);
         if (pThread->GetThreadID() == iThreadID)
         {
            crit_sec_.Leave();
            return pThread;
         }
         iter++;
      }

      crit_sec_.Leave();
      shared_ptr<ThreadIdentifier> pEmpty;
      return pEmpty;

   }

   int 
   ServerThreads::GetThreadCount()
   {
       crit_sec_.Enter();
       int iRetVal = current_connections_.size();
       crit_sec_.Leave();
       return iRetVal; 
   }

}
