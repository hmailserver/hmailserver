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
   MessageCache::AddMessage(shared_ptr<Message> pMessage)
   {
      CriticalSectionScope scope(m_csMessage);

      /*
         The message cache may grow if there's many deliveries
         running at the same time. The message is saved in the
         database before it's added to the cache, so the delivery
         manager may read the message from the database before
         it has been added to the cache.

         To prevent this cache from leaking, we hence clear it
         once in a while.
      */

      if (m_mapMessage.size() > 500)
         m_mapMessage.clear();

      if (m_mapMessage.find(pMessage->GetID()) != m_mapMessage.end())
         return;

      m_mapMessage[pMessage->GetID()] = pMessage;

   }

   shared_ptr<Message> 
   MessageCache::GetMessage(__int64 iMessageID)
   {
      CriticalSectionScope scope(m_csMessage);

      std::map<__int64, shared_ptr<Message> >::iterator iterMessage = m_mapMessage.find(iMessageID);

      shared_ptr<Message> pMessage;
      if (iterMessage == m_mapMessage.end())
      {
         // Message not found in cache.
         return pMessage;
      }
      
      // Set message to return from cache
      pMessage = (*iterMessage).second;

      // Delete the message from cache.
      m_mapMessage.erase(iterMessage);

      return pMessage;
   }
}