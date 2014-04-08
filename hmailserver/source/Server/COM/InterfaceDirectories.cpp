// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceDirectories.h"

STDMETHODIMP InterfaceDirectories::get_ProgramDirectory(BSTR *pVal)
{
   try
   {
      if (!m_pIniFileSettings)
         return GetAccessDenied();

      *pVal = m_pIniFileSettings->GetProgramDirectory().AllocSysString();
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
      if (!m_pIniFileSettings)
         return GetAccessDenied();

      m_pIniFileSettings->SetProgramDirectory(newVal);
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
      if (!m_pIniFileSettings)
         return GetAccessDenied();

      *pVal = m_pIniFileSettings->GetDatabaseDirectory().AllocSysString();
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
      if (!m_pIniFileSettings)
         return GetAccessDenied();

      m_pIniFileSettings->SetDatabaseDirectory(newVal);
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
      if (!m_pIniFileSettings)
         return GetAccessDenied();

      *pVal = m_pIniFileSettings->GetDataDirectory().AllocSysString();
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
      if (!m_pIniFileSettings)
         return GetAccessDenied();

      m_pIniFileSettings->SetDataDirectory(newVal);
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
      if (!m_pIniFileSettings)
         return GetAccessDenied();

      *pVal = m_pIniFileSettings->GetLogDirectory().AllocSysString();
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
      if (!m_pIniFileSettings)
         return GetAccessDenied();

      m_pIniFileSettings->SetLogDirectory(newVal);
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
      if (!m_pIniFileSettings)
         return GetAccessDenied();

      *pVal = m_pIniFileSettings->GetTempDirectory().AllocSysString();
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
      if (!m_pIniFileSettings)
         return GetAccessDenied();

      m_pIniFileSettings->SetTempDirectory(newVal);
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

STDMETHODIMP InterfaceDirectories::put_EventDirectory(BSTR newVal)
{
   try
   {
      if (!m_pIniFileSettings)
         return GetAccessDenied();

      m_pIniFileSettings->SetEventDirectory(newVal);
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
      if (!m_pIniFileSettings)
         return GetAccessDenied();

      *pVal = m_pIniFileSettings->GetDBScriptDirectory().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


