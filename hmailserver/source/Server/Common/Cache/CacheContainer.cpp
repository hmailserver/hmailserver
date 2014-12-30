// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "CacheContainer.h"

#include "CacheConfiguration.h"

#include "Cache.h"

#include "../Application/PropertySet.h"
#include "../Application/Property.h"

#include "../BO/Domain.h"
#include "../BO/Account.h"
#include "../BO/Alias.h"
#include "../BO/DistributionList.h"
#include "../BO/Group.h"

#include "../Persistence/PersistentDomain.h"
#include "../Persistence/PersistentAccount.h"
#include "../Persistence/PersistentAlias.h"
#include "../Persistence/PersistentDistributionList.h"
#include "../Persistence/PersistentGroup.h"
#include "../Persistence/PersistentServerMessage.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   CacheContainer::CacheContainer(void)
   {
      // Limit the cache of each entity to 1MB each.
      Cache<Domain, PersistentDomain>::Instance()->SetMaxSize(1048576);
      Cache<Domain, PersistentAccount>::Instance()->SetMaxSize(1048576);
      Cache<Alias, PersistentAlias>::Instance()->SetMaxSize(1048576);
      Cache<DistributionList, PersistentDistributionList>::Instance()->SetMaxSize(1048576);
   }

   CacheContainer::~CacheContainer(void)
   {

   }

   std::shared_ptr<const Account> 
   CacheContainer::GetAccount(const String &sName)
   {
      return Cache<Account, PersistentAccount>::Instance()->GetObject(sName);
   }

   std::shared_ptr<const Account> 
   CacheContainer::GetAccount(__int64 iID)
   {
      return Cache<Account, PersistentAccount>::Instance()->GetObject(iID);
   }

   size_t
   CacheContainer::GetAccountCacheSize()
   {
      return Cache<Account, PersistentAccount>::Instance()->GetSize();
   }

   void
   CacheContainer::SetAccountCacheMaxSize(size_t max_size)
   {
      return Cache<Account, PersistentAccount>::Instance()->SetMaxSize(max_size);
   }

   size_t
   CacheContainer::GetAccountCacheMaxSize()
   {
      return Cache<Account, PersistentAccount>::Instance()->GetMaxSize();
   }

   std::shared_ptr<const Domain> 
   CacheContainer::GetDomain(const String &sName)
   {
#ifdef _DEBUG
      if (sName.Contains(_T("@")))
      {
         assert(0);
      }
#endif
      return Cache<Domain, PersistentDomain>::Instance()->GetObject(sName);
   }

   std::shared_ptr<const Domain> 
   CacheContainer::GetDomain(__int64 iID)
   {
      return Cache<Domain, PersistentDomain>::Instance()->GetObject(iID);
   }
   
   size_t
   CacheContainer::GetDomainCacheSize()
   {
      return Cache<Domain, PersistentDomain>::Instance()->GetSize();
   }

   void
   CacheContainer::SetDomainCacheMaxSize(size_t max_size)
   {
      return Cache<Domain, PersistentDomain>::Instance()->SetMaxSize(max_size);
   }

   size_t
   CacheContainer::GetDomainCacheMaxSize()
   {
      return Cache<Domain, PersistentDomain>::Instance()->GetMaxSize();
   }

   void
   CacheContainer::RemoveDomain(std::shared_ptr<Domain> domain)
   {
      return Cache<Domain, PersistentDomain>::Instance()->RemoveObject(domain);
   }

   std::shared_ptr<const Alias> 
   CacheContainer::GetAlias(const String &sName)
   {
      return Cache<Alias, PersistentAlias>::Instance()->GetObject(sName);
   }

   std::shared_ptr<const Alias> 
   CacheContainer::GetAlias(__int64 iID)
   {
      return Cache<Alias, PersistentAlias>::Instance()->GetObject(iID);
   }

   size_t
   CacheContainer::GetAliasCacheSize()
   {
      return Cache<Alias, PersistentAlias>::Instance()->GetSize();
   }

   void
   CacheContainer::SetAliasCacheMaxSize(size_t max_size)
   {
      return Cache<Alias, PersistentAlias>::Instance()->SetMaxSize(max_size);
   }

   size_t
   CacheContainer::GetAliasCacheMaxSize()
   {
      return Cache<Alias, PersistentAlias>::Instance()->GetMaxSize();
   }

   std::shared_ptr<const DistributionList> 
   CacheContainer::GetDistributionList(const String &sName)
   {
      return Cache<DistributionList, PersistentDistributionList>::Instance()->GetObject(sName);
   }

   std::shared_ptr<const DistributionList> 
   CacheContainer::GetDistributionList(__int64 iID)
   {
      return Cache<DistributionList, PersistentDistributionList>::Instance()->GetObject(iID);
   }

   size_t
   CacheContainer::GetDistributionListCacheSize()
   {
      return Cache<DistributionList, PersistentDistributionList>::Instance()->GetSize();
   }

   void
   CacheContainer::SetDistributionListCacheMaxSize(size_t max_size)
   {
      return Cache<DistributionList, PersistentDistributionList>::Instance()->SetMaxSize (max_size);
   }

   size_t
   CacheContainer::GetDistributionListCacheMaxSize()
   {
      return Cache<DistributionList, PersistentDistributionList>::Instance()->GetMaxSize();
   }

   void 
   CacheContainer::OnPropertyChanged(std::shared_ptr<Property> pProperty)
   {
      String sPropName = pProperty->GetName();

      if (sPropName == PROPERTY_USECACHE)
      {
         bool bMainCacheEnabled = pProperty->GetBoolValue();

         Cache<Domain, PersistentDomain>::Instance()->SetEnabled(bMainCacheEnabled);
         Cache<Account, PersistentAccount>::Instance()->SetEnabled(bMainCacheEnabled);
         Cache<Alias, PersistentAlias>::Instance()->SetEnabled(bMainCacheEnabled);
         Cache<DistributionList, PersistentDistributionList>::Instance()->SetEnabled(bMainCacheEnabled);
         Cache<Group, PersistentGroup>::Instance()->SetEnabled(bMainCacheEnabled);
      }
      else if (sPropName == PROPERTY_DOMAINCACHETTL)
         Cache<Domain, PersistentDomain>::Instance()->SetTTL(pProperty->GetLongValue());       
      else if (sPropName == PROPERTY_ACCOUNTCACHETTL)
         Cache<Account, PersistentAccount>::Instance()->SetTTL(pProperty->GetLongValue());
      else if (sPropName == PROPERTY_ALIASCACHETTL)
         Cache<Alias, PersistentAlias>::Instance()->SetTTL(pProperty->GetLongValue());
      else if (sPropName == PROPERTY_DISTRIBUTIONLISTCACHETTL)
         Cache<DistributionList, PersistentDistributionList>::Instance()->SetTTL(pProperty->GetLongValue());      
      else if (sPropName == PROPERTY_GROUPCACHETTL)
         Cache<Group, PersistentGroup>::Instance()->SetTTL(pProperty->GetLongValue());      

   }

   InboxIDCache &
   CacheContainer::GetInboxIDCache()
   {
      return inbox_idcache_;
   }

   void 
   CacheContainer::Clear()
   {
      inbox_idcache_.Clear();
   }

}