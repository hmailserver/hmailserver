// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceScripting.h"

#include "..\Common\Application\ScriptingHost\ScriptServer.h"

bool 
InterfaceScripting::LoadSettings()
{
   if (!GetIsServerAdmin())
      return false;

   m_pConfig = HM::Configuration::Instance();
   m_pIniFileSettings = HM::IniFileSettings::Instance();

   return true;
}

STDMETHODIMP InterfaceScripting::get_Enabled(VARIANT_BOOL *pVal)
{
   try
   {
      if (!m_pIniFileSettings)
         return GetAccessDenied();

      *pVal = m_pConfig->GetUseScriptServer() ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceScripting::put_Enabled(VARIANT_BOOL newVal)
{
   try
   {
      if (!m_pIniFileSettings)
         return GetAccessDenied();

      m_pConfig->SetUseScriptServer(newVal == VARIANT_TRUE ? true : false);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceScripting::get_Language(BSTR *pVal)
{
   try
   {
      if (!m_pIniFileSettings)
         return GetAccessDenied();

      *pVal = m_pConfig->GetScriptLanguage().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceScripting::put_Language(BSTR newVal)
{
   try
   {
      if (!m_pIniFileSettings)
         return GetAccessDenied();

      m_pConfig->SetScriptLanguage(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceScripting::Reload(void)
{
   try
   {
      if (!m_pIniFileSettings)
         return GetAccessDenied();

      HM::ScriptServer::Instance()->LoadScripts();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceScripting::CheckSyntax(BSTR *pVal)
{
   try
   {
      if (!m_pIniFileSettings)
         return GetAccessDenied();

      *pVal = HM::ScriptServer::Instance()->CheckSyntax().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceScripting::get_Directory(BSTR *pVal)
{
   try
   {
      if (!m_pIniFileSettings)
         return GetAccessDenied();

      *pVal = m_pIniFileSettings->GetEventDirectory().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceScripting::get_CurrentScriptFile(BSTR *pVal)
{
   try
   {
      if (!m_pIniFileSettings)
         return GetAccessDenied();

   
      *pVal = HM::ScriptServer::Instance()->GetCurrentScriptFile().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


