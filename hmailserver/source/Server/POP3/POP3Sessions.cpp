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
      boost::lock_guard<boost::recursive_mutex> guard(mutex_);

      bool bRet = false;
      if (locked_accounts_.find(iAccount) != locked_accounts_.end())
         bRet = true;

      return bRet;
   }

   bool 
   POP3Sessions::Lock(__int64 iAccount)
   {
      boost::lock_guard<boost::recursive_mutex> guard(mutex_);

      if (locked_accounts_.find(iAccount) == locked_accounts_.end())
      {
         locked_accounts_.insert(iAccount);
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
      boost::lock_guard<boost::recursive_mutex> guard(mutex_);

      auto iter =  locked_accounts_.find(iAccount);
      if (iter != locked_accounts_.end())
         locked_accounts_.erase(iter);
   }
}
