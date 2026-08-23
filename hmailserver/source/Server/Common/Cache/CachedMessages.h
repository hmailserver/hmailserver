// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com
// Created 2005-07-21

#pragma once

#include "../Util/Singleton.h"
#include "../BO/Messages.h"

namespace HM
{

   class CachedMessages
   {
   public:
      
      CachedMessages(std::shared_ptr<Messages> messages) :
         messages_(messages),
         refresh_needed_(true)
      {
         
      }

      __int64 GetID()
      {
         return messages_->GetFolderID();
      }

      String GetName()
      {
         String name = Formatter::Format("{0}", messages_->GetAccountID());
         
         return name;
      }

      size_t GetEstimatedCachingSize()
      {
         return 1024 * messages_->GetCount();
      }

      std::shared_ptr<Messages> GetMessages(bool update_recent_flags) 
      { 
         // Held across the refresh, so a second caller waits for it rather than returning a
         // folder which has only been partially loaded.
         boost::lock_guard<boost::recursive_mutex> guard(refresh_mutex_);

         if (refresh_needed_)
         {
            // Cleared before the refresh, so a change made while it runs isn't swallowed.
            refresh_needed_ = false;

            messages_->Refresh(update_recent_flags);
         }

         return messages_; 
      }

      void SetRefreshNeeded()
      {
         boost::lock_guard<boost::recursive_mutex> guard(refresh_mutex_);

         refresh_needed_ = true;
      }

   private:

      std::shared_ptr<Messages> messages_;
      bool refresh_needed_;
      boost::recursive_mutex refresh_mutex_;
   };
}