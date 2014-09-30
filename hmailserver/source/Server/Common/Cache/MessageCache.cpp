// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com
// Created 2005-07-21

#include "StdAfx.h"

#include "MessageCache.h"
#include "..\BO\Message.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   MessageCache::MessageCache(void)
   {
   }

   MessageCache::~MessageCache(void)
   {
   }

   void 
   MessageCache::AddMessage(std::shared_ptr<Message> pMessage)
   {
      boost::lock_guard<boost::recursive_mutex> guard(mutex_);

      /*
         The message cache may grow if there's many deliveries
         running at the same time. The message is saved in the
         database before it's added to the cache, so the delivery
         manager may read the message from the database before
         it has been added to the cache.

         To prevent this cache from leaking, we hence clear it
         once in a while.
      */

      if (message_.size() > 500)
         message_.clear();

      if (message_.find(pMessage->GetID()) != message_.end())
         return;

      message_[pMessage->GetID()] = pMessage;

   }

   std::shared_ptr<Message> 
   MessageCache::GetMessage(__int64 iMessageID)
   {
      boost::lock_guard<boost::recursive_mutex> guard(mutex_);

      auto iterMessage = message_.find(iMessageID);

      std::shared_ptr<Message> pMessage;
      if (iterMessage == message_.end())
      {
         // Message not found in cache.
         return pMessage;
      }
      
      // Set message to return from cache
      pMessage = (*iterMessage).second;

      // Delete the message from cache.
      message_.erase(iterMessage);

      return pMessage;
   }
}