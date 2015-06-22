// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceBackupSettings.h"

#include "../common/Application/IniFileSettings.h"

InterfaceBackupSettings::InterfaceBackupSettings() :
   config_(nullptr),
   ini_file_settings_(nullptr)
{

}

bool 
InterfaceBackupSettings::LoadSettings()
{
   if (!GetIsServerAdmin())
      return false;

   config_ = HM::Configuration::Instance();
   ini_file_settings_ = HM::IniFileSettings::Instance();

   return true;
}

STDMETHODIMP InterfaceBackupSettings::get_Destination(BSTR *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetBackupDestination().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceBackupSettings::put_Destination(BSTR newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->SetBackupDestination(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceBackupSettings::get_LogFile(BSTR *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      HM::String sLogDir = ini_file_settings_->GetLogDirectory();
      if (sLogDir.Right(1) != _T("\\"))
         sLogDir += "\\";
   
      sLogDir += "hmailserver_backup.log";
   
      *pVal = sLogDir.AllocSysString();
      
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP
InterfaceBackupSettings::get_BackupSettings(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetBackupOption(HM::Backup::BOSettings) ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP
InterfaceBackupSettings::put_BackupSettings(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->SetBackupOption(HM::Backup::BOSettings, newVal == VARIANT_TRUE);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP
InterfaceBackupSettings::get_BackupDomains(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetBackupOption(HM::Backup::BODomains) ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP
InterfaceBackupSettings::put_BackupDomains(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->SetBackupOption(HM::Backup::BODomains, newVal == VARIANT_TRUE);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP
InterfaceBackupSettings::get_BackupMessages(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetBackupOption(HM::Backup::BOMessages) ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP
InterfaceBackupSettings::put_BackupMessages(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->SetBackupOption(HM::Backup::BOMessages, newVal == VARIANT_TRUE);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP
InterfaceBackupSettings::get_CompressDestinationFiles(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetBackupOption(HM::Backup::BOCompression) ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP
InterfaceBackupSettings::put_CompressDestinationFiles(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->SetBackupOption(HM::Backup::BOCompression, newVal == VARIANT_TRUE);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


