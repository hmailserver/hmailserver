// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceBackup.h"

#include "../Common/Application/BackupManager.h"

void 
InterfaceBackup::Attach(std::shared_ptr<HM::Backup> pBackup)
{
   backup_ = pBackup;
}

STDMETHODIMP 
InterfaceBackup::StartRestore()
{
   try
   {
      if (!backup_)
         return GetAccessDenied();

      // Start a restore of this backup.
      HM::Application::Instance()->GetBackupManager()->StartRestore(backup_);
      
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceBackup::get_ContainsSettings(VARIANT_BOOL* pVal)
{
   try
   {
      if (!backup_)
         return GetAccessDenied();

      *pVal = backup_->GetContains(HM::Backup::BOSettings) ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceBackup::get_ContainsDomains(VARIANT_BOOL* pVal)
{
   try
   {
      if (!backup_)
         return GetAccessDenied();

      *pVal = backup_->GetContains(HM::Backup::BODomains) ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceBackup::get_ContainsMessages(VARIANT_BOOL* pVal)
{
   try
   {
      if (!backup_)
         return GetAccessDenied();

      *pVal = backup_->GetContains(HM::Backup::BOMessages) ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceBackup::get_RestoreSettings(VARIANT_BOOL* pVal)
{
   try
   {
      if (!backup_)
         return GetAccessDenied();

      *pVal = backup_->GetRestoreOption(HM::Backup::BOSettings) ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceBackup::put_RestoreSettings(VARIANT_BOOL newVal)
{
   try
   {
      if (!backup_)
         return GetAccessDenied();

      backup_->SetRestoreOption(HM::Backup::BOSettings, newVal == VARIANT_TRUE);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceBackup::get_RestoreDomains(VARIANT_BOOL* pVal)
{
   try
   {
      if (!backup_)
         return GetAccessDenied();

      *pVal = backup_->GetRestoreOption(HM::Backup::BODomains) ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceBackup::put_RestoreDomains(VARIANT_BOOL newVal)
{
   try
   {
      if (!backup_)
         return GetAccessDenied();

      backup_->SetRestoreOption(HM::Backup::BODomains, newVal == VARIANT_TRUE);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceBackup::get_RestoreMessages(VARIANT_BOOL* pVal)
{
   try
   {
      if (!backup_)
         return GetAccessDenied();

      *pVal = backup_->GetRestoreOption(HM::Backup::BOMessages) ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceBackup::put_RestoreMessages(VARIANT_BOOL newVal)
{
   try
   {
      if (!backup_)
         return GetAccessDenied();

      backup_->SetRestoreOption(HM::Backup::BOMessages, newVal == VARIANT_TRUE);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


