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

#ifdef _DEBUG
long InterfaceSettings::counter = 0;
#endif

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

   m_pConfig = HM::Configuration::Instance();
   m_pIniFileSettings = HM::IniFileSettings::Instance();

   return true;
}

STDMETHODIMP InterfaceSettings::get_MaxMessageSize(long *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetSMTPConfiguration()->GetMaxMessageSize();
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
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetSMTPConfiguration()->SetMaxMessageSize(newVal);
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
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetSMTPConfiguration()->GetMaxSMTPConnections();
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
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetSMTPConfiguration()->SetMaxSMTPConnections(newVal);
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
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetIMAPConfiguration()->GetMaxIMAPConnections();
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
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetIMAPConfiguration()->SetMaxIMAPConnections(newVal);
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
      if (!m_pConfig)
         return GetAccessDenied();

   
      *pVal = m_pConfig->GetPOP3Configuration()->GetMaxPOP3Connections();
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetPOP3Configuration()->SetMaxPOP3Connections(newVal);
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetMirrorAddress().AllocSysString();
   
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
      if (!m_pConfig)
         return GetAccessDenied();

   
      m_pConfig->SetMirrorAddress(newVal);
   
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
      if (!m_pConfig)
         return GetAccessDenied();

   
      if (m_pConfig->GetSMTPConfiguration()->GetAuthAllowPlainText())
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
      if (!m_pConfig)
         return GetAccessDenied();

   
      if (newVal == VARIANT_TRUE)
         m_pConfig->GetSMTPConfiguration()->SetAuthAllowPlainText(true);
      else
         m_pConfig->GetSMTPConfiguration()->SetAuthAllowPlainText(false);
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      if (m_pConfig->GetSMTPConfiguration()->GetAllowMailFromNull())
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
      if (!m_pConfig)
         return GetAccessDenied();

   
   
      if (newVal == VARIANT_TRUE)
         m_pConfig->GetSMTPConfiguration()->SetAllowMailFromNull (false);
      else
         m_pConfig->GetSMTPConfiguration()->SetAllowMailFromNull (true);
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetSMTPConfiguration()->GetAllowIncorrectLineEndings() ? VARIANT_TRUE : VARIANT_FALSE;
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetSMTPConfiguration()->SetAllowIncorrectLineEndings(newVal == VARIANT_TRUE);
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      CComObject<InterfaceLogging>* pInterfaceLogging = new CComObject<InterfaceLogging>;
      
      pInterfaceLogging->SetAuthentication(m_pAuthentication);
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
      if (!m_pConfig)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      CComObject<InterfaceAntiVirus>* pInterfaceAntiVirus = new CComObject<InterfaceAntiVirus>;
   
      pInterfaceAntiVirus->SetAuthentication(m_pAuthentication);
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      CComObject<InterfaceCache>* pInterfaceCache = new CComObject<InterfaceCache>;
      
      pInterfaceCache->SetAuthentication(m_pAuthentication);
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
      if (!m_pConfig)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      CComObject<InterfaceSecurityRanges>* pRangesInt = new CComObject<InterfaceSecurityRanges>();
      pRangesInt->SetAuthentication(m_pAuthentication);
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
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetSMTPConfiguration()->GetNoOfRetries();
   
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
      if (!m_pConfig)
         return GetAccessDenied();

   
      m_pConfig->GetSMTPConfiguration()->SetNoOfRetries(newVal);
   
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
      if (!m_pConfig)
         return GetAccessDenied();

   
   
      *pVal = m_pConfig->GetSMTPConfiguration()->GetMinutesBetweenTry();
   
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
      if (!m_pConfig)
         return GetAccessDenied();

   
      m_pConfig->GetSMTPConfiguration()->SetMinutesBetweenTry(newVal);
   
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
      if (!m_pConfig)
         return GetAccessDenied();

   
   
      *pVal = m_pConfig->GetSMTPConfiguration()->GetMaxNoOfDeliveryThreads();
   
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
      if (!m_pConfig)
         return GetAccessDenied();

   
      m_pConfig->GetSMTPConfiguration()->SetMaxNoOfDeliveryThreads(newVal);
   
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
      if (!m_pConfig)
         return GetAccessDenied();

   
      *pVal = m_pConfig->GetSMTPConfiguration()->GetSMTPRelayer().AllocSysString();
   
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
      if (!m_pConfig)
         return GetAccessDenied();

   
      m_pConfig->GetSMTPConfiguration()->SetSMTPRelayer(newVal);
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
      if (!m_pConfig)
         return GetAccessDenied();

   
      *pVal = m_pConfig->GetSMTPConfiguration()->GetSMTPRelayerPort();
   
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
      if (!m_pConfig)
         return GetAccessDenied();

   
      m_pConfig->GetSMTPConfiguration()->SetSMTPRelayerPort(newVal);
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
      if (!m_pConfig)
         return GetAccessDenied();

   
      *pVal = m_pConfig->GetHostName().AllocSysString();
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      HM::String sNewVal = newVal;
      m_pConfig->SetHostName(sNewVal);
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
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetSMTPConfiguration()->GetWelcomeMessage().AllocSysString();
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetSMTPConfiguration()->SetWelcomeMessage(newVal);
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetPOP3Configuration()->GetWelcomeMessage().AllocSysString();
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetPOP3Configuration()->SetWelcomeMessage(newVal);
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetIMAPConfiguration()->GetWelcomeMessage().AllocSysString();
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetIMAPConfiguration()->SetWelcomeMessage(newVal);
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      if (m_pConfig->GetUseSMTP())
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
      if (!m_pConfig)
         return GetAccessDenied();

      if (newVal == VARIANT_TRUE)
         m_pConfig->SetUseSMTP(true);
      else
         m_pConfig->SetUseSMTP(false);
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      if (m_pConfig->GetUsePOP3())
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
      if (!m_pConfig)
         return GetAccessDenied();

      if (newVal == VARIANT_TRUE)
         m_pConfig->SetUsePOP3(true);
      else
         m_pConfig->SetUsePOP3(false);
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetUseIMAP() ? VARIANT_TRUE : VARIANT_FALSE;
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->SetUseIMAP(newVal == VARIANT_TRUE ? true : false);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::get_SendStatistics(VARIANT_BOOL *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetSendStatistics() ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSettings::put_SendStatistics(VARIANT_BOOL newVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->SetSendStatistics(newVal == VARIANT_TRUE);
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetSMTPConfiguration()->GetSMTPRelayerRequiresAuthentication() ? VARIANT_TRUE : VARIANT_FALSE;
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetSMTPConfiguration()->SetSMTPRelayerRequiresAuthentication(newVal == VARIANT_TRUE);
   
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
      if (!m_pConfig)
         return GetAccessDenied();

   
      *pVal = m_pConfig->GetSMTPConfiguration()->GetSMTPRelayerUsername().AllocSysString();
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetSMTPConfiguration()->SetSMTPRelayerUsername(newVal);
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
      if (!m_pConfig)
         return GetAccessDenied();

   
      *pVal = m_pIniFileSettings->GetUserInterfaceLanguage().AllocSysString();
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      m_pIniFileSettings->SetUserInterfaceLanguage(newVal);
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
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetSMTPConfiguration()->SetSMTPRelayerPassword(newVal);
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
      if (!m_pConfig)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return false;
      
   
      m_pIniFileSettings->SetAdministratorPassword(newVal);
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      CComObject<InterfaceRoutes>* pRoutesInterface = new CComObject<InterfaceRoutes>();
      pRoutesInterface->SetAuthentication(m_pAuthentication);
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
      if (!m_pConfig)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      CComObject<InterfaceScripting>* pItem = new CComObject<InterfaceScripting >();
      pItem->SetAuthentication(m_pAuthentication);
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
      if (!m_pConfig)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      CComObject<InterfaceBackupSettings>* pItem = new CComObject<InterfaceBackupSettings>();
     
      pItem->SetAuthentication(m_pAuthentication);
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
      if (!m_pConfig)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      CComObject<InterfaceDirectories>* pItem = new CComObject<InterfaceDirectories>();
   
      m_pIniFileSettings->LoadSettings();
   
      pItem->SetAuthentication(m_pAuthentication);
      pItem->LoadSettings(m_pIniFileSettings);
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
      if (!m_pConfig)
         return GetAccessDenied();

      CComObject<InterfaceAntiSpam>* pItem = new CComObject<InterfaceAntiSpam>();
      
      pItem->SetAuthentication(m_pAuthentication);
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
      if (!m_pConfig)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      CComObject<InterfaceServerMessages>* pItem = new CComObject<InterfaceServerMessages>();
      
      pItem->SetAuthentication(m_pAuthentication);
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
      if (!m_pConfig)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      CComObject<InterfaceTCPIPPorts>* pItem = new CComObject<InterfaceTCPIPPorts>();
      pItem->SetAuthentication(m_pAuthentication);
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
      if (!m_pConfig)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return false;
   
      shared_ptr<HM::SSLCertificates> pSSLCertificates = HM::Configuration::Instance()->GetSSLCertificates();
   
      CComObject<InterfaceSSLCertificates>* pItem = new CComObject<InterfaceSSLCertificates>();
      pItem->SetAuthentication(m_pAuthentication);
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
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetSMTPConfiguration()->GetRuleLoopLimit();
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetSMTPConfiguration()->SetRuleLoopLimit(newVal);
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
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetDefaultDomain().AllocSysString();
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
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->SetDefaultDomain(newVal);
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
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetIMAPConfiguration()->GetIMAPPublicFolderName().AllocSysString();
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
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetIMAPConfiguration()->SetIMAPPublicFolderName(newVal);
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
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetSMTPConfiguration()->GetSMTPDeliveryBindToIP().AllocSysString();
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
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetSMTPConfiguration()->SetSMTPDeliveryBindToIP(newVal);
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
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetIMAPConfiguration()->GetUseIMAPSort() ? VARIANT_TRUE : VARIANT_FALSE;
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
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetIMAPConfiguration()->SetUseIMAPSort(newVal == VARIANT_TRUE);
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
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetIMAPConfiguration()->GetUseIMAPQuota() ? VARIANT_TRUE : VARIANT_FALSE;
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
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetIMAPConfiguration()->SetUseIMAPQuota(newVal == VARIANT_TRUE);
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
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetIMAPConfiguration()->GetUseIMAPIdle() ? VARIANT_TRUE : VARIANT_FALSE;
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
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetIMAPConfiguration()->SetUseIMAPIdle(newVal == VARIANT_TRUE);
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
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetIMAPConfiguration()->GetUseIMAPACL() ? VARIANT_TRUE : VARIANT_FALSE;
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
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetIMAPConfiguration()->SetUseIMAPACL(newVal == VARIANT_TRUE);
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
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetWorkerThreadPriority();
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->SetWorkerThreadPriority(newVal);
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetTCPIPThreads();
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
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->SetTCPIPThreads(newVal);
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
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetAsynchronousThreads();
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
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->SetAsynchronousThreads(newVal);
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
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetSMTPConfiguration()->GetMaxSMTPRecipientsInBatch();
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetSMTPConfiguration()->SetMaxSMTPRecipientsInBatch(newVal);
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetDisconnectInvalidClients() ? VARIANT_TRUE : VARIANT_FALSE;
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->SetDisconnectInvalidClients(newVal == VARIANT_TRUE);
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetMaximumIncorrectCommands();
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->SetMaximumIncorrectCommands(newVal);
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetSMTPConfiguration()->GetSMTPRelayerUseSSL() ? VARIANT_TRUE : VARIANT_FALSE;
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
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetSMTPConfiguration()->SetSMTPRelayerUseSSL(newVal== VARIANT_TRUE);
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
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetSMTPConfiguration()->GetAddDeliveredToHeader() ? VARIANT_TRUE : VARIANT_FALSE;
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
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetSMTPConfiguration()->SetAddDeliveredToHeader(newVal== VARIANT_TRUE);
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
      if (!m_pConfig)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return false;
   
      shared_ptr<HM::IMAPFolders> pIMAPFolders = HM::Configuration::Instance()->GetIMAPConfiguration()->GetPublicFolders();
   
      CComObject<InterfaceIMAPFolders>* pItem = new CComObject<InterfaceIMAPFolders>();
      pItem->SetAuthentication(m_pAuthentication);
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
      if (!m_pConfig)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return false;
   
      *pVal = m_pConfig->GetIMAPConfiguration()->GetPublicFolderDiskName().AllocSysString();
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
      if (!m_pConfig)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return false;
   
      CComObject<InterfaceGroups>* pItem = new CComObject<InterfaceGroups>();
      pItem->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::Groups> pGroups = m_pConfig->GetIMAPConfiguration()->GetGroups();
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
      if (!m_pConfig)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      CComObject<InterfaceIncomingRelays>* pItem = new CComObject<InterfaceIncomingRelays>();
      pItem->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::IncomingRelays> incomingRelays = m_pConfig->GetSMTPConfiguration()->GetIncomingRelays();
      
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
      if (!m_pConfig)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      CComObject<InterfaceMessageIndexing>* pItem = new CComObject<InterfaceMessageIndexing>();
      pItem->SetAuthentication(m_pAuthentication);
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
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetAutoBanLogonEnabled() ? VARIANT_TRUE : VARIANT_FALSE;
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->SetAutoBanLogonEnabled(newVal == VARIANT_TRUE);
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetMaxInvalidLogonAttempts();
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->SetMaxInvalidLogonAttempts(newVal);
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetMaxLogonAttemptsWithin();
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->SetMaxLogonAttemptsWithin(newVal);
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetAutoBanMinutes();
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->SetAutoBanMinutes(newVal);
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return false;
   
      m_pConfig->ClearOldLogonFailures();
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetIMAPConfiguration()->GetHierarchyDelimiter().AllocSysString();
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
      if (!m_pConfig)
         return GetAccessDenied();

      if (!m_pConfig->GetIMAPConfiguration()->SetHierarchyDelimiter(newVal))
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
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetSMTPConfiguration()->GetMaxNumberOfMXHosts();
   
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
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetSMTPConfiguration()->SetMaxNumberOfMXHosts(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

