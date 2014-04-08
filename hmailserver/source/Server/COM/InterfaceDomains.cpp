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
InterfaceDomains::SetAuthentication(shared_ptr<HM::COMAuthentication> pAuthentication)
{
   COMAuthenticator::SetAuthentication(pAuthentication);
}

STDMETHODIMP InterfaceDomains::Refresh()
{
   try
   {
      if (!m_pAuthentication->GetIsAuthenticated())
         return m_pAuthentication->GetAccessDenied();
   
      m_pObjects = shared_ptr<HM::Domains>(new HM::Domains);
   
      try
      {
         if (m_pAuthentication->GetIsServerAdmin())
            // Load all domains
            m_pObjects->Refresh();
         else
            m_pObjects->Refresh(m_pAuthentication->GetDomainID());
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
      if (!m_pObjects)
         return GetAccessDenied();

      if (!m_pAuthentication->GetIsAuthenticated())
         return m_pAuthentication->GetAccessDenied();
   
      *pVal = m_pObjects->GetCount();
   
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
      if (!m_pAuthentication->GetIsServerAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      if (!m_pObjects)
         return m_pAuthentication->GetAccessDenied();
   
      CComObject<InterfaceDomain>* pDomainInterface = new CComObject<InterfaceDomain>;
      pDomainInterface->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::Domain> pDomain = shared_ptr<HM::Domain>(new HM::Domain());
      
      pDomainInterface->AttachItem(pDomain);
      pDomainInterface->AttachParent(m_pObjects, false);
      pDomainInterface->SetAuthentication(m_pAuthentication);
   
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
      if (!m_pObjects)
         return GetAccessDenied();

      if (!m_pAuthentication->GetIsAuthenticated())
         return m_pAuthentication->GetAccessDenied();
   
      CComObject<InterfaceDomain>* pDomain = new CComObject<InterfaceDomain>();
      pDomain->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::Domain> pPersDomain = m_pObjects->GetItemByName(ItemName);
   
      if (!pPersDomain)
         return DISP_E_BADINDEX;
   
      pDomain->AttachItem(pPersDomain);
      pDomain->AttachParent(m_pObjects, true);
      pDomain->SetAuthentication(m_pAuthentication);
   
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
      if (!m_pObjects)
         return GetAccessDenied();

      if (!m_pAuthentication->GetIsAuthenticated())
         return m_pAuthentication->GetAccessDenied();
   
      CComObject<InterfaceDomain>* pDomain = new CComObject<InterfaceDomain>();
      pDomain->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::Domain> pPersDomain = m_pObjects->GetItem(Index);
   
      if (!pPersDomain)
         return DISP_E_BADINDEX;
   
      pDomain->SetAuthentication(m_pAuthentication);
      pDomain->AttachItem(pPersDomain);
      pDomain->AttachParent(m_pObjects, true);
   
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
      if (!m_pObjects)
         return GetAccessDenied();

      if (!m_pAuthentication->GetIsAuthenticated())
         return m_pAuthentication->GetAccessDenied();
   
      CComObject<InterfaceDomain>* pDomain = new CComObject<InterfaceDomain>();
      pDomain->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::Domain> pPersDomain = m_pObjects->GetItemByDBID(DBID);
   
      if (!pPersDomain)
         return DISP_E_BADINDEX;
   
      pDomain->SetAuthentication(m_pAuthentication);
      pDomain->AttachItem(pPersDomain);
      pDomain->AttachParent(m_pObjects, true);
   
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
      if (!m_pObjects)
         return GetAccessDenied();

      if (!m_pAuthentication->GetIsAuthenticated())
         return m_pAuthentication->GetAccessDenied();
   
      HM::String names = m_pObjects->GetNames();
   
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
      if (!m_pObjects)
         return GetAccessDenied();

      if (!m_pAuthentication->GetIsServerAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      m_pObjects->DeleteItemByDBID(DBID);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

