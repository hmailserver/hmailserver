// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "COMError.h"

#include "InterfaceSettings.h"
#include "InterfaceLogging.h"
#include "InterfaceAntiVirus.h"
#include "InterfaceCache.h"

#include "../Common/Application/ACLManager.h"
#include "../Common/BO/TCPIPPorts.h"
#include "../Common/BO/SSLCertificates.h"
#include "../Common/BO/IncomingRelays.h"
#include "../Common/AntiSpam/AntiSpamConfiguration.h"

#include "../Common/Persistence/PersistentServerMessage.h"

#include "../POP3/POP3Configuration.h"
#include "../SMTP/SMTPConfiguration.h"
#include "../IMAP/IMAPConfiguration.h"


#include "InterfaceSecurityRanges.h"
#include "InterfaceRoutes.h"
#include "InterfaceIMAPFolders.h"
#include "InterfaceScripting.h"
#include "InterfaceBackupSettings.h"
#include "InterfaceAntiSpam.h"
#include "InterfaceServerMessages.h"
#include "InterfaceTCPIPPorts.h"
#include "InterfaceSSLCertificates.h"
#include "InterfaceDirectories.h"
#include "InterfaceGroups.h"
#include "InterfaceIncomingRelays.h"
#include "InterfaceMessageIndexing.h"

#include "../Common/BO/Groups.h"

InterfaceSettings::InterfaceSettings() :
   config_(nullptr),
   ini_file_settings_(nullptr)
{

}

STDMETHODIMP InterfaceSettings::InterfaceSupportsErrorInfo(REFIID riid)
{
   static const IID* arr[] = 
   {
      &IID_IInterfaceSettings,
   };

   for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
   {
      if (InlineIsEqualGUID(*arr[i],riid))
         return S_OK;
   }
   return S_FALSE;
}
   
bool 
InterfaceSettings::LoadSettings()
{
   if (!GetIsServerAdmin())
      return false;

   config_ = HM::Configuration::Instance();
   ini_file_settings_ = HM::IniFileSettings::Instance();

   return true;
}

STDMETHODIMP InterfaceSettings::get_MaxMessageSize(long *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetSMTPConfiguration()->GetMaxMessageSize();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_MaxMessageSize(long newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->GetSMTPConfiguration()->SetMaxMessageSize(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_MaxSMTPConnections(long *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetSMTPConfiguration()->GetMaxSMTPConnections();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_MaxSMTPConnections(long newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->GetSMTPConfiguration()->SetMaxSMTPConnections(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_MaxIMAPConnections(long *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetIMAPConfiguration()->GetMaxIMAPConnections();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_MaxIMAPConnections(long newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->GetIMAPConfiguration()->SetMaxIMAPConnections(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_MaxPOP3Connections(long *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

   
      *pVal = config_->GetPOP3Configuration()->GetMaxPOP3Connections();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_MaxPOP3Connections(long newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->GetPOP3Configuration()->SetMaxPOP3Connections(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_MirrorEMailAddress(BSTR *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetMirrorAddress().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_MirrorEMailAddress(BSTR newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

   
      config_->SetMirrorAddress(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_AllowSMTPAuthPlain(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

   
      if (config_->GetSMTPConfiguration()->GetAuthAllowPlainText())
         *pVal = VARIANT_TRUE;
      else
         *pVal = VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_AllowSMTPAuthPlain(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

   
      if (newVal == VARIANT_TRUE)
         config_->GetSMTPConfiguration()->SetAuthAllowPlainText(true);
      else
         config_->GetSMTPConfiguration()->SetAuthAllowPlainText(false);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_DenyMailFromNull(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      if (config_->GetSMTPConfiguration()->GetAllowMailFromNull())
         *pVal = VARIANT_FALSE;
      else
         *pVal = VARIANT_TRUE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_DenyMailFromNull(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

   
   
      if (newVal == VARIANT_TRUE)
         config_->GetSMTPConfiguration()->SetAllowMailFromNull (false);
      else
         config_->GetSMTPConfiguration()->SetAllowMailFromNull (true);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_AllowIncorrectLineEndings(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetSMTPConfiguration()->GetAllowIncorrectLineEndings() ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_AllowIncorrectLineEndings(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->GetSMTPConfiguration()->SetAllowIncorrectLineEndings(newVal == VARIANT_TRUE);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_Logging(IInterfaceLogging **pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceLogging>* pInterfaceLogging = new CComObject<InterfaceLogging>;
      
      pInterfaceLogging->SetAuthentication(authentication_);
      pInterfaceLogging->LoadSettings();
   
      pInterfaceLogging->AddRef();
   
      *pVal = pInterfaceLogging;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_AntiVirus(IInterfaceAntiVirus **pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceAntiVirus>* pInterfaceAntiVirus = new CComObject<InterfaceAntiVirus>;
   
      pInterfaceAntiVirus->SetAuthentication(authentication_);
   
      pInterfaceAntiVirus->AddRef();
   
      *pVal = pInterfaceAntiVirus;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_Cache(IInterfaceCache **pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceCache>* pInterfaceCache = new CComObject<InterfaceCache>;
      
      pInterfaceCache->SetAuthentication(authentication_);
      pInterfaceCache->LoadSettings();
   
      pInterfaceCache->AddRef();
   
      *pVal = pInterfaceCache;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_SecurityRanges(IInterfaceSecurityRanges **pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceSecurityRanges>* pRangesInt = new CComObject<InterfaceSecurityRanges>();
      pRangesInt->SetAuthentication(authentication_);
      pRangesInt->LoadSettings();
   
      pRangesInt->AddRef();
      *pVal = pRangesInt;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_SMTPNoOfTries(long *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetSMTPConfiguration()->GetNoOfRetries();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_SMTPNoOfTries(long newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

   
      config_->GetSMTPConfiguration()->SetNoOfRetries(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_SMTPMinutesBetweenTry(long *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

   
   
      *pVal = config_->GetSMTPConfiguration()->GetMinutesBetweenTry();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_SMTPMinutesBetweenTry(long newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

   
      config_->GetSMTPConfiguration()->SetMinutesBetweenTry(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_MaxDeliveryThreads(long *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

   
   
      *pVal = config_->GetSMTPConfiguration()->GetMaxNoOfDeliveryThreads();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_MaxDeliveryThreads(long newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

   
      config_->GetSMTPConfiguration()->SetMaxNoOfDeliveryThreads(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_SMTPRelayer(BSTR *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

   
      *pVal = config_->GetSMTPConfiguration()->GetSMTPRelayer().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_SMTPRelayer(BSTR newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

   
      config_->GetSMTPConfiguration()->SetSMTPRelayer(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_SMTPRelayerPort(long *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

   
      *pVal = config_->GetSMTPConfiguration()->GetSMTPRelayerPort();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_SMTPRelayerPort(long newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

   
      config_->GetSMTPConfiguration()->SetSMTPRelayerPort(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_HostName(BSTR *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

   
      *pVal = config_->GetHostName().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_HostName(BSTR newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      HM::String sNewVal = newVal;
      config_->SetHostName(sNewVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_WelcomeSMTP(BSTR *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetSMTPConfiguration()->GetWelcomeMessage().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_WelcomeSMTP(BSTR newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->GetSMTPConfiguration()->SetWelcomeMessage(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_WelcomePOP3(BSTR *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetPOP3Configuration()->GetWelcomeMessage().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_WelcomePOP3(BSTR newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->GetPOP3Configuration()->SetWelcomeMessage(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_WelcomeIMAP(BSTR *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetIMAPConfiguration()->GetWelcomeMessage().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_WelcomeIMAP(BSTR newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->GetIMAPConfiguration()->SetWelcomeMessage(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_ServiceSMTP(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      if (config_->GetUseSMTP())
         *pVal = VARIANT_TRUE;
      else
         *pVal = VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_ServiceSMTP(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      if (newVal == VARIANT_TRUE)
         config_->SetUseSMTP(true);
      else
         config_->SetUseSMTP(false);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_ServicePOP3(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      if (config_->GetUsePOP3())
         *pVal = VARIANT_TRUE;
      else
         *pVal = VARIANT_FALSE;
   
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_ServicePOP3(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      if (newVal == VARIANT_TRUE)
         config_->SetUsePOP3(true);
      else
         config_->SetUsePOP3(false);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_ServiceIMAP(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetUseIMAP() ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_ServiceIMAP(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->SetUseIMAP(newVal == VARIANT_TRUE ? true : false);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_SMTPRelayerRequiresAuthentication(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetSMTPConfiguration()->GetSMTPRelayerRequiresAuthentication() ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_SMTPRelayerRequiresAuthentication(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->GetSMTPConfiguration()->SetSMTPRelayerRequiresAuthentication(newVal == VARIANT_TRUE);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_SMTPRelayerUsername(BSTR *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

   
      *pVal = config_->GetSMTPConfiguration()->GetSMTPRelayerUsername().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_SMTPRelayerUsername(BSTR newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->GetSMTPConfiguration()->SetSMTPRelayerUsername(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_UserInterfaceLanguage(BSTR *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

   
      *pVal = ini_file_settings_->GetUserInterfaceLanguage().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_UserInterfaceLanguage(BSTR newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      ini_file_settings_->SetUserInterfaceLanguage(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::SetSMTPRelayerPassword(BSTR newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->GetSMTPConfiguration()->SetSMTPRelayerPassword(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::SetAdministratorPassword(BSTR newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return false;
      
   
      ini_file_settings_->SetAdministratorPassword(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_Routes(IInterfaceRoutes **pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceRoutes>* pRoutesInterface = new CComObject<InterfaceRoutes>();
      pRoutesInterface->SetAuthentication(authentication_);
      pRoutesInterface->LoadSettings();
   
      pRoutesInterface->AddRef();
      *pVal = pRoutesInterface;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_Scripting(IInterfaceScripting **pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceScripting>* pItem = new CComObject<InterfaceScripting >();
      pItem->SetAuthentication(authentication_);
      pItem->LoadSettings();
   
      pItem->AddRef();
      *pVal = pItem;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_Backup(IInterfaceBackupSettings **pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceBackupSettings>* pItem = new CComObject<InterfaceBackupSettings>();
     
      pItem->SetAuthentication(authentication_);
      pItem->LoadSettings();
   
      pItem->AddRef();
      *pVal = pItem;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_Directories(IInterfaceDirectories **pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceDirectories>* pItem = new CComObject<InterfaceDirectories>();
   
      ini_file_settings_->LoadSettings();
   
      pItem->SetAuthentication(authentication_);
      pItem->LoadSettings(ini_file_settings_);
      pItem->AddRef();
      *pVal = pItem;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_AntiSpam(IInterfaceAntiSpam **pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      CComObject<InterfaceAntiSpam>* pItem = new CComObject<InterfaceAntiSpam>();
      
      pItem->SetAuthentication(authentication_);
      pItem->LoadSettings();
   
      pItem->AddRef();
      *pVal = pItem;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_ServerMessages(IInterfaceServerMessages **pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceServerMessages>* pItem = new CComObject<InterfaceServerMessages>();
      
      pItem->SetAuthentication(authentication_);
      pItem->LoadSettings();
   
      pItem->AddRef();
      *pVal = pItem;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_TCPIPPorts(IInterfaceTCPIPPorts **pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceTCPIPPorts>* pItem = new CComObject<InterfaceTCPIPPorts>();
      pItem->SetAuthentication(authentication_);
      pItem->Attach(HM::Configuration::Instance()->GetTCPIPPorts());
   
      pItem->AddRef();
      *pVal = pItem;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_SSLCertificates(IInterfaceSSLCertificates **pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return false;
   
      std::shared_ptr<HM::SSLCertificates> pSSLCertificates = HM::Configuration::Instance()->GetSSLCertificates();
   
      CComObject<InterfaceSSLCertificates>* pItem = new CComObject<InterfaceSSLCertificates>();
      pItem->SetAuthentication(authentication_);
      pItem->Attach(pSSLCertificates);
   
      pItem->AddRef();
      *pVal = pItem;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_RuleLoopLimit(long *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetSMTPConfiguration()->GetRuleLoopLimit();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_RuleLoopLimit(long newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->GetSMTPConfiguration()->SetRuleLoopLimit(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_DefaultDomain(BSTR *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetDefaultDomain().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_DefaultDomain(BSTR newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->SetDefaultDomain(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_IMAPPublicFolderName(BSTR *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetIMAPConfiguration()->GetIMAPPublicFolderName().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_IMAPPublicFolderName(BSTR newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->GetIMAPConfiguration()->SetIMAPPublicFolderName(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_SMTPDeliveryBindToIP(BSTR *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetSMTPConfiguration()->GetSMTPDeliveryBindToIP().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_SMTPDeliveryBindToIP(BSTR newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->GetSMTPConfiguration()->SetSMTPDeliveryBindToIP(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_IMAPSortEnabled(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetIMAPConfiguration()->GetUseIMAPSort() ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_IMAPSortEnabled(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->GetIMAPConfiguration()->SetUseIMAPSort(newVal == VARIANT_TRUE);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_IMAPQuotaEnabled(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetIMAPConfiguration()->GetUseIMAPQuota() ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_IMAPQuotaEnabled(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->GetIMAPConfiguration()->SetUseIMAPQuota(newVal == VARIANT_TRUE);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_IMAPIdleEnabled(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetIMAPConfiguration()->GetUseIMAPIdle() ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_IMAPIdleEnabled(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->GetIMAPConfiguration()->SetUseIMAPIdle(newVal == VARIANT_TRUE);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_IMAPACLEnabled(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetIMAPConfiguration()->GetUseIMAPACL() ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_IMAPACLEnabled(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->GetIMAPConfiguration()->SetUseIMAPACL(newVal == VARIANT_TRUE);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_WorkerThreadPriority(long *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetWorkerThreadPriority();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_WorkerThreadPriority(long newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->SetWorkerThreadPriority(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_TCPIPThreads(long *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetTCPIPThreads();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_TCPIPThreads(long newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->SetTCPIPThreads(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_MaxAsynchronousThreads(long *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetAsynchronousThreads();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_MaxAsynchronousThreads(long newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->SetAsynchronousThreads(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_CrashSimulationMode(long *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetCrashSimulationMode();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_CrashSimulationMode(long newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->SetCrashSimulationMode(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


STDMETHODIMP InterfaceSettings::get_MaxSMTPRecipientsInBatch(long *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetSMTPConfiguration()->GetMaxSMTPRecipientsInBatch();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_MaxSMTPRecipientsInBatch(long newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->GetSMTPConfiguration()->SetMaxSMTPRecipientsInBatch(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_DisconnectInvalidClients(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetDisconnectInvalidClients() ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_DisconnectInvalidClients(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->SetDisconnectInvalidClients(newVal == VARIANT_TRUE);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_MaxNumberOfInvalidCommands(long *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetMaximumIncorrectCommands();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_MaxNumberOfInvalidCommands(long newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->SetMaximumIncorrectCommands(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_SMTPRelayerUseSSL(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetSMTPConfiguration()->GetSMTPRelayerConnectionSecurity() == HM::CSSSL? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_SMTPRelayerUseSSL(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      if (newVal == VARIANT_TRUE)
         config_->GetSMTPConfiguration()->SetSMTPRelayerConnectionSecurity(HM::CSSSL);
      else
         config_->GetSMTPConfiguration()->SetSMTPRelayerConnectionSecurity(HM::CSNone);

      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


STDMETHODIMP InterfaceSettings::put_SMTPRelayerConnectionSecurity(eConnectionSecurity newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->GetSMTPConfiguration()->SetSMTPRelayerConnectionSecurity((HM::ConnectionSecurity) newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_SMTPRelayerConnectionSecurity(eConnectionSecurity *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = (eConnectionSecurity) config_->GetSMTPConfiguration()->GetSMTPRelayerConnectionSecurity();

      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_SMTPConnectionSecurity(eConnectionSecurity newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->GetSMTPConfiguration()->SetSMTPConnectionSecurity((HM::ConnectionSecurity) newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_SMTPConnectionSecurity(eConnectionSecurity *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = (eConnectionSecurity) config_->GetSMTPConfiguration()->GetSMTPConnectionSecurity();

      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


STDMETHODIMP InterfaceSettings::get_AddDeliveredToHeader(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetSMTPConfiguration()->GetAddDeliveredToHeader() ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_AddDeliveredToHeader(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->GetSMTPConfiguration()->SetAddDeliveredToHeader(newVal== VARIANT_TRUE);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_PublicFolders(IInterfaceIMAPFolders **pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return false;
   
      std::shared_ptr<HM::IMAPFolders> pIMAPFolders = HM::Configuration::Instance()->GetIMAPConfiguration()->GetPublicFolders();
   
      CComObject<InterfaceIMAPFolders>* pItem = new CComObject<InterfaceIMAPFolders>();
      pItem->SetAuthentication(authentication_);
      pItem->Attach(pIMAPFolders);
      pItem->AddRef();
   
      *pVal = pItem;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_PublicFolderDiskName(BSTR *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return false;
   
      *pVal = config_->GetIMAPConfiguration()->GetPublicFolderDiskName().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_Groups(IInterfaceGroups **pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return false;
   
      CComObject<InterfaceGroups>* pItem = new CComObject<InterfaceGroups>();
      pItem->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::Groups> pGroups = config_->GetIMAPConfiguration()->GetGroups();
      pGroups->Refresh();
   
      if (pGroups)
      {
         pItem->Attach(pGroups);
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

STDMETHODIMP InterfaceSettings::get_IncomingRelays(IInterfaceIncomingRelays **pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceIncomingRelays>* pItem = new CComObject<InterfaceIncomingRelays>();
      pItem->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::IncomingRelays> incomingRelays = config_->GetSMTPConfiguration()->GetIncomingRelays();
      
      if (incomingRelays)
      {
         pItem->Attach(incomingRelays);
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

STDMETHODIMP InterfaceSettings::get_MessageIndexing(IInterfaceMessageIndexing **pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceMessageIndexing>* pItem = new CComObject<InterfaceMessageIndexing>();
      pItem->SetAuthentication(authentication_);
      pItem->LoadSettings();
      pItem->AddRef();
   
      *pVal = pItem;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_AutoBanOnLogonFailure(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetAutoBanLogonEnabled() ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_AutoBanOnLogonFailure(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->SetAutoBanLogonEnabled(newVal == VARIANT_TRUE);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_MaxInvalidLogonAttempts(long *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetMaxInvalidLogonAttempts();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_MaxInvalidLogonAttempts(long newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->SetMaxInvalidLogonAttempts(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_MaxInvalidLogonAttemptsWithin(long *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetMaxLogonAttemptsWithin();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_MaxInvalidLogonAttemptsWithin(long newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->SetMaxLogonAttemptsWithin(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_AutoBanMinutes(long *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetAutoBanMinutes();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_AutoBanMinutes(long newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->SetAutoBanMinutes(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::ClearLogonFailureList()
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return false;
   
      config_->ClearOldLogonFailures();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_IMAPHierarchyDelimiter(BSTR *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetIMAPConfiguration()->GetHierarchyDelimiter().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_IMAPHierarchyDelimiter(BSTR newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      if (!config_->GetIMAPConfiguration()->SetHierarchyDelimiter(newVal))
         return COMError::GenerateError("It was not possible to change the IMAP hierarchy delimiter. It has probably failed because there exists one or more IMAP folders containing the new character.");
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_MaxNumberOfMXHosts(long *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetSMTPConfiguration()->GetMaxNumberOfMXHosts();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_MaxNumberOfMXHosts(long newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->GetSMTPConfiguration()->SetMaxNumberOfMXHosts(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


STDMETHODIMP InterfaceSettings::get_VerifyRemoteSslCertificate(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();


      if (config_->GetVerifyRemoteSslCertificate())
         *pVal = VARIANT_TRUE;
      else
         *pVal = VARIANT_FALSE;

      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_VerifyRemoteSslCertificate(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->SetVerifyRemoteSslCertificate(newVal == VARIANT_TRUE);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_SslCipherList(BSTR *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetSslCipherList().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_SslCipherList(BSTR newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->SetSslCipherList(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_TlsVersion10Enabled(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->SetSslVersionEnabled(HM::TlsVersion10, newVal == VARIANT_TRUE);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


STDMETHODIMP InterfaceSettings::get_TlsVersion10Enabled(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetSslVersionEnabled(HM::TlsVersion10) ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_TlsVersion11Enabled(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->SetSslVersionEnabled(HM::TlsVersion11, newVal == VARIANT_TRUE);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


STDMETHODIMP InterfaceSettings::get_TlsVersion11Enabled(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetSslVersionEnabled(HM::TlsVersion11) ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


STDMETHODIMP InterfaceSettings::put_TlsVersion12Enabled(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->SetSslVersionEnabled(HM::TlsVersion12, newVal == VARIANT_TRUE);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


STDMETHODIMP InterfaceSettings::get_TlsVersion12Enabled(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetSslVersionEnabled(HM::TlsVersion12) ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


STDMETHODIMP InterfaceSettings::put_TlsVersion13Enabled(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->SetSslVersionEnabled(HM::TlsVersion13, newVal == VARIANT_TRUE);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


STDMETHODIMP InterfaceSettings::get_TlsVersion13Enabled(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetSslVersionEnabled(HM::TlsVersion13) ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_IMAPMasterUser(BSTR *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();


      *pVal = config_->GetIMAPConfiguration()->GetIMAPMasterUser().AllocSysString();

      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_IMAPMasterUser(BSTR newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      HM::String sNewVal = newVal;
      config_->GetIMAPConfiguration()->SetIMAPMasterUser(sNewVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_IMAPSASLPlainEnabled(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->GetIMAPConfiguration()->SetUseIMAPSASLPlain(newVal == VARIANT_TRUE);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


STDMETHODIMP InterfaceSettings::get_IMAPSASLPlainEnabled(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetIMAPConfiguration()->GetUseIMAPSASLPlain() ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_IMAPSASLInitialResponseEnabled(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->GetIMAPConfiguration()->SetUseIMAPSASLInitialResponse(newVal == VARIANT_TRUE);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


STDMETHODIMP InterfaceSettings::get_IMAPSASLInitialResponseEnabled(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetIMAPConfiguration()->GetUseIMAPSASLInitialResponse() ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_IPv6PreferredEnabled(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetIPv6Preferred() ? VARIANT_TRUE : VARIANT_FALSE;

      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_IPv6PreferredEnabled(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->SetIPv6Preferred(newVal == VARIANT_TRUE);

      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}