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
      CriticalSectionScope scope(_lock);

      std::map<__int64, __int64>::iterator iter = _accountSizes.find(accountID);
      if (iter == _accountSizes.end())
         return;

      __int64 currentSize = (*iter).second;

      if (bIncrease)
         currentSize += iSize;
      else
         currentSize -= iSize;

      _accountSizes[accountID] = currentSize;
   }
   
   void
   AccountSizeCache::Reset(__int64 accountID)
   {
      CriticalSectionScope scope(_lock);

      std::map<__int64, __int64>::iterator iter = _accountSizes.find(accountID);
      if (iter == _accountSizes.end())
         return;

      _accountSizes.erase(iter);
   }


   __int64
   AccountSizeCache::GetSize(__int64 accountID)
   {
      CriticalSectionScope scope(_lock);

      std::map<__int64, __int64>::iterator iter = _accountSizes.find(accountID);
      if (iter == _accountSizes.end())
      {
         __int64 size = PersistentAccount::GetMessageBoxSize(accountID);
         _accountSizes[accountID] = size;
         return size;
      }

      return (*iter).second;
   }
   
}