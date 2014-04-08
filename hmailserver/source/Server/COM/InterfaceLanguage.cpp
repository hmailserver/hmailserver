// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "InterfaceLanguage.h"
#include "../Common/Util/Language.h"
#include "COMError.h"

void 
InterfaceLanguage::Attach(shared_ptr<HM::Language> pLanguage)
{
   m_pLanguage = pLanguage;
}

STDMETHODIMP InterfaceLanguage::get_Name(BSTR *pVal)
{
   try
   {
      if (!m_pLanguage)
         return GetAccessDenied();

      HM::String sName = m_pLanguage->GetName();
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
      if (!m_pLanguage)
         return GetAccessDenied();

      HM::String sText = m_pLanguage->GetString(EnglishString);
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
      if (!m_pLanguage)
         return GetAccessDenied();

      *pVal = m_pLanguage->GetIsDownloded() ? VARIANT_TRUE : VARIANT_FALSE;
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
      if (!m_pLanguage)
         return GetAccessDenied();

      // m_pLanguage->Download();   
   
      return COMError::GenerateError("Not implemented.");
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

