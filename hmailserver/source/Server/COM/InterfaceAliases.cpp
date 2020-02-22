// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"

#include "InterfaceAliases.h"
#include "InterfaceAlias.h"

void
InterfaceAliases::Attach(std::shared_ptr<HM::Aliases> pAliases)
{
   aliases_ = pAliases;
} 

STDMETHODIMP InterfaceAliases::get_Count(long *pVal)
{
   try
   {
      if (!aliases_)
         return GetAccessDenied();

      *pVal = (int) aliases_->GetCount();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAliases::Delete(long Index)
{
   try
   {
      if (!aliases_)
         return GetAccessDenied();

      aliases_->DeleteItem(Index);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAliases::Refresh()
{
   try
   {
      if (!aliases_)
         return GetAccessDenied();

      aliases_->Refresh();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAliases::get_Item(long Index, IInterfaceAlias **pVal)
{
   try
   {
      if (!aliases_)
         return GetAccessDenied();

      CComObject<InterfaceAlias>* pAlias = new CComObject<InterfaceAlias>();
      pAlias->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::Alias> pPersAlias = aliases_->GetItem(Index);
   
      if (!pPersAlias)
         return DISP_E_BADINDEX;  
   
      pAlias->AttachItem(pPersAlias);
      pAlias->AddRef();
      *pVal = pAlias;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAliases::Add(IInterfaceAlias **pVal)
{
   try
   {
      if (!aliases_)
         return GetAccessDenied();

      if (!authentication_->GetIsDomainAdmin())
         return authentication_->GetAccessDenied();
   
      if (!aliases_)
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceAlias>* pIntAlias = new CComObject<InterfaceAlias>();
      pIntAlias->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::Alias> pAliasADO = std::shared_ptr<HM::Alias>(new HM::Alias);
      
      pIntAlias->AttachItem(pAliasADO);
      pIntAlias->AttachParent(aliases_, false);
   
      pAliasADO->SetDomainID(domain_id_);
   
      pIntAlias->AddRef();
      *pVal = pIntAlias;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAliases::get_ItemByDBID(long DBID, IInterfaceAlias **pVal)
{
   try
   {
      if (!aliases_)
         return GetAccessDenied();

      CComObject<InterfaceAlias>* pAlias = new CComObject<InterfaceAlias>();
      pAlias->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::Alias> pPersAlias = aliases_->GetItemByDBID(DBID);
   
      if (pPersAlias)
      {
         pAlias->AttachItem(pPersAlias);
         pAlias->AddRef();
         *pVal = pAlias;
      }
      else
      {
         return DISP_E_BADINDEX;  
      }
   
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAliases::DeleteByDBID(long DBID)
{
   try
   {
      if (!aliases_)
         return GetAccessDenied();

   
      if (aliases_)
         aliases_->DeleteItemByDBID(DBID);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAliases::get_ItemByName(BSTR Name, IInterfaceAlias **pVal)
{
   try
   {
      if (!aliases_)
         return GetAccessDenied();

      CComObject<InterfaceAlias>* pAlias = new CComObject<InterfaceAlias>();
      pAlias->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::Alias> pPersAlias = aliases_->GetItemByName(Name);
   
      if (pPersAlias)
      {
         pAlias->AttachItem(pPersAlias);
         pAlias->AddRef();
         *pVal = pAlias;
      }
      else
      {
         return DISP_E_BADINDEX;  
      }
   
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


