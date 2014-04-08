// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceRouteAddresses.h"
#include "InterfaceRouteAddress.h"

void
InterfaceRouteAddresses::Attach(shared_ptr<HM::RouteAddresses> pRouteAddresses)
{
   m_pRouteAddresses = pRouteAddresses;
} 

STDMETHODIMP 
InterfaceRouteAddresses::get_Count(long *pVal)
{
   try
   {
      if (!m_pRouteAddresses)
         return GetAccessDenied();

      *pVal = m_pRouteAddresses->GetCount();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceRouteAddresses::get_Item(long Index, IInterfaceRouteAddress **pVal)
{
   try
   {
      if (!m_pRouteAddresses)
         return GetAccessDenied();

      CComObject<InterfaceRouteAddress>* pInterfaceRouteAddress = new CComObject<InterfaceRouteAddress>();
      pInterfaceRouteAddress->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::RouteAddress> pRouteAddress = m_pRouteAddresses->GetItem(Index);
   
      if (!pRouteAddress)
         return DISP_E_BADINDEX;  
   
      pInterfaceRouteAddress->AttachItem(pRouteAddress);
      pInterfaceRouteAddress->AttachParent(m_pRouteAddresses, true);
      pInterfaceRouteAddress->AddRef();
      *pVal = pInterfaceRouteAddress;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceRouteAddresses::DeleteByDBID(long DBID)
{
   try
   {
      if (!m_pRouteAddresses)
         return GetAccessDenied();

      m_pRouteAddresses->DeleteItemByDBID(DBID);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceRouteAddresses::DeleteByAddress(BSTR bstrAddress)
{
   try
   {
      if (!m_pRouteAddresses)
         return GetAccessDenied();

      HM::String sAddress = bstrAddress;
      m_pRouteAddresses->DeleteByAddress(sAddress);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceRouteAddresses::Add(IInterfaceRouteAddress **pVal)
{
   try
   {
      if (!m_pRouteAddresses)
         return GetAccessDenied();

      if (!m_pRouteAddresses)
         return m_pAuthentication->GetAccessDenied();
   
      CComObject<InterfaceRouteAddress>* pRouteAddressInterface = new CComObject<InterfaceRouteAddress>();
      pRouteAddressInterface->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::RouteAddress> pRouteAddress = shared_ptr<HM::RouteAddress>(new HM::RouteAddress);
   
      // Set the ID of the new route.
      pRouteAddress->SetRouteID(m_pRouteAddresses->GetRouteID());
   
      pRouteAddressInterface->AttachParent(m_pRouteAddresses, false);
      pRouteAddressInterface->AttachItem(pRouteAddress);
   
      pRouteAddressInterface->AddRef();
      *pVal = pRouteAddressInterface;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceRouteAddresses::get_ItemByDBID(long lDBID, IInterfaceRouteAddress **pVal)
{
   try
   {
      if (!m_pRouteAddresses)
         return GetAccessDenied();

      CComObject<InterfaceRouteAddress>* pInterfaceRouteAddress = new CComObject<InterfaceRouteAddress>();
      pInterfaceRouteAddress->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::RouteAddress> pRouteAddress = m_pRouteAddresses->GetItemByDBID(lDBID);
   
      if (pRouteAddress)
      {
         pInterfaceRouteAddress->AttachItem(pRouteAddress);
         pInterfaceRouteAddress->AttachParent(m_pRouteAddresses, true);
         pInterfaceRouteAddress->AddRef();
         *pVal = pInterfaceRouteAddress;
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


