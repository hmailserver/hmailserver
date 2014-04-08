// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceWhiteListAddresses.h"


#include "InterfaceWhiteListAddress.h"

#include "..\Common\BO\WhiteListAddress.h"
#include "..\Common\BO\WhiteListAddresses.h"


void 
InterfaceWhiteListAddresses::Attach(shared_ptr<HM::WhiteListAddresses> pBA) 
{ 
   m_pObject = pBA; 
}

STDMETHODIMP 
InterfaceWhiteListAddresses::Refresh()
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      if (!m_pObject)
         return S_FALSE;
   
      m_pObject->Refresh();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceWhiteListAddresses::Clear()
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      if (!m_pObject)
         return S_FALSE;
   
      m_pObject->DeleteAll();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceWhiteListAddresses::get_Count(long *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = m_pObject->GetCount();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceWhiteListAddresses::get_Item(long Index, IInterfaceWhiteListAddress **pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      CComObject<InterfaceWhiteListAddress>* pInterfaceWhiteListAddress = new CComObject<InterfaceWhiteListAddress>();
      pInterfaceWhiteListAddress->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::WhiteListAddress> pBA = m_pObject->GetItem(Index);
   
      if (!pBA)
         return DISP_E_BADINDEX;
   
      pInterfaceWhiteListAddress->AttachItem(pBA);
      pInterfaceWhiteListAddress->AttachParent(m_pObject, true);
      pInterfaceWhiteListAddress->AddRef();
      *pVal = pInterfaceWhiteListAddress;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceWhiteListAddresses::DeleteByDBID(long DBID)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      m_pObject->DeleteItemByDBID(DBID);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceWhiteListAddresses::get_ItemByDBID(long lDBID, IInterfaceWhiteListAddress **pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      CComObject<InterfaceWhiteListAddress>* pInterfaceWhiteListAddress = new CComObject<InterfaceWhiteListAddress>();
      pInterfaceWhiteListAddress->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::WhiteListAddress> pBA = m_pObject->GetItemByDBID(lDBID);
   
      if (!pBA)
         return DISP_E_BADINDEX;
   
      pInterfaceWhiteListAddress->AttachItem(pBA);
      pInterfaceWhiteListAddress->AttachParent(m_pObject, true);
      pInterfaceWhiteListAddress->AddRef();
   
      *pVal = pInterfaceWhiteListAddress;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceWhiteListAddresses::get_ItemByName(BSTR sName, IInterfaceWhiteListAddress **pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      CComObject<InterfaceWhiteListAddress>* pInterfaceWhiteListAddress = new CComObject<InterfaceWhiteListAddress>();
      pInterfaceWhiteListAddress->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::WhiteListAddress> pBA = m_pObject->GetItemByName(sName);
   
      if (!pBA)
         return DISP_E_BADINDEX;
   
      pInterfaceWhiteListAddress->AttachItem(pBA);
      pInterfaceWhiteListAddress->AttachParent(m_pObject, true);
      pInterfaceWhiteListAddress->AddRef();
   
      *pVal = pInterfaceWhiteListAddress;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceWhiteListAddresses::Add(IInterfaceWhiteListAddress **pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      if (!m_pObject)
         return m_pAuthentication->GetAccessDenied();
   
      CComObject<InterfaceWhiteListAddress>* pInterfaceWhiteListAddress = new CComObject<InterfaceWhiteListAddress>();
      pInterfaceWhiteListAddress->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::WhiteListAddress> pBA = shared_ptr<HM::WhiteListAddress>(new HM::WhiteListAddress);
   
      pInterfaceWhiteListAddress->AttachItem(pBA);
      pInterfaceWhiteListAddress->AttachParent(m_pObject, false);
   
      pInterfaceWhiteListAddress->AddRef();
   
      *pVal = pInterfaceWhiteListAddress;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


