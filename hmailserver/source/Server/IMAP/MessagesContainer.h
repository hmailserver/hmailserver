// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../Common/Cache/Cache.h"

namespace HM
{
   class Messages;
   class CachedMessages;

   class MessagesContainer : public Singleton<MessagesContainer>
   {
   public:

      MessagesContainer();

      std::shared_ptr<Messages> GetMessages(__int64 account_id, __int64 folder_id);
      std::shared_ptr<Messages> GetMessages(__int64 account_id, __int64 folder_id, std::set<__int64> &recent_messages, bool update_recent_messages);

      void SetFolderNeedsRefresh(__int64 folder_id);
      void UncacheAccount(__int64 account_id);
      void Clear();

   private:
      
      Cache<CachedMessages> messages_cache_;


   };
}
