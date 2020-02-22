// Copyright (c) 2013 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class WhiteListAddress;

   class WhiteListCache
   {
   public:
      WhiteListCache(void);

      
      static void SetNeedRefresh();

      bool IsWhitelisted(const String &fromAddress, const IPAddress &address);
   

   private:

      static void Refresh();
      static std::vector<std::shared_ptr<WhiteListAddress>> whitelistItems_;
      static bool needRefresh_;
   };
}