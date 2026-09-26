// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

namespace HM
{
   /*
      A folder's creation time is its IMAP UIDVALIDITY. A folder re-created under the same name
      restarts its UIDs, so it must get a higher UIDVALIDITY (RFC 3501 2.3.1.1), even when it
      is re-created within the same second. So every new folder gets a later second than the last.
   */
   class UIDValidityGenerator
   {
   public:

      static DateTime GetNewCreationTime();

   private:

      // The last value handed out. After a restart, it's the newest folder in the database.
      static unsigned int GetLastUIDValidity_();

      static boost::mutex mutex_;
      static bool loaded_;
      static unsigned int last_uid_validity_;
   };
}
