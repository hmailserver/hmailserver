// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "InboxIDCache.h"

#include "../Persistence/PersistentIMAPFolder.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   InboxIDCache::InboxIDCache() :
      _enabled (true)
   {

   }

   InboxIDCache::~InboxIDCache(void)
   {
   }

   void 
   InboxIDCache::Clear()
   {
      if (!_enabled)
         return;
   
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      _inboxID.clear();
   }

   __int64
   InboxIDCache::GetUserInboxFolder(__int64 accountID)
   {
      if (_enabled)
      {
         boost::lock_guard<boost::recursive_mutex> guard(_mutex);
         std::map<__int64, __int64>::iterator iter = _inboxID.find(accountID);

         if (iter != _inboxID.end())
         {
            __int64 result = (*iter).second;
            return result;
         }
      }

      __int64 inboxID = PersistentIMAPFolder::GetUserInboxFolder(accountID);

      if (_enabled)
      {
         boost::lock_guard<boost::recursive_mutex> guard(_mutex);
         _inboxID[accountID] = inboxID;
      }

      return inboxID;
   }
}