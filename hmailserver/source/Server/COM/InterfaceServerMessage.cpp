// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "..\COM\InterfaceServerMessage.h"

#include "..\Common\Persistence\PersistentServerMessage.h"
#include "..\Common\BO\ServerMessage.h"

#include "COMError.h"

STDMETHODIMP 
InterfaceServerMessage::Save()
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      if (HM::PersistentServerMessage::SaveObject(object_))
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

STDMETHODIMP InterfaceServerMessage::get_ID(long *pVal)
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

STDMETHODIMP InterfaceServerMessage::put_Name(BSTR newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetName(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceServerMessage::get_Name(BSTR *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetName().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceServerMessage::put_Text(BSTR newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetText(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceServerMessage::get_Text(BSTR *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetText().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

