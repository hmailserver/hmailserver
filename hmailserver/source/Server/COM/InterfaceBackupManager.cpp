// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "InterfaceBackupManager.h"
#include "InterfaceBackup.h"

#include "../Common/Application/BackupManager.h"

#include "COMError.h"

bool 
InterfaceBackupManager::LoadSettings()
{
   if (!GetIsServerAdmin())
      return false;

   backup_manager_ = HM::Application::Instance()->GetBackupManager();
   if (!backup_manager_)
      return false;

   return true;

}

STDMETHODIMP 
InterfaceBackupManager::StartBackup()
{
   try
   {
      if (!backup_manager_)
         return GetAccessDenied();

      backup_manager_->StartBackup();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceBackupManager::LoadBackup(BSTR sXMLFile, IInterfaceBackup **pVal)
{
   try
   {
      if (!backup_manager_)
         return GetAccessDenied();

      CComObject<InterfaceBackup>* pBackupInt = new CComObject<InterfaceBackup>();
      pBackupInt->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::Backup> pBackup = backup_manager_->LoadBackup(sXMLFile);
   
      if (!pBackup)
         return DISP_E_BADINDEX;
   
      pBackupInt->Attach(pBackup);
      pBackupInt->AddRef();
      *pVal = pBackupInt;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

