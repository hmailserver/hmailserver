// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceDomainAliases.h"

#include "InterfaceDomainAlias.h"

#include "../Common/BO/DomainAliases.h"
#include "../Common/BO/DomainAlias.h"

void 
InterfaceDomainAliases::Attach(shared_ptr<HM::DomainAliases> pDomainAliases)
{
   m_pDomainAliases = pDomainAliases;
}

STDMETHODIMP InterfaceDomainAliases::get_ItemByDBID(long lDBID, IInterfaceDomainAlias** pVal)
{
   try
   {
      if (!m_pDomainAliases)
         return GetAccessDenied();

      CComObject<InterfaceDomainAlias>* pInterfaceDA = new CComObject<InterfaceDomainAlias>();
      pInterfaceDA->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::DomainAlias> pDomainAlias = m_pDomainAliases->GetItemByDBID(lDBID);
      if (!pDomainAlias)
         return DISP_E_BADINDEX;
   
      pInterfaceDA->AttachItem(pDomainAlias);
      pInterfaceDA->AttachParent(m_pDomainAliases, true);
      pInterfaceDA->AddRef();
      *pVal = pInterfaceDA;   
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDomainAliases::get_Item(long lIndex, IInterfaceDomainAlias** pVal)
{
   try
   {
      if (!m_pDomainAliases)
         return GetAccessDenied();

      CComObject<InterfaceDomainAlias>* pInterfaceAccount = new CComObject<InterfaceDomainAlias>();
      pInterfaceAccount->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::DomainAlias> pDomainAlias = m_pDomainAliases->GetItem(lIndex);
      if (!pDomainAlias)
         return DISP_E_BADINDEX;
   
      pInterfaceAccount->AttachItem(pDomainAlias);
      pInterfaceAccount->AttachParent(m_pDomainAliases, true);
      pInterfaceAccount->AddRef();
      *pVal = pInterfaceAccount;   
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDomainAliases::Add(IInterfaceDomainAlias **pVal)
{
   try
   {
      if (!m_pDomainAliases)
         return GetAccessDenied();

      if (!m_pDomainAliases)
         return m_pAuthentication->GetAccessDenied();
   
      CComObject<InterfaceDomainAlias>* pIntDA = new CComObject<InterfaceDomainAlias>();
      pIntDA->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::DomainAlias> pDA = shared_ptr<HM::DomainAlias>(new HM::DomainAlias);
   
      pDA->SetDomainID(m_pDomainAliases->GetDomainID());
   
      pIntDA->AttachItem(pDA);
      pIntDA->AttachParent(m_pDomainAliases, false);
   
      pIntDA->AddRef();
   
      *pVal = pIntDA;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDomainAliases::Refresh(void)
{
   try
   {
      if (!m_pDomainAliases)
         return GetAccessDenied();

      m_pDomainAliases->Refresh();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDomainAliases::Delete(LONG Index)
{
   try
   {
      if (!m_pDomainAliases)
         return GetAccessDenied();

      m_pDomainAliases->DeleteItem(Index);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDomainAliases::DeleteByDBID(LONG DBID)
{
   try
   {
      if (!m_pDomainAliases)
         return GetAccessDenied();

      m_pDomainAliases->DeleteItemByDBID(DBID);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDomainAliases::get_Count(long *pVal)
{
   try
   {
      if (!m_pDomainAliases)
         return GetAccessDenied();

      *pVal = m_pDomainAliases->GetCount();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


