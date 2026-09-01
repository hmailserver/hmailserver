// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class FetchAccount;

   // Locks a fetch account for the lifetime of this object. A locked account is skipped by
   // ExternalFetchManager, so an account which is never unlocked is never fetched again until
   // the server is restarted.
   class FetchAccountLock
   {
   public:
      FetchAccountLock(std::shared_ptr<FetchAccount> fetch_account);
      ~FetchAccountLock();

   private:

      std::shared_ptr<FetchAccount> fetch_account_;
   };
}
