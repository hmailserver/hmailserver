// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "POP3Sessions.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{

   POP3Sessions::POP3Sessions()
   {

   }

   POP3Sessions::~POP3Sessions()
   {

   }

   bool 
   POP3Sessions::IsLocked(__int64 iAccount)
   {
      CriticalSectionScope scope(m_oCritSec);

      bool bRet = false;
      if (m_vecLockedAccounts.find(iAccount) != m_vecLockedAccounts.end())
         bRet = true;

      return bRet;
   }

   bool 
   POP3Sessions::Lock(__int64 iAccount)
   {
      CriticalSectionScope scope(m_oCritSec);

      if (m_vecLockedAccounts.find(iAccount) == m_vecLockedAccounts.end())
      {
         m_vecLockedAccounts.insert(iAccount);
         return true;
      }
      else
      {
         return false;
      }
   }
      
   void 
   POP3Sessions::Unlock(__int64 iAccount)
   {
      CriticalSectionScope scope(m_oCritSec);

      std::set<__int64>::iterator iter =  m_vecLockedAccounts.find(iAccount);
      if (iter != m_vecLockedAccounts.end())
         m_vecLockedAccounts.erase(iter);
   }
}
