// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "..\Common\Threading\Task.h"
#include "../Common/Util/Event.h"

namespace HM
{
   class ClientInfo;
   class FetchAccount;
   class FetchAccounts;

   class ExternalFetchManager : public Task
   {
   public:
      ExternalFetchManager(void);
      ~ExternalFetchManager(void);

      void DoWork();
      
      void SetCheckNow();
   private:

      bool FetchIsAllowed_(std::shared_ptr<FetchAccount> pFA);

      std::shared_ptr<FetchAccounts> fetch_accounts_;

      size_t queue_id_;
      const String queue_name_;

      Event check_now_;
   };
}