// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceRecipient.h"


void 
InterfaceRecipient::AttachItem(std::shared_ptr<HM::MessageRecipient> pRecipient)
{
   object_ = pRecipient;
}

STDMETHODIMP 
InterfaceRecipient::get_Address(BSTR *pVal)
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
InterfaceRecipient::get_OriginalAddress(BSTR *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetOriginalAddress().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceRecipient::get_IsLocalUser(VARIANT_BOOL *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetLocalAccountID() > 0 ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


