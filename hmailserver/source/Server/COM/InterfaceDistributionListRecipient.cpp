// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "InterfaceDistributionListRecipient.h"

#include "../common/persistence/PersistentDistributionListRecipient.h"

#include "../Common/BO/DistributionListRecipients.h"

#include "COMError.h"

STDMETHODIMP InterfaceDistributionListRecipient::InterfaceSupportsErrorInfo(REFIID riid)
{
   try
   {
      static const IID* arr[] = 
      {
         &IID_IInterfaceDistributionListRecipient,
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

STDMETHODIMP InterfaceDistributionListRecipient::get_ID(long *pVal)
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

STDMETHODIMP InterfaceDistributionListRecipient::put_ID(long newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

   
   
      object_->SetID(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDistributionListRecipient::get_RecipientAddress(BSTR *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

   
   
      HM::String sVal = object_->GetAddress();
      *pVal = sVal.AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDistributionListRecipient::put_RecipientAddress(BSTR newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

   
   
      HM::String sNewVal = newVal;
      object_->SetAddress(sNewVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDistributionListRecipient::Delete()
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      if (!object_)
         return S_FALSE;
   
      parent_collection_->DeleteItemByDBID(object_->GetID());
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDistributionListRecipient::Save()
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      if (!object_)
         return S_FALSE;
   
      HM::String sErrorMessage;
      if (HM::PersistentDistributionListRecipient::SaveObject(object_, sErrorMessage, HM::PersistenceModeNormal))
      {
         AddToParentCollection();
   
         return S_OK;
      }
   
      return COMError::GenerateError(sErrorMessage);
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

