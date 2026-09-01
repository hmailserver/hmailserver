// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "StdAfx.h"
#include ".\FetchAccountLock.h"

#include "..\Common\BO\FetchAccount.h"
#include "../Common/Persistence/PersistentFetchAccount.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   FetchAccountLock::FetchAccountLock(std::shared_ptr<FetchAccount> fetch_account) :
      fetch_account_(fetch_account)
   {
      PersistentFetchAccount::Lock(fetch_account_->GetID());
   }

   FetchAccountLock::~FetchAccountLock()
   {
      // Set the next try time first, so that a failing account backs off rather than being
      // retried immediately. Unlocking must be attempted even if that fails.
      try
      {
         PersistentFetchAccount::SetNextTryTime(fetch_account_);
      }
      catch (...)
      {
      }

      try
      {
         PersistentFetchAccount::Unlock(fetch_account_->GetID());
      }
      catch (...)
      {
      }
   }
}
