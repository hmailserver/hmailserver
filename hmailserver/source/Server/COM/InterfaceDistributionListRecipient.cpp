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
      if (!m_pObject)
         return GetAccessDenied();

   
   
      *pVal = (long) m_pObject->GetID();
   
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
      if (!m_pObject)
         return GetAccessDenied();

   
   
      m_pObject->SetID(newVal);
   
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
      if (!m_pObject)
         return GetAccessDenied();

   
   
      HM::String sVal = m_pObject->GetAddress();
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
      if (!m_pObject)
         return GetAccessDenied();

   
   
      HM::String sNewVal = newVal;
      m_pObject->SetAddress(sNewVal);
   
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
      if (!m_pObject)
         return GetAccessDenied();

      if (!m_pObject)
         return S_FALSE;
   
      m_pParentCollection->DeleteItemByDBID(m_pObject->GetID());
   
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
      if (!m_pObject)
         return GetAccessDenied();

      if (!m_pObject)
         return S_FALSE;
   
      HM::String sErrorMessage;
      if (HM::PersistentDistributionListRecipient::SaveObject(m_pObject, sErrorMessage))
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

