// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "InterfaceRoute.h"

#include "InterfaceRouteAddresses.h"

#include "../Common/BO/Routes.h"

#include "COMError.h"

STDMETHODIMP InterfaceRoute::InterfaceSupportsErrorInfo(REFIID riid)
{
   try
   {
      static const IID* arr[] = 
      {
         &IID_IInterfaceRoute,
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

STDMETHODIMP InterfaceRoute::get_ID(long *pVal)
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

STDMETHODIMP InterfaceRoute::get_DomainName(BSTR *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->DomainName().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRoute::put_DomainName(BSTR newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->DomainName(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRoute::get_Description(BSTR *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetDescription().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRoute::put_Description(BSTR newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetDescription(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRoute::get_TargetSMTPHost(BSTR *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->TargetSMTPHost().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRoute::put_TargetSMTPHost(BSTR newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->TargetSMTPHost(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRoute::get_TargetSMTPPort(long *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->TargetSMTPPort();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRoute::put_TargetSMTPPort(long newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->TargetSMTPPort(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRoute::get_NumberOfTries(long *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->NumberOfTries();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRoute::put_NumberOfTries(long newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->NumberOfTries(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRoute::get_MinutesBetweenTry(long *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->MinutesBetweenTry();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRoute::put_MinutesBetweenTry(long newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->MinutesBetweenTry(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRoute::Save()
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      HM::String sErrorMessage;
      if (HM::PersistentRoute::SaveObject(object_, sErrorMessage, HM::PersistenceModeNormal))
      {
         // Add to parent collection
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

STDMETHODIMP InterfaceRoute::get_AllAddresses(VARIANT_BOOL *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->ToAllAddresses() ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRoute::put_AllAddresses(VARIANT_BOOL newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->ToAllAddresses(newVal == VARIANT_TRUE ? true : false);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRoute::get_Addresses(IInterfaceRouteAddresses **pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      CComObject<InterfaceRouteAddresses>* pInterfaceAddresses = new CComObject<InterfaceRouteAddresses>;
      pInterfaceAddresses->SetAuthentication(authentication_);
      
      std::shared_ptr<HM::RouteAddresses> pAdresses = object_->GetAddresses();
   
      pInterfaceAddresses->Attach(pAdresses);
      pInterfaceAddresses->AddRef();
   
      *pVal = pInterfaceAddresses;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRoute::get_RelayerRequiresAuth(VARIANT_BOOL *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

   
      *pVal = object_->GetRelayerRequiresAuth() ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRoute::put_RelayerRequiresAuth(VARIANT_BOOL newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

   
      object_->SetRelayerRequiresAuth(newVal == VARIANT_TRUE ? true : false);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRoute::get_TreatSecurityAsLocalDomain(VARIANT_BOOL *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

   
      *pVal = object_->GetTreatRecipientAsLocalDomain() ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRoute::put_TreatSecurityAsLocalDomain(VARIANT_BOOL newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetTreatRecipientAsLocalDomain(newVal == VARIANT_TRUE ? true : false);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRoute::get_TreatSenderAsLocalDomain(VARIANT_BOOL *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

   
      *pVal = object_->GetTreatSenderAsLocalDomain() ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRoute::put_TreatSenderAsLocalDomain(VARIANT_BOOL newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetTreatSenderAsLocalDomain(newVal == VARIANT_TRUE ? true : false);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRoute::get_TreatRecipientAsLocalDomain(VARIANT_BOOL *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

   
      *pVal = object_->GetTreatRecipientAsLocalDomain() ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRoute::put_TreatRecipientAsLocalDomain(VARIANT_BOOL newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetTreatRecipientAsLocalDomain(newVal == VARIANT_TRUE ? true : false);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRoute::get_RelayerAuthUsername(BSTR *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetRelayerAuthUsername().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRoute::put_RelayerAuthUsername(BSTR newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetRelayerAuthUsername(HM::String(newVal));
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRoute::SetRelayerAuthPassword(BSTR newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetRelayerAuthPassword(HM::String(newVal));
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRoute::get_UseSSL(VARIANT_BOOL *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetConnectionSecurity() == HM::CSSSL ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRoute::put_UseSSL(VARIANT_BOOL newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      if (newVal == VARIANT_TRUE)
         object_->SetConnectionSecurity(HM::CSSSL);
      else
         object_->SetConnectionSecurity(HM::CSNone);

      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRoute::put_ConnectionSecurity(eConnectionSecurity newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetConnectionSecurity((HM::ConnectionSecurity) newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRoute::get_ConnectionSecurity(eConnectionSecurity *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = (eConnectionSecurity) object_->GetConnectionSecurity();

      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRoute::Delete()
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      
      if (!authentication_->GetIsServerAdmin())
         return authentication_->GetAccessDenied();
   
      if (!parent_collection_)
         return HM::PersistentRoute::DeleteObject(object_) ? S_OK : S_FALSE;
   
      parent_collection_->DeleteItemByDBID(object_->GetID());
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

