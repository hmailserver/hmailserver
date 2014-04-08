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
      m_bIsExitingAll = false; 
   }

   ServerThreads::~ServerThreads()
   {

   }

   bool 
   ServerThreads::AddThread(shared_ptr<ThreadIdentifier> pThread)
   {
      m_oCritSec.Enter();

      if (m_vecCurrentConnections.find(pThread) == m_vecCurrentConnections.end())
      {
         m_vecCurrentConnections.insert(pThread);
         m_oCritSec.Leave();
         return true;
      }
      else
      {
         m_oCritSec.Leave();
         return false;
      }
   }

   bool
   ServerThreads::CloseThread(shared_ptr<ThreadIdentifier> pThread)
   {
      if (m_bIsExitingAll)
      {
         // We are stopping the server right now.
         return true;
      }

      m_oCritSec.Enter();

      std::set<shared_ptr<ThreadIdentifier> >::iterator iter =  m_vecCurrentConnections.begin();;
      while (iter != m_vecCurrentConnections.end())
      {
         shared_ptr<ThreadIdentifier> pCurThread = (*iter);
         if (pCurThread == pThread)
         {
            pThread->Close();
            
            m_vecCurrentConnections.erase(iter);
            break;
         }

         iter++;
      }

      m_oCritSec.Leave();  

      return true;
   }
   
   bool
   ServerThreads::ExitAllThreads()
   {
      m_bIsExitingAll = true;

      m_oCritSec.Enter();

      std::set<shared_ptr<ThreadIdentifier> >::iterator iter =  m_vecCurrentConnections.begin();;
      while (iter != m_vecCurrentConnections.end())
      {
         // Actually stop the thread from running.
         shared_ptr<ThreadIdentifier> pThread = (*iter);
         pThread->Stop();
         iter++;
      }
      m_vecCurrentConnections.clear();

      m_oCritSec.Leave();     

      m_bIsExitingAll = false; 

      return true;
   }


   shared_ptr<ThreadIdentifier> 
   ServerThreads::GetThreadByThreadID(int iThreadID) 
   {
      m_oCritSec.Enter();

      std::set<shared_ptr<ThreadIdentifier> >::const_iterator iter =  m_vecCurrentConnections.begin();;
      while (iter != m_vecCurrentConnections.end())
      {
         shared_ptr<ThreadIdentifier> pThread = (*iter);
         if (pThread->GetThreadID() == iThreadID)
         {
            m_oCritSec.Leave();
            return pThread;
         }
         iter++;
      }

      m_oCritSec.Leave();
      shared_ptr<ThreadIdentifier> pEmpty;
      return pEmpty;

   }

   int 
   ServerThreads::GetThreadCount()
   {
       m_oCritSec.Enter();
       int iRetVal = m_vecCurrentConnections.size();
       m_oCritSec.Leave();
       return iRetVal; 
   }

}
