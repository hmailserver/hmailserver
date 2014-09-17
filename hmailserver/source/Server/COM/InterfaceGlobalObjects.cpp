// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceGlobalObjects.h"
#include "InterfaceDeliveryQueue.h"
#include "InterfaceLanguages.h"

#include "COMAuthentication.h"

STDMETHODIMP 
InterfaceGlobalObjects::InterfaceSupportsErrorInfo(REFIID riid)
{
   static const IID* arr[] = 
   {
      &IID_IInterfaceGlobalObjects,
   };

   for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
   {
      if (InlineIsEqualGUID(*arr[i],riid))
         return S_OK;
   }
   return S_FALSE;
}

void
InterfaceGlobalObjects::SetAuthentication(std::shared_ptr<HM::COMAuthentication> pAuthentication)
{
   authentication_ = pAuthentication;
}

STDMETHODIMP InterfaceGlobalObjects::get_DeliveryQueue(IInterfaceDeliveryQueue **pVal)
{
   try
   {
      if (!authentication_)
         return GetAccessDenied();

      if (!authentication_->GetIsServerAdmin())
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceDeliveryQueue>* pInterfaceDeliveryQueue = new CComObject<InterfaceDeliveryQueue>;
      pInterfaceDeliveryQueue->SetAuthentication(authentication_);
   
      pInterfaceDeliveryQueue->AddRef();
      *pVal = pInterfaceDeliveryQueue;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceGlobalObjects::get_Languages(IInterfaceLanguages **pVal)
{
   try
   {
      if (!authentication_)
         return GetAccessDenied();

      CComObject<InterfaceLanguages>* pLanguageInt = new CComObject<InterfaceLanguages>();
   
      pLanguageInt->AddRef();
      *pVal = pLanguageInt;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


