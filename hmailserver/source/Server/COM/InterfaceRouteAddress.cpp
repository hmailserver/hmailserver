// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "InterfaceRouteAddress.h"

#include "../Common/Persistence/PersistentRouteAddress.h"
#include "../Common/BO/RouteAddresses.h"
#include "../Common/BO/RouteAddress.h"

#include "COMError.h"

STDMETHODIMP 
InterfaceRouteAddress::get_ID(long *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = (long) object_->GetID();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceRouteAddress::get_RouteID(long *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = (long) object_->GetRouteID();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceRouteAddress::put_RouteID(long newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetRouteID(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceRouteAddress::get_Address(BSTR *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

   
   
      *pVal = object_->GetAddress().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceRouteAddress::put_Address(BSTR newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetAddress(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceRouteAddress::Save()
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      if (HM::PersistentRouteAddress::SaveObject(object_))
      {
         // Add to parent collection
         AddToParentCollection();
   
         return S_OK;
      }
   
      return COMError::GenerateError("Failed to save object. See hMailServer error log.");
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRouteAddress::Delete()
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      if (!authentication_->GetIsServerAdmin())
         return authentication_->GetAccessDenied();
   
      if (!parent_collection_)
         return HM::PersistentRouteAddress::DeleteObject(object_) ? S_OK : S_FALSE;
   
      parent_collection_->DeleteItemByDBID(object_->GetID());
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

