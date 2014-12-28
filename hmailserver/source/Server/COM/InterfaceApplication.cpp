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
   authentication_ = std::shared_ptr<HM::COMAuthentication>(new HM::COMAuthentication);

   authentication_->AttempAnonymousAuthentication();
}

STDMETHODIMP InterfaceApplication::Start()
{
   try
   {
      // Start the server threads.
      if (!authentication_->GetIsServerAdmin())
         return authentication_->GetAccessDenied();
   
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
      if (!authentication_->GetIsServerAdmin())
         return authentication_->GetAccessDenied();
       
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
      if (!authentication_->GetIsServerAdmin())
         return authentication_->GetAccessDenied();
   
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
      if (!authentication_->GetIsServerAdmin())
         return authentication_->GetAccessDenied();
   
      HRESULT hResult = EnsureDatabaseConnectivity_();
      if (hResult != S_OK)
         return hResult;
   
      CComObject<InterfaceSettings>* pInterfaceSettings = new CComObject<InterfaceSettings>;
      pInterfaceSettings->SetAuthentication(authentication_);
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
      if (!authentication_->GetIsServerAdmin())
         return authentication_->GetAccessDenied();
   
      HRESULT hResult = EnsureDatabaseConnectivity_();
      if (hResult != S_OK)
         return hResult;
   
      CComObject<InterfaceBackupManager>* pInterfaceBackupManager = new CComObject<InterfaceBackupManager>;
      pInterfaceBackupManager->SetAuthentication(authentication_);
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
      
      pInterfaceGlobalObjects->SetAuthentication(authentication_);
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
      if (!authentication_->GetIsAuthenticated())
         return authentication_->GetAccessDenied();
   
      HRESULT hResult = EnsureDatabaseConnectivity_();
      if (hResult != S_OK)
         return hResult;
   
   
      CComObject<InterfaceDomains>* pInterfaceDomains = new CComObject<InterfaceDomains>;
   
      pInterfaceDomains->SetAuthentication(authentication_);
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
      if (!authentication_->GetIsServerAdmin())
         return authentication_->GetAccessDenied();
   
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
   
      pInterfaceDatabase->SetAuthentication(authentication_);
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
      if (!authentication_->GetIsServerAdmin())
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceStatus>* pStatus = new CComObject<InterfaceStatus>;
   
      pStatus->SetAuthentication(authentication_);
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
      std::shared_ptr<const HM::Account> pAccount = authentication_->Authenticate(sUsername, sPassword);
   
      if (pAccount)
      {
         std::shared_ptr<HM::Account> accountCopy = std::shared_ptr<HM::Account>(new HM::Account(*pAccount.get()));
   
         // Return the account.
         CComObject<InterfaceAccount>* pAccountInt = new CComObject<InterfaceAccount>();
   
         pAccountInt->AttachItem(accountCopy);
         pAccountInt->SetAuthentication(authentication_);
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

STDMETHODIMP InterfaceApplication::get_VersionArchitecture(BSTR *pVal)
{
   try
   {
      *pVal = HM::Application::Instance()->GetVersionArchitecture().AllocSysString();
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
   
      pUtilities->SetAuthentication(authentication_);
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
      if (!authentication_->GetIsServerAdmin())
         return authentication_->GetAccessDenied();
   
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
      if (!authentication_->GetIsServerAdmin())
         return authentication_->GetAccessDenied();
   
      HRESULT hResult = EnsureDatabaseConnectivity_();
      if (hResult != S_OK)
         return hResult;
   
      CComObject<InterfaceRules >* pItem = new CComObject<InterfaceRules >();
      pItem->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::Rules> pRules = std::shared_ptr<HM::Rules>(new HM::Rules(0));
   
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
InterfaceApplication::EnsureDatabaseConnectivity_()
{
   std::shared_ptr<HM::DatabaseConnectionManager> pConnectionManager = HM::Application::Instance()->GetDBManager();
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
      if (!authentication_->GetIsServerAdmin())
         return authentication_->GetAccessDenied();
   
      HRESULT hResult = EnsureDatabaseConnectivity_();
      if (hResult != S_OK)
         return hResult;
   
      CComObject<InterfaceLinks>* pItem = new CComObject<InterfaceLinks >();
      pItem->SetAuthentication(authentication_);
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
      if (!authentication_->GetIsServerAdmin())
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceDiagnostics>* pInterfaceDiagnostics = new CComObject<InterfaceDiagnostics>;
   
      pInterfaceDiagnostics->SetAuthentication(authentication_);
      pInterfaceDiagnostics->AddRef();
   
      *pVal = pInterfaceDiagnostics;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

