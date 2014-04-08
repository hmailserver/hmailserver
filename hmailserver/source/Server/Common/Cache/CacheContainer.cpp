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

   }

   CacheContainer::~CacheContainer(void)
   {

   }

   void 
   CacheContainer::CreateCaches()
   {
      Cache<Domain, PersistentDomain>::CreateInstance();
      Cache<Account, PersistentAccount>::CreateInstance();
      Cache<Alias, PersistentAlias>::CreateInstance();
      Cache<DistributionList, PersistentDistributionList>::CreateInstance();
      Cache<Group, PersistentGroup>::CreateInstance();
   }

   void 
   CacheContainer::DeleteCaches()
   {
      Cache<Group, PersistentGroup>::DeleteInstance();
      Cache<Domain, PersistentDomain>::DeleteInstance();
      Cache<Account, PersistentAccount>::DeleteInstance();
      Cache<Alias, PersistentAlias>::DeleteInstance();
      Cache<DistributionList, PersistentDistributionList>::DeleteInstance();
   }

   shared_ptr<const Account> 
   CacheContainer::GetAccount(const String &sName)
   {
      return Cache<Account, PersistentAccount>::Instance()->GetObject(sName);
   }

   shared_ptr<const Account> 
   CacheContainer::GetAccount(__int64 iID)
   {
      return Cache<Account, PersistentAccount>::Instance()->GetObject(iID);
   }

   shared_ptr<const Domain> 
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

   shared_ptr<const Domain> 
   CacheContainer::GetDomain(__int64 iID)
   {
      return Cache<Domain, PersistentDomain>::Instance()->GetObject(iID);
   }

   shared_ptr<const Alias> 
   CacheContainer::GetAlias(const String &sName)
   {
      return Cache<Alias, PersistentAlias>::Instance()->GetObject(sName);
   }

   shared_ptr<const Alias> 
   CacheContainer::GetAlias(__int64 iID)
   {
      return Cache<Alias, PersistentAlias>::Instance()->GetObject(iID);
   }

   shared_ptr<const DistributionList> 
   CacheContainer::GetDistributionList(const String &sName)
   {
      return Cache<DistributionList, PersistentDistributionList>::Instance()->GetObject(sName);
   }

   shared_ptr<const DistributionList> 
   CacheContainer::GetDistributionList(__int64 iID)
   {
      return Cache<DistributionList, PersistentDistributionList>::Instance()->GetObject(iID);
   }

   void 
   CacheContainer::OnPropertyChanged(shared_ptr<Property> pProperty)
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
      return _inboxIDCache;
   }

   void 
   CacheContainer::Clear()
   {
      _inboxIDCache.Clear();
   }

}