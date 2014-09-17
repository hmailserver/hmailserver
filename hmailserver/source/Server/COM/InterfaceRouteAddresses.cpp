// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceRouteAddresses.h"
#include "InterfaceRouteAddress.h"

void
InterfaceRouteAddresses::Attach(std::shared_ptr<HM::RouteAddresses> pRouteAddresses)
{
   route_addresses_ = pRouteAddresses;
} 

STDMETHODIMP 
InterfaceRouteAddresses::get_Count(long *pVal)
{
   try
   {
      if (!route_addresses_)
         return GetAccessDenied();

      *pVal = route_addresses_->GetCount();
   
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
      if (!route_addresses_)
         return GetAccessDenied();

      CComObject<InterfaceRouteAddress>* pInterfaceRouteAddress = new CComObject<InterfaceRouteAddress>();
      pInterfaceRouteAddress->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::RouteAddress> pRouteAddress = route_addresses_->GetItem(Index);
   
      if (!pRouteAddress)
         return DISP_E_BADINDEX;  
   
      pInterfaceRouteAddress->AttachItem(pRouteAddress);
      pInterfaceRouteAddress->AttachParent(route_addresses_, true);
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
      if (!route_addresses_)
         return GetAccessDenied();

      route_addresses_->DeleteItemByDBID(DBID);
   
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
      if (!route_addresses_)
         return GetAccessDenied();

      HM::String sAddress = bstrAddress;
      route_addresses_->DeleteByAddress(sAddress);
   
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
      if (!route_addresses_)
         return GetAccessDenied();

      if (!route_addresses_)
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceRouteAddress>* pRouteAddressInterface = new CComObject<InterfaceRouteAddress>();
      pRouteAddressInterface->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::RouteAddress> pRouteAddress = std::shared_ptr<HM::RouteAddress>(new HM::RouteAddress);
   
      // Set the ID of the new route.
      pRouteAddress->SetRouteID(route_addresses_->GetRouteID());
   
      pRouteAddressInterface->AttachParent(route_addresses_, false);
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
      if (!route_addresses_)
         return GetAccessDenied();

      CComObject<InterfaceRouteAddress>* pInterfaceRouteAddress = new CComObject<InterfaceRouteAddress>();
      pInterfaceRouteAddress->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::RouteAddress> pRouteAddress = route_addresses_->GetItemByDBID(lDBID);
   
      if (pRouteAddress)
      {
         pInterfaceRouteAddress->AttachItem(pRouteAddress);
         pInterfaceRouteAddress->AttachParent(route_addresses_, true);
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


