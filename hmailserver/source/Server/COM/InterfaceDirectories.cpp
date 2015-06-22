// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceDirectories.h"

InterfaceDirectories::InterfaceDirectories() :
   ini_file_settings_(nullptr)
{

}

STDMETHODIMP InterfaceDirectories::get_ProgramDirectory(BSTR *pVal)
{
   try
   {
      if (!ini_file_settings_)
         return GetAccessDenied();

      *pVal = ini_file_settings_->GetProgramDirectory().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDirectories::put_ProgramDirectory(BSTR newVal)
{
   try
   {
      if (!ini_file_settings_)
         return GetAccessDenied();

      ini_file_settings_->SetProgramDirectory(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDirectories::get_DatabaseDirectory(BSTR *pVal)
{
   try
   {
      if (!ini_file_settings_)
         return GetAccessDenied();

      *pVal = ini_file_settings_->GetDatabaseDirectory().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDirectories::put_DatabaseDirectory(BSTR newVal)
{
   try
   {
      if (!ini_file_settings_)
         return GetAccessDenied();

      ini_file_settings_->SetDatabaseDirectory(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDirectories::get_DataDirectory(BSTR *pVal)
{
   try
   {
      if (!ini_file_settings_)
         return GetAccessDenied();

      *pVal = ini_file_settings_->GetDataDirectory().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDirectories::put_DataDirectory(BSTR newVal)
{
   try
   {
      if (!ini_file_settings_)
         return GetAccessDenied();

      ini_file_settings_->SetDataDirectory(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDirectories::get_LogDirectory(BSTR *pVal)
{
   try
   {
      if (!ini_file_settings_)
         return GetAccessDenied();

      *pVal = ini_file_settings_->GetLogDirectory().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDirectories::put_LogDirectory(BSTR newVal)
{
   try
   {
      if (!ini_file_settings_)
         return GetAccessDenied();

      ini_file_settings_->SetLogDirectory(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDirectories::get_TempDirectory(BSTR *pVal)
{
   try
   {
      if (!ini_file_settings_)
         return GetAccessDenied();

      *pVal = ini_file_settings_->GetTempDirectory().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDirectories::put_TempDirectory(BSTR newVal)
{
   try
   {
      if (!ini_file_settings_)
         return GetAccessDenied();

      ini_file_settings_->SetTempDirectory(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDirectories::get_EventDirectory(BSTR *pVal)
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

STDMETHODIMP InterfaceDirectories::put_EventDirectory(BSTR newVal)
{
   try
   {
      if (!ini_file_settings_)
         return GetAccessDenied();

      ini_file_settings_->SetEventDirectory(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDirectories::get_DBScriptDirectory(BSTR *pVal)
{
   try
   {
      if (!ini_file_settings_)
         return GetAccessDenied();

      *pVal = ini_file_settings_->GetDBScriptDirectory().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


