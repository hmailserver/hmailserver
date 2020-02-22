// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "AccountSizeCache.h"
#include "../Persistence/PersistentAccount.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   AccountSizeCache::AccountSizeCache(void)
   {

   }

   AccountSizeCache::~AccountSizeCache(void)
   {

   }

   void
   AccountSizeCache::ModifySize(__int64 accountID, __int64 iSize, bool bIncrease)
   {
      boost::lock_guard<boost::recursive_mutex> guard(mutex_);

      auto iter = account_sizes_.find(accountID);
      if (iter == account_sizes_.end())
         return;

      __int64 currentSize = (*iter).second;

      if (bIncrease)
         currentSize += iSize;
      else
         currentSize -= iSize;

      account_sizes_[accountID] = currentSize;
   }
   
   void
   AccountSizeCache::Reset(__int64 accountID)
   {
      boost::lock_guard<boost::recursive_mutex> guard(mutex_);

      auto iter = account_sizes_.find(accountID);
      if (iter == account_sizes_.end())
         return;

      account_sizes_.erase(iter);
   }


   __int64
   AccountSizeCache::GetSize(__int64 accountID)
   {
      boost::lock_guard<boost::recursive_mutex> guard(mutex_);

      auto iter = account_sizes_.find(accountID);
      if (iter == account_sizes_.end())
      {
         __int64 size = PersistentAccount::GetMessageBoxSize(accountID);
         account_sizes_[accountID] = size;
         return size;
      }

      return (*iter).second;
   }
   
}