// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "InterfaceLanguage.h"
#include "../Common/Util/Language.h"
#include "COMError.h"

void 
InterfaceLanguage::Attach(std::shared_ptr<HM::Language> pLanguage)
{
   language_ = pLanguage;
}

STDMETHODIMP InterfaceLanguage::get_Name(BSTR *pVal)
{
   try
   {
      if (!language_)
         return GetAccessDenied();

      HM::String sName = language_->GetName();
      *pVal = sName.AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceLanguage::get_String(BSTR EnglishString, BSTR *pVal)
{
   try
   {
      if (!language_)
         return GetAccessDenied();

      HM::String sText = language_->GetString(EnglishString);
      *pVal = sText.AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceLanguage::get_IsDownloaded(VARIANT_BOOL *pVal)
{
   try
   {
      if (!language_)
         return GetAccessDenied();

      *pVal = language_->GetIsDownloded() ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceLanguage::Download()
{
   try
   {
      if (!language_)
         return GetAccessDenied();

      // language_->Download();   
   
      return COMError::GenerateError("Not implemented.");
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

