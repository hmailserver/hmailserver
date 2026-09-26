// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

namespace HM
{
   /*
      Hands out IMAP UIDVALIDITY values for new folders. A folder re-created under the same name
      restarts its UIDs, so it must get a higher UIDVALIDITY (RFC 3501 2.3.1.1). Each account
      has its own counter in hm_uidvalidity, so a user can't learn how many folders other
      accounts create. Public folders use account 0.
   */
   class UIDValidityGenerator
   {
   public:

      // A value higher than any given out in the account before. 0 if it can't be stored.
      static unsigned int GetNext(__int64 account_id);

      // Removes the account's counter.
      static bool DeleteAccount(__int64 account_id);

   private:

      // The account's counter, or where it should start if the account has none yet.
      static bool GetCounter_(__int64 account_id, __int64 &value, bool &exists);
      static bool SetCounter_(__int64 account_id, __int64 value, bool exists);

      static boost::mutex mutex_;
   };
}
