// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "MessagesContainer.h"

#include "../Common/BO/Messages.h"
#include "../Common/Cache/CachedMessages.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   
   MessagesContainer::MessagesContainer()
   {
      messages_cache_.SetEnabled(true);
      messages_cache_.SetTTL(30 * 60);
      messages_cache_.SetMaxSize(512 * 1024 * 1024); // 512MB

      // When the server is stopped, the cache should be cleared.
      Application::Instance()->OnServerStopped.connect
         (
            [this]() { Clear(); }
         );

   }

   std::shared_ptr<Messages> 
   MessagesContainer::GetMessages(__int64 account_id, __int64 folder_id)
   {
      std::set<__int64> recent_messages;

      return GetMessages(account_id, folder_id, recent_messages, false);
   }

   std::shared_ptr<Messages>
   MessagesContainer::GetMessages(__int64 account_id, __int64 folder_id, std::set<__int64> &recent_messages, bool update_recent_messages)
   {
      auto cached_messages = messages_cache_.GetObject(folder_id);

      if (cached_messages == nullptr)
      {
         auto messages = std::shared_ptr<Messages>(new Messages(account_id, folder_id));

         cached_messages = std::make_shared<CachedMessages>(messages);
         messages_cache_.Add(cached_messages);
      }

      size_t estimated_size_before = cached_messages->GetEstimatedCachingSize();

      // this will cause message to be refreshed from db, if needed:
      auto messages = cached_messages->GetMessages(update_recent_messages);

      size_t estimated_size_after = cached_messages->GetEstimatedCachingSize();

      bool increased_size = false;
      size_t size_change = 0;

      if (estimated_size_after > estimated_size_before)
      {
         increased_size = true;
         size_change = estimated_size_after - estimated_size_before;
      }
      else
      {
         increased_size = false;
         size_change = estimated_size_before - estimated_size_after;
      }
      
      messages_cache_.AdjustEstimatedSize(increased_size, size_change);

      recent_messages.clear();
            
      for (std::shared_ptr<Message> message : messages->GetVector())
      {
         if (message->GetFlagRecent())
            recent_messages.insert(message->GetID());
      }

      if (update_recent_messages)
         messages->RemoveRecentFlags();

      return messages;
   }

   void 
   MessagesContainer::UncacheAccount(__int64 account_id)
   {
      messages_cache_.RemoveObject(Formatter::Format("{0}", account_id));
   }

   void
   MessagesContainer::SetFolderNeedsRefresh(__int64 folder_id)
   {
      auto cached_messages = messages_cache_.GetObject(folder_id);
      if (cached_messages == nullptr)
      {
         // object is currently not cached, no refresh needed
         return;
      }
      
      cached_messages->SetRefreshNeeded();
   }

   void
   MessagesContainer::Clear()
   {
      messages_cache_.Clear();
   }

}
