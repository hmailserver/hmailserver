// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "InterfaceApplication.h"
#include "InterfaceAccount.h"
#include "InterfaceDomains.h"
#include "InterfaceDatabase.h"
#include "InterfaceDiagnostics.h"
#include "InterfaceStatus.h"
#include "InterfaceUtilities.h"
#include "InterfaceRules.h"
#include "InterfaceSettings.h"
#include "InterfaceBackupManager.h"
#include "InterfaceGlobalObjects.h"
#include "InterfaceLinks.h"

#include "COMError.h"

#include "../common/Application/IniFileSettings.h"

#include "../common/util/ServiceManager.h"
#include "../common/util/ServerStatus.h"

#include "../common/BO/Rules.h"
#include "../common/BO/Domains.h"

#include "COMAuthentication.h"

STDMETHODIMP InterfaceApplication::InterfaceSupportsErrorInfo(REFIID riid)
{
   static const IID* arr[] = 
   {
      &IID_IInterfaceApplication,
   };

   for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
   {
      if (InlineIsEqualGUID(*arr[i],riid))
         return S_OK;
   }

   return S_FALSE;
}

InterfaceApplication::InterfaceApplication()
{
   m_pAuthentication = shared_ptr<HM::COMAuthentication>(new HM::COMAuthentication);

   m_pAuthentication->AttempAnonymousAuthentication();
}

STDMETHODIMP InterfaceApplication::Start()
{
   try
   {
      // Start the server threads.
      if (!m_pAuthentication->GetIsServerAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      if (!HM::Application::Instance()->StartServers())
         return COMError::GenerateError("Server start failed. Please check the hMailServer error log.");
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceApplication::Stop()
{
   try
   {
      // Stop the server threads.
      if (!m_pAuthentication->GetIsServerAdmin())
         return m_pAuthentication->GetAccessDenied();
       
      HM::Application::Instance()->StopServers();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceApplication::Reinitialize()
{
   try
   {
      if (!m_pAuthentication->GetIsServerAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      HM::String sErrorMessage = HM::Application::Instance()->Reinitialize();
      if (!sErrorMessage.IsEmpty())
         return COMError::GenerateError(sErrorMessage);
      
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceApplication::get_Settings(IInterfaceSettings **pVal)
{
   try
   {
      if (!m_pAuthentication->GetIsServerAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      HRESULT hResult = _EnsureDatabaseConnectivity();
      if (hResult != S_OK)
         return hResult;
   
      CComObject<InterfaceSettings>* pInterfaceSettings = new CComObject<InterfaceSettings>;
      pInterfaceSettings->SetAuthentication(m_pAuthentication);
      pInterfaceSettings->LoadSettings();
   
      pInterfaceSettings->AddRef();
      *pVal = pInterfaceSettings;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceApplication::get_BackupManager(IInterfaceBackupManager **pVal)
{
   try
   {
      if (!m_pAuthentication->GetIsServerAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      HRESULT hResult = _EnsureDatabaseConnectivity();
      if (hResult != S_OK)
         return hResult;
   
      CComObject<InterfaceBackupManager>* pInterfaceBackupManager = new CComObject<InterfaceBackupManager>;
      pInterfaceBackupManager->SetAuthentication(m_pAuthentication);
      if (!pInterfaceBackupManager->LoadSettings())
         return COMError::GenerateError("Backup manager not available");
   	  
      pInterfaceBackupManager->AddRef();
      *pVal = pInterfaceBackupManager;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceApplication::get_GlobalObjects(IInterfaceGlobalObjects **pVal)
{
   try
   {
      CComObject<InterfaceGlobalObjects>* pInterfaceGlobalObjects = new CComObject<InterfaceGlobalObjects>;
      
      pInterfaceGlobalObjects->SetAuthentication(m_pAuthentication);
      pInterfaceGlobalObjects->AddRef();
      *pVal = pInterfaceGlobalObjects;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceApplication::get_Domains(IInterfaceDomains **pVal)
{
   try
   {
      if (!m_pAuthentication->GetIsAuthenticated())
         return m_pAuthentication->GetAccessDenied();
   
      HRESULT hResult = _EnsureDatabaseConnectivity();
      if (hResult != S_OK)
         return hResult;
   
   
      CComObject<InterfaceDomains>* pInterfaceDomains = new CComObject<InterfaceDomains>;
   
      pInterfaceDomains->SetAuthentication(m_pAuthentication);
      pInterfaceDomains->Refresh();
   
      pInterfaceDomains->AddRef();
      *pVal = pInterfaceDomains;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceApplication::get_ServerState(eServerState *pVal)
{
   try
   {
      if (!m_pAuthentication->GetIsServerAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      HM::ServerStatus::ServerState iState = (HM::ServerStatus::ServerState) HM::ServerStatus::Instance()->GetState();
   
      switch (iState)
      {
      case HM::ServerStatus::StateStarting:
         *pVal = hStateStarting;
         break;
      case HM::ServerStatus::StateRunning:
         *pVal = hStateRunning;
         break;
      case HM::ServerStatus::StateStopping:
         *pVal = hStateStopping;
         break;
      case HM::ServerStatus::StateStopped:
         *pVal = hStateStopped;
         break;
      default:
         *pVal = hStateUnknown;
         break;
      }
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceApplication::get_Database(IInterfaceDatabase **pVal)
{
   try
   {
      CComObject<InterfaceDatabase>* pInterfaceDatabase = new CComObject<InterfaceDatabase>;
   
      pInterfaceDatabase->SetAuthentication(m_pAuthentication);
      pInterfaceDatabase->LoadSettings();
   
      pInterfaceDatabase->AddRef();
      *pVal = pInterfaceDatabase;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceApplication::get_Status(IInterfaceStatus **pVal)
{
   try
   {
      if (!m_pAuthentication->GetIsServerAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      CComObject<InterfaceStatus>* pStatus = new CComObject<InterfaceStatus>;
   
      pStatus->SetAuthentication(m_pAuthentication);
      pStatus->LoadSettings();
   
      pStatus->AddRef();
      *pVal = pStatus;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceApplication::SubmitEMail()
{
   try
   {
      DWORD SubmitEmailOpCode = 200;
   
      HM::ServiceManager pServiceManager;
      pServiceManager.UserControlService("hMailServer", SubmitEmailOpCode);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceApplication::Connect()
{
   try
   {
      HM::String sErrorMessage = HM::Application::Instance()->GetLastErrorMessage();
      if (!sErrorMessage.IsEmpty())
         return COMError::GenerateError(sErrorMessage);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceApplication::Authenticate(BSTR sUsername, BSTR sPassword, IInterfaceAccount **pVal)
{
   try
   {
      // Authenticates the user and returns the account object.
      shared_ptr<const HM::Account> pAccount = m_pAuthentication->Authenticate(sUsername, sPassword);
   
      if (pAccount)
      {
         shared_ptr<HM::Account> accountCopy = shared_ptr<HM::Account>(new HM::Account(*pAccount.get()));
   
         // Return the account.
         CComObject<InterfaceAccount>* pAccountInt = new CComObject<InterfaceAccount>();
   
         pAccountInt->AttachItem(accountCopy);
         pAccountInt->SetAuthentication(m_pAuthentication);
         pAccountInt->AddRef();
   
         *pVal = pAccountInt;
   
      }
      
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceApplication::get_Version(BSTR *pVal)
{
   try
   {
      *pVal = HM::Application::Instance()->GetVersionNumber().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceApplication::get_Utilities(IInterfaceUtilities **pVal)
{
   try
   {
      CComObject<InterfaceUtilities>* pUtilities = new CComObject<InterfaceUtilities>;
   
      pUtilities->SetAuthentication(m_pAuthentication);
      pUtilities->AddRef();
      *pVal = pUtilities;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceApplication::get_InitializationFile(BSTR *pVal)
{
   try
   {
      if (!m_pAuthentication->GetIsServerAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      *pVal = HM::IniFileSettings::Instance()->GetInitializationFile().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceApplication::get_Rules(IInterfaceRules **pVal)
{
   try
   {
      if (!m_pAuthentication->GetIsServerAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      HRESULT hResult = _EnsureDatabaseConnectivity();
      if (hResult != S_OK)
         return hResult;
   
      CComObject<InterfaceRules >* pItem = new CComObject<InterfaceRules >();
      pItem->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::Rules> pRules = shared_ptr<HM::Rules>(new HM::Rules(0));
   
      if (pRules)
      {
         pRules->Refresh();
         pItem->Attach(pRules);
         pItem->AddRef();
         *pVal = pItem;
      }
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

HRESULT
InterfaceApplication::_EnsureDatabaseConnectivity()
{
   shared_ptr<HM::DatabaseConnectionManager> pConnectionManager = HM::Application::Instance()->GetDBManager();
   if (!pConnectionManager || !pConnectionManager->GetIsConnected())
   {
      return COMError::GenerateError("The connection to the database is not available. Please check the hMailServer error log for details.");
   }

   return S_OK;
}

STDMETHODIMP InterfaceApplication::get_Links(IInterfaceLinks **pVal)
{
   try
   {
      if (!m_pAuthentication->GetIsServerAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      HRESULT hResult = _EnsureDatabaseConnectivity();
      if (hResult != S_OK)
         return hResult;
   
      CComObject<InterfaceLinks>* pItem = new CComObject<InterfaceLinks >();
      pItem->SetAuthentication(m_pAuthentication);
      pItem->AddRef();
      *pVal = pItem;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceApplication::get_Diagnostics(IInterfaceDiagnostics **pVal)
{
   try
   {
      if (!m_pAuthentication->GetIsServerAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      CComObject<InterfaceDiagnostics>* pInterfaceDiagnostics = new CComObject<InterfaceDiagnostics>;
   
      pInterfaceDiagnostics->SetAuthentication(m_pAuthentication);
      pInterfaceDiagnostics->AddRef();
   
      *pVal = pInterfaceDiagnostics;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

