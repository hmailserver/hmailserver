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

      CriticalSection _criticalSection;
      std::map<__int64, __int64 > _inboxID;

      bool _enabled;
   };
}