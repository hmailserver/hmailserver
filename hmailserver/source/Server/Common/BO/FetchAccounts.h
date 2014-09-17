// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once
#include "Collection.h"

#include "../Persistence/PersistentFetchAccount.h"
#include "FetchAccount.h"

namespace HM
{
   class FetchAccounts : public Collection<FetchAccount, PersistentFetchAccount>
   {
   public:
      FetchAccounts(__int64 iAccountID);
      ~FetchAccounts(void);

      void RefreshPendingList();
      
      void Refresh();

      __int64 GetAccountID () {return account_id_; }

   protected:
      virtual bool PreSaveObject(std::shared_ptr<FetchAccount> pFA, XNode *node);
      virtual String GetCollectionName() const {return "FetchAccounts"; }
   private:

      __int64 account_id_;
   };

}