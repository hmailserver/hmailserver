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

   m_pBackupManager = HM::Application::Instance()->GetBackupManager();
   if (!m_pBackupManager)
      return false;

   return true;

}

STDMETHODIMP 
InterfaceBackupManager::StartBackup()
{
   try
   {
      if (!m_pBackupManager)
         return GetAccessDenied();

      m_pBackupManager->StartBackup();
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
      if (!m_pBackupManager)
         return GetAccessDenied();

      CComObject<InterfaceBackup>* pBackupInt = new CComObject<InterfaceBackup>();
      pBackupInt->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::Backup> pBackup = m_pBackupManager->LoadBackup(sXMLFile);
   
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

