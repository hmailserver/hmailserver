// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "InboxIDCache.h"

namespace HM
{
   class Domain;
   class Account;
   class Alias;
   class DistributionList;
   class Group;
   class CacheConfiguration;
   
   

   class CacheContainer : public Singleton<CacheContainer>
   {
   public:
      CacheContainer(void);
      ~CacheContainer(void);

      void CreateCaches();
      void DeleteCaches();

      shared_ptr<const Account> GetAccount(const String &sName);
      shared_ptr<const Account> GetAccount(__int64 iID);

      shared_ptr<const Domain> GetDomain(const String &sName);
      shared_ptr<const Domain> GetDomain(__int64 iID);

      shared_ptr<const Alias> GetAlias(const String &sName);
      shared_ptr<const Alias> GetAlias(__int64 iID);

      shared_ptr<const DistributionList> GetDistributionList(const String &sName);
      shared_ptr<const DistributionList> GetDistributionList(__int64 iID);

      void OnPropertyChanged(shared_ptr<Property> pProperty);

      InboxIDCache &GetInboxIDCache();

      void Clear();
   private:

      InboxIDCache _inboxIDCache;
   };
}