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

      bool _FetchIsAllowed(shared_ptr<FetchAccount> pFA);

      shared_ptr<FetchAccounts> m_pFetchAccounts;

      int m_iQueueID;
      const String m_sQueueName;

      Event check_now_;
   };
}