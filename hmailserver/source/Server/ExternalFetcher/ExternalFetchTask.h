// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

#include "..\Common\Threading\Task.h"
#include ".\FetchAccountLock.h"

namespace HM
{
   class FetchAccount;

   class ExternalFetchTask : public Task
   {
   public:
      ExternalFetchTask(std::shared_ptr<FetchAccount> pFA);
      ~ExternalFetchTask(void);

      virtual void DoWork();

   private:

      std::shared_ptr<FetchAccount> fetch_account_;

      // Declared last, so that the account remains locked until the task has been destroyed.
      FetchAccountLock lock_;
   };
}
