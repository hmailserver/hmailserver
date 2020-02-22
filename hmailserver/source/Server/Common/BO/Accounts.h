// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "Collection.h"

#include "../Persistence/PersistentAccount.h"
#include "../BO/Account.h"

namespace HM
{

   class Account;
   class PersistentAccount;

   class Accounts : public Collection<Account, PersistentAccount>
   {
   public:
	   Accounts (__int64 iDomainID);
      Accounts (__int64 iDomainID, __int64 iAccountID);
	   virtual ~Accounts();

      void Refresh();
  
   protected:
      
      virtual bool PreSaveObject(std::shared_ptr<Account> pAccount, XNode *node);
      virtual String GetCollectionName() const {return "Accounts"; }


   private:

      __int64 domain_id_;
      __int64 account_id_;
  
   };

}