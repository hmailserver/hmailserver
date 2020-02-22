// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceCache.h"

#include "..\Common\Cache\Cache.h"
#include "..\Common\Cache\CacheConfiguration.h"
#include "..\Common\Cache\CacheContainer.h"

#include "..\Common\BO\Domain.h"   
#include "..\Common\BO\Account.h"
#include "..\Common\BO\Alias.h"
#include "..\Common\BO\DistributionList.h"

#include "../IMAP/MessagesContainer.h"

InterfaceCache::InterfaceCache() :
   config_(nullptr),
   cache_config_(nullptr)
{

}

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
   
      *pVal = HM::Cache<HM::Domain>::Instance()->GetHitRate();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


STDMETHODIMP
InterfaceCache::get_DomainCacheSizeKb(long *pVal)
{
   try
   {
      if (!cache_config_)
         return GetAccessDenied();

      *pVal = (int) HM::CacheContainer::Instance()->GetDomainCacheSize() / 1024;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


STDMETHODIMP
InterfaceCache::put_DomainCacheMaxSizeKb(long pVal)
{
   try
   {
      if (!cache_config_)
         return GetAccessDenied();

      HM::CacheContainer::Instance()->SetDomainCacheMaxSize(pVal * 1024);

      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP
InterfaceCache::get_DomainCacheMaxSizeKb(long *pVal)
{
   try
   {
      if (!cache_config_)
         return GetAccessDenied();

      *pVal = (int)HM::CacheContainer::Instance()->GetDomainCacheMaxSize() / 1024;
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
   
      *pVal = HM::Cache<HM::Account>::Instance()->GetHitRate();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP
InterfaceCache::get_AccountCacheSizeKb(long *pVal)
{
   try
   {
      if (!cache_config_)
         return GetAccessDenied();

      *pVal = (int)HM::CacheContainer::Instance()->GetAccountCacheSize() / 1024;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


STDMETHODIMP
InterfaceCache::put_AccountCacheMaxSizeKb(long pVal)
{
   try
   {
      if (!cache_config_)
         return GetAccessDenied();

      HM::CacheContainer::Instance()->SetAccountCacheMaxSize(pVal * 1024);

      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP
InterfaceCache::get_AccountCacheMaxSizeKb(long *pVal)
{
   try
   {
      if (!cache_config_)
         return GetAccessDenied();

      *pVal = (int)HM::CacheContainer::Instance()->GetAccountCacheMaxSize() / 1024;
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
   
      *pVal = HM::Cache<HM::Alias>::Instance()->GetHitRate();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


STDMETHODIMP
InterfaceCache::get_AliasCacheSizeKb(long *pVal)
{
   try
   {
      if (!cache_config_)
         return GetAccessDenied();

      *pVal = (int)HM::CacheContainer::Instance()->GetAliasCacheSize() / 1024;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP
InterfaceCache::put_AliasCacheMaxSizeKb(long pVal)
{
   try
   {
      if (!cache_config_)
         return GetAccessDenied();

      HM::CacheContainer::Instance()->SetAliasCacheMaxSize(pVal * 1024);

      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP
InterfaceCache::get_AliasCacheMaxSizeKb(long *pVal)
{
   try
   {
      if (!cache_config_)
         return GetAccessDenied();

      *pVal = (int)HM::CacheContainer::Instance()->GetAliasCacheMaxSize() / 1024;
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
   
      *pVal = HM::Cache<HM::DistributionList>::Instance()->GetHitRate();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP
InterfaceCache::get_DistributionListCacheSizeKb(long *pVal)
{
   try
   {
      if (!cache_config_)
         return GetAccessDenied();

      *pVal = (int)HM::CacheContainer::Instance()->GetDistributionListCacheSize() / 1024;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP
InterfaceCache::put_DistributionListCacheMaxSizeKb(long pVal)
{
   try
   {
      if (!cache_config_)
         return GetAccessDenied();

      HM::CacheContainer::Instance()->SetDistributionListCacheMaxSize(pVal * 1024);

      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP
InterfaceCache::get_DistributionListCacheMaxSizeKb(long *pVal)
{
   try
   {
      if (!cache_config_)
         return GetAccessDenied();

      *pVal = (int)HM::CacheContainer::Instance()->GetDistributionListCacheMaxSize() / 1024;
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
   
      HM::Cache<HM::Account>::Instance()->Clear();
      HM::Cache<HM::Domain>::Instance()->Clear();
      HM::Cache<HM::Alias>::Instance()->Clear();
      HM::Cache<HM::DistributionList>::Instance()->Clear();
      HM::MessagesContainer::Instance()->Clear();

      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


