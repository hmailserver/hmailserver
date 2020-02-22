// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "InterfaceDomainAlias.h"

#include "../Common/BO/DomainAlias.h"
#include "../Common/BO/DomainAliases.h"

#include "../Common/Persistence/PersistentDomainAlias.h"

#include "COMError.h"


STDMETHODIMP InterfaceDomainAlias::InterfaceSupportsErrorInfo(REFIID riid)
{
   try
   {
      static const IID* arr[] = 
      {
         &IID_IInterfaceDomainAlias,
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

STDMETHODIMP InterfaceDomainAlias::get_ID(LONG* pVal)
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

STDMETHODIMP InterfaceDomainAlias::get_DomainID(LONG* pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = (long) object_->GetDomainID();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDomainAlias::get_AliasName(BSTR* pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetAlias().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDomainAlias::put_AliasName(BSTR newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetAlias(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDomainAlias::Save()
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      HM::String sErrorMessage;
      if (HM::PersistentDomainAlias::SaveObject(object_, sErrorMessage, HM::PersistenceModeNormal))
      {
         // Add to parent collection
         AddToParentCollection();
   
         return S_OK;
      }
   
      return COMError::GenerateError("Failed to save object. " +  sErrorMessage);
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDomainAlias::put_DomainID(LONG newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      // Only here for backwards compatibility (4.x)
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDomainAlias::Delete()
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      if (!authentication_->GetIsDomainAdmin())
         return authentication_->GetAccessDenied();
   
      if (!parent_collection_)
         return HM::PersistentDomainAlias::DeleteObject(object_) ? S_OK : S_FALSE;
   
      parent_collection_->DeleteItemByDBID(object_->GetID());
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

