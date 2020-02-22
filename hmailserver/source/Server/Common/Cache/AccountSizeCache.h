// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class AccountSizeCache : public Singleton<AccountSizeCache>
   {
   public:
      AccountSizeCache(void);
      virtual ~AccountSizeCache(void);

      void ModifySize(__int64 accountID, __int64 iSize, bool bIncrease);
      void Reset(__int64 accountID);
      __int64 GetSize(__int64 accountID);

   private:

      std::map<__int64, __int64> account_sizes_;

      boost::recursive_mutex mutex_;
   };
}