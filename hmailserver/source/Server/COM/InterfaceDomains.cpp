// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "InterfaceDomains.h"

#include "../Common/BO/Domains.h"

#include "../Common/Persistence/PersistentDomain.h"

#include "InterfaceDomain.h"

#include "COMAuthentication.h"
#include "COMError.h"

#ifdef _DEBUG
long InterfaceDomains::counter = 0;
#endif

STDMETHODIMP InterfaceDomains::InterfaceSupportsErrorInfo(REFIID riid)
{
   try
   {
      static const IID* arr[] = 
      {
         &IID_IInterfaceDomains,
      };
   
      for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
      {
         if (InlineIsEqualGUID(*arr[i],riid))
            return S_OK;
      }
      return S_FALSE;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}   

void
InterfaceDomains::SetAuthentication(std::shared_ptr<HM::COMAuthentication> pAuthentication)
{
   COMAuthenticator::SetAuthentication(pAuthentication);
}

STDMETHODIMP InterfaceDomains::Refresh()
{
   try
   {
      if (!authentication_->GetIsAuthenticated())
         return authentication_->GetAccessDenied();
   
      objects_ = std::shared_ptr<HM::Domains>(new HM::Domains);
   
      try
      {
         if (authentication_->GetIsServerAdmin())
            // Load all domains
            objects_->Refresh();
         else
            objects_->Refresh(authentication_->GetDomainID());
      }
      catch (...)
      {
         return COMError::GenerateError("It was not possible to retrieve a list of domains from the database.\r\nPlease check the hMailServer log for details.");         	
      }
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDomains::get_Count(long *pVal)
{
   try
   {
      if (!objects_)
         return GetAccessDenied();

      if (!authentication_->GetIsAuthenticated())
         return authentication_->GetAccessDenied();
   
      *pVal = objects_->GetCount();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDomains::Add(/*[out, retval] */IInterfaceDomain** pVal)
{
   try
   {
      if (!authentication_->GetIsServerAdmin())
         return authentication_->GetAccessDenied();
   
      if (!objects_)
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceDomain>* pDomainInterface = new CComObject<InterfaceDomain>;
      pDomainInterface->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::Domain> pDomain = std::shared_ptr<HM::Domain>(new HM::Domain());
      
      pDomainInterface->AttachItem(pDomain);
      pDomainInterface->AttachParent(objects_, false);
      pDomainInterface->SetAuthentication(authentication_);
   
      pDomainInterface->AddRef();
   
      *pVal = pDomainInterface;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDomains::get_ItemByName(BSTR ItemName, IInterfaceDomain **pVal)
{
   try
   {
      if (!objects_)
         return GetAccessDenied();

      if (!authentication_->GetIsAuthenticated())
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceDomain>* pDomain = new CComObject<InterfaceDomain>();
      pDomain->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::Domain> pPersDomain = objects_->GetItemByName(ItemName);
   
      if (!pPersDomain)
         return DISP_E_BADINDEX;
   
      pDomain->AttachItem(pPersDomain);
      pDomain->AttachParent(objects_, true);
      pDomain->SetAuthentication(authentication_);
   
      pDomain->AddRef();
      *pVal = pDomain;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDomains::get_Item(long Index, IInterfaceDomain **pVal)
{
   try
   {
      if (!objects_)
         return GetAccessDenied();

      if (!authentication_->GetIsAuthenticated())
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceDomain>* pDomain = new CComObject<InterfaceDomain>();
      pDomain->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::Domain> pPersDomain = objects_->GetItem(Index);
   
      if (!pPersDomain)
         return DISP_E_BADINDEX;
   
      pDomain->SetAuthentication(authentication_);
      pDomain->AttachItem(pPersDomain);
      pDomain->AttachParent(objects_, true);
   
      pDomain->AddRef();
   
      *pVal = pDomain;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDomains::get_ItemByDBID(long DBID, IInterfaceDomain **pVal)
{
   try
   {
      if (!objects_)
         return GetAccessDenied();

      if (!authentication_->GetIsAuthenticated())
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceDomain>* pDomain = new CComObject<InterfaceDomain>();
      pDomain->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::Domain> pPersDomain = objects_->GetItemByDBID(DBID);
   
      if (!pPersDomain)
         return DISP_E_BADINDEX;
   
      pDomain->SetAuthentication(authentication_);
      pDomain->AttachItem(pPersDomain);
      pDomain->AttachParent(objects_, true);
   
      pDomain->AddRef();
      *pVal = pDomain;
      
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDomains::get_Names(BSTR *sNames)
{
   try
   {
      if (!objects_)
         return GetAccessDenied();

      if (!authentication_->GetIsAuthenticated())
         return authentication_->GetAccessDenied();
   
      HM::String names = objects_->GetNames();
   
      *sNames = names.AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDomains::DeleteByDBID(long DBID)
{
   try
   {
      if (!objects_)
         return GetAccessDenied();

      if (!authentication_->GetIsServerAdmin())
         return authentication_->GetAccessDenied();
   
      objects_->DeleteItemByDBID(DBID);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

