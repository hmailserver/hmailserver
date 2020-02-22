// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceScripting.h"

#include "..\Common\Scripting\ScriptServer.h"

InterfaceScripting::InterfaceScripting() :
   config_(nullptr),
   ini_file_settings_(nullptr)
{ 

}

bool 
InterfaceScripting::LoadSettings()
{
   if (!GetIsServerAdmin())
      return false;

   config_ = HM::Configuration::Instance();
   ini_file_settings_ = HM::IniFileSettings::Instance();

   return true;
}

STDMETHODIMP InterfaceScripting::get_Enabled(VARIANT_BOOL *pVal)
{
   try
   {
      if (!ini_file_settings_)
         return GetAccessDenied();

      *pVal = config_->GetUseScriptServer() ? VARIANT_TRUE : VARIANT_FALSE;
   
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
      if (!ini_file_settings_)
         return GetAccessDenied();

      config_->SetUseScriptServer(newVal == VARIANT_TRUE ? true : false);
   
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
      if (!ini_file_settings_)
         return GetAccessDenied();

      *pVal = config_->GetScriptLanguage().AllocSysString();
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
      if (!ini_file_settings_)
         return GetAccessDenied();

      config_->SetScriptLanguage(newVal);
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
      if (!ini_file_settings_)
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
      if (!ini_file_settings_)
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
      if (!ini_file_settings_)
         return GetAccessDenied();

      *pVal = ini_file_settings_->GetEventDirectory().AllocSysString();
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
      if (!ini_file_settings_)
         return GetAccessDenied();

   
      *pVal = HM::ScriptServer::Instance()->GetCurrentScriptFile().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


