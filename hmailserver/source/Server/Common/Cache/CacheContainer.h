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
   class CacheConfiguration;

   class CacheContainer : public Singleton<CacheContainer>
   {
   public:
      CacheContainer(void);
      ~CacheContainer(void);

      std::shared_ptr<const Account> GetAccount(const String &sName);
      std::shared_ptr<const Account> GetAccount(__int64 iID);
      size_t GetAccountCacheSize();
      void SetAccountCacheMaxSize(size_t max_size);
      size_t GetAccountCacheMaxSize();

      std::shared_ptr<const Domain> GetDomain(const String &sName);
      std::shared_ptr<const Domain> GetDomain(__int64 iID);
      void RemoveDomain(std::shared_ptr<Domain> domain);
      size_t GetDomainCacheSize();
      void SetDomainCacheMaxSize(size_t max_size);
      size_t GetDomainCacheMaxSize();

      std::shared_ptr<const Alias> GetAlias(const String &sName);
      std::shared_ptr<const Alias> GetAlias(__int64 iID);
      size_t GetAliasCacheSize();
      void SetAliasCacheMaxSize(size_t max_size);
      size_t GetAliasCacheMaxSize();

      std::shared_ptr<const DistributionList> GetDistributionList(const String &sName);
      std::shared_ptr<const DistributionList> GetDistributionList(__int64 iID);
      size_t GetDistributionListCacheSize();
      void SetDistributionListCacheMaxSize(size_t max_size);
      size_t GetDistributionListCacheMaxSize();

      void OnPropertyChanged(std::shared_ptr<Property> pProperty);

      InboxIDCache &GetInboxIDCache();

      void Clear();
   private:

      
      InboxIDCache inbox_idcache_;
   };
}