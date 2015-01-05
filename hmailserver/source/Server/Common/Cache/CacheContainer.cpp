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

#include "CacheReaderWithDbFallback.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   CacheContainer::CacheContainer(void)
   {
      // Limit the cache of each entity to 10MB each.
      Cache<Domain>::Instance()->SetMaxSize(10 * 1024 * 1024);
      Cache<Domain>::Instance()->SetMaxSize(10 * 1024 * 1024);
      Cache<Alias>::Instance()->SetMaxSize(10 * 1024 * 1024);
      Cache<DistributionList>::Instance()->SetMaxSize(10 * 1024 * 1024);


      // When the server is stopped, the cache should be cleared.
      Application::Instance()->OnServerStopped.connect
         (
         [this]() 
            { 
               Cache<Domain>::Instance()->Clear(); 
               Cache<Domain>::Instance()->Clear();
               Cache<Alias>::Instance()->Clear();
               Cache<DistributionList>::Instance()->Clear();
            }
      );
   }

   CacheContainer::~CacheContainer(void)
   {

   }



   std::shared_ptr<const Account> 
   CacheContainer::GetAccount(__int64 iID)
   {
      CacheReaderWithDbFallback<Account, PersistentAccount, __int64> reader;
      return reader.GetObject(iID);
   }

   std::shared_ptr<const Account>
   CacheContainer::GetAccount(const String &sName)
   {
      CacheReaderWithDbFallback<Account, PersistentAccount, String> reader;
      return reader.GetObject(sName);
   }


   size_t
   CacheContainer::GetAccountCacheSize()
   {
      return Cache<Account>::Instance()->GetSize();
   }

   void
   CacheContainer::SetAccountCacheMaxSize(size_t max_size)
   {
      return Cache<Account>::Instance()->SetMaxSize(max_size);
   }

   size_t
   CacheContainer::GetAccountCacheMaxSize()
   {
      return Cache<Account>::Instance()->GetMaxSize();
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
      
      CacheReaderWithDbFallback<Domain, PersistentDomain, String> reader;
      auto domain = reader.GetObject(sName);

      return domain;
   }

   std::shared_ptr<const Domain> 
   CacheContainer::GetDomain(__int64 iID)
   {
      CacheReaderWithDbFallback<Domain, PersistentDomain, __int64> reader;
      return reader.GetObject(iID);
   }
   
   size_t
   CacheContainer::GetDomainCacheSize()
   {
      return Cache<Domain>::Instance()->GetSize();
   }

   void
   CacheContainer::SetDomainCacheMaxSize(size_t max_size)
   {
      return Cache<Domain>::Instance()->SetMaxSize(max_size);
   }

   size_t
   CacheContainer::GetDomainCacheMaxSize()
   {
      return Cache<Domain>::Instance()->GetMaxSize();
   }

   void
   CacheContainer::RemoveDomain(std::shared_ptr<Domain> domain)
   {
      return Cache<Domain>::Instance()->RemoveObject(domain);
   }

   std::shared_ptr<const Alias> 
   CacheContainer::GetAlias(const String &sName)
   {
      CacheReaderWithDbFallback<Alias, PersistentAlias, String> reader;
      return reader.GetObject(sName);
   }

   std::shared_ptr<const Alias> 
   CacheContainer::GetAlias(__int64 iID)
   {
      CacheReaderWithDbFallback<Alias, PersistentAlias, __int64> reader;
      return reader.GetObject(iID);
   }

   size_t
   CacheContainer::GetAliasCacheSize()
   {
      return Cache<Alias>::Instance()->GetSize();
   }

   void
   CacheContainer::SetAliasCacheMaxSize(size_t max_size)
   {
      return Cache<Alias>::Instance()->SetMaxSize(max_size);
   }

   size_t
   CacheContainer::GetAliasCacheMaxSize()
   {
      return Cache<Alias>::Instance()->GetMaxSize();
   }

   std::shared_ptr<const DistributionList> 
   CacheContainer::GetDistributionList(const String &sName)
   {
      CacheReaderWithDbFallback<DistributionList, PersistentDistributionList, String> reader;
      return reader.GetObject(sName);
   }

   std::shared_ptr<const DistributionList> 
   CacheContainer::GetDistributionList(__int64 iID)
   {
      CacheReaderWithDbFallback<DistributionList, PersistentDistributionList, __int64> reader;
      return reader.GetObject(iID);
   }

   size_t
   CacheContainer::GetDistributionListCacheSize()
   {
      return Cache<DistributionList>::Instance()->GetSize();
   }

   void
   CacheContainer::SetDistributionListCacheMaxSize(size_t max_size)
   {
      return Cache<DistributionList>::Instance()->SetMaxSize (max_size);
   }

   size_t
   CacheContainer::GetDistributionListCacheMaxSize()
   {
      return Cache<DistributionList>::Instance()->GetMaxSize();
   }

   void 
   CacheContainer::OnPropertyChanged(std::shared_ptr<Property> pProperty)
   {
      String sPropName = pProperty->GetName();

      if (sPropName == PROPERTY_USECACHE)
      {
         bool bMainCacheEnabled = pProperty->GetBoolValue();

         Cache<Domain>::Instance()->SetEnabled(bMainCacheEnabled);
         Cache<Account>::Instance()->SetEnabled(bMainCacheEnabled);
         Cache<Alias>::Instance()->SetEnabled(bMainCacheEnabled);
         Cache<DistributionList>::Instance()->SetEnabled(bMainCacheEnabled);
      }
      else if (sPropName == PROPERTY_DOMAINCACHETTL)
         Cache<Domain>::Instance()->SetTTL(pProperty->GetLongValue());       
      else if (sPropName == PROPERTY_ACCOUNTCACHETTL)
         Cache<Account>::Instance()->SetTTL(pProperty->GetLongValue());
      else if (sPropName == PROPERTY_ALIASCACHETTL)
         Cache<Alias>::Instance()->SetTTL(pProperty->GetLongValue());
      else if (sPropName == PROPERTY_DISTRIBUTIONLISTCACHETTL)
         Cache<DistributionList>::Instance()->SetTTL(pProperty->GetLongValue());      
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