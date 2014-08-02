// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceCache.h"

#include "..\Common\Cache\Cache.h"
#include "..\Common\Cache\CacheConfiguration.h"

#include "..\Common\BO\Domain.h"   
#include "..\Common\BO\Account.h"
#include "..\Common\BO\Alias.h"
#include "..\Common\BO\DistributionList.h"

#include "..\Common\Persistence\PersistentDomain.h"
#include "..\Common\Persistence\PersistentAccount.h"
#include "..\Common\Persistence\PersistentAlias.h"
#include "..\Common\Persistence\PersistentDistributionList.h"

#include "..\Common\BO\Routes.h"
#include "..\Common\BO\DNSBlackLists.h"
#include "..\Common\BO\SURBLServers.h"
#include "..\Common\BO\BlockedAttachments.h"
#include "..\Common\BO\GreyListingWhiteAddresses.h"

bool 
InterfaceCache::LoadSettings()
{
   if (!GetIsServerAdmin())
      return false;

   config_ = HM::Configuration::Instance();
   cache_config_ = config_->GetCacheConfiguration();

   return true;
}


STDMETHODIMP
InterfaceCache::get_Enabled(VARIANT_BOOL *pVal)
{
   try
   {
      if (!cache_config_)
         return GetAccessDenied();

      *pVal = cache_config_->GetUseCache() ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP
InterfaceCache::put_Enabled(VARIANT_BOOL newVal)
{
   try
   {
      if (!cache_config_)
         return GetAccessDenied();

      cache_config_->SetUseCache(newVal == VARIANT_TRUE);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceCache::get_DomainCacheTTL(long *pVal)
{
   try
   {
      if (!cache_config_)
         return GetAccessDenied();

      *pVal = cache_config_->GetDomainCacheTTL();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceCache::put_DomainCacheTTL(long newVal)
{
   try
   {
      if (!cache_config_)
         return GetAccessDenied();

      cache_config_->SetDomainCacheTTL(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceCache::get_DomainHitRate(long *pVal)
{
   try
   {
      if (!cache_config_)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return false;
   
      *pVal = HM::Cache<HM::Domain, HM::PersistentDomain>::Instance()->GetHitRate();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceCache::get_AccountCacheTTL(long *pVal)
{
   try
   {
      if (!cache_config_)
         return GetAccessDenied();

      *pVal = cache_config_->GetAccountCacheTTL();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceCache::put_AccountCacheTTL(long newVal)
{
   try
   {
      if (!cache_config_)
         return GetAccessDenied();

      cache_config_->SetAccountCacheTTL(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceCache::get_AccountHitRate(long *pVal)
{
   try
   {
      if (!cache_config_)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return false;
   
      *pVal = HM::Cache<HM::Account, HM::PersistentAccount>::Instance()->GetHitRate();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceCache::get_AliasCacheTTL(long *pVal)
{
   try
   {
      if (!cache_config_)
         return GetAccessDenied();

      *pVal = cache_config_->GetAliasCacheTTL();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceCache::put_AliasCacheTTL(long newVal)
{
   try
   {
      if (!cache_config_)
         return GetAccessDenied();

      cache_config_->SetAliasCacheTTL(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceCache::get_AliasHitRate(long *pVal)
{
   try
   {
      if (!cache_config_)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return false;
   
      *pVal = HM::Cache<HM::Alias, HM::PersistentAlias>::Instance()->GetHitRate();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceCache::get_DistributionListCacheTTL(long *pVal)
{
   try
   {
      if (!cache_config_)
         return GetAccessDenied();

      *pVal = cache_config_->GetDistributionListCacheTTL();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceCache::put_DistributionListCacheTTL(long newVal)
{
   try
   {
      if (!cache_config_)
         return GetAccessDenied();

      cache_config_->SetDistributionListCacheTTL(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceCache::get_DistributionListHitRate(long *pVal)
{
   try
   {
      if (!cache_config_)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return false;
   
      *pVal = HM::Cache<HM::DistributionList, HM::PersistentDistributionList>::Instance()->GetHitRate();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceCache::Clear()
{
   try
   {
      if (!cache_config_)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return false;
   
      HM::Cache<HM::Account, HM::PersistentAccount>::Instance()->Clear();
      HM::Cache<HM::Domain, HM::PersistentDomain>::Instance()->Clear();
      HM::Cache<HM::Alias, HM::PersistentAlias>::Instance()->Clear();
      HM::Cache<HM::DistributionList, HM::PersistentDistributionList>::Instance()->Clear();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


