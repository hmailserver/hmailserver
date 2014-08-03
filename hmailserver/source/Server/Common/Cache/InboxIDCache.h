// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class InboxIDCache
   {
   public:
      InboxIDCache(void);
      virtual ~InboxIDCache(void);

      void Clear();

      __int64 GetUserInboxFolder(__int64 accountID);

   private:

      boost::recursive_mutex mutex_;
      std::map<__int64, __int64 > inbox_id_;

      bool enabled_;
   };
}