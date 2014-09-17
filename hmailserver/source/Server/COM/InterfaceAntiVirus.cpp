// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"

#include "InterfaceAntiVirus.h"
#include "InterfaceBlockedAttachments.h"

#include "../SMTP/SMTPConfiguration.h"
#include "../Common/AntiVirus/VirusScannerTester.h"
#include "../Common/BO/BlockedAttachments.h"

InterfaceAntiVirus::InterfaceAntiVirus() :
   antiVirusConfiguration_(HM::Configuration::Instance()->GetAntiVirusConfiguration())
{
   
}

STDMETHODIMP InterfaceAntiVirus::get_ClamWinEnabled(VARIANT_BOOL *pVal)
{
   try
   {
      if (!GetIsServerAdmin())
         return GetAccessDenied();

      *pVal = antiVirusConfiguration_.ClamWinEnabled() ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiVirus::put_ClamWinEnabled(VARIANT_BOOL newVal)
{
   try
   {
      if (!GetIsServerAdmin())
         return GetAccessDenied();

      antiVirusConfiguration_.ClamWinEnabled (newVal == VARIANT_TRUE ? true : false);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiVirus::get_ClamWinExecutable(BSTR *pVal)
{
   try
   {
      if (!GetIsServerAdmin())
         return GetAccessDenied();

      *pVal = antiVirusConfiguration_.ClamWinExecutable().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiVirus::put_ClamWinExecutable(BSTR newVal)
{
   try
   {
      if (!GetIsServerAdmin())
         return GetAccessDenied();

      antiVirusConfiguration_.ClamWinExecutable(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiVirus::get_ClamWinDBFolder(BSTR *pVal)
{
   try
   {
      if (!GetIsServerAdmin())
         return GetAccessDenied();

      *pVal = antiVirusConfiguration_.ClamWinDatabase().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiVirus::put_ClamWinDBFolder(BSTR newVal)
{
   try
   {
      if (!GetIsServerAdmin())
         return GetAccessDenied();

      antiVirusConfiguration_.ClamWinDatabase(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiVirus::get_CustomScannerEnabled(VARIANT_BOOL *pVal)
{
   try
   {
      if (!GetIsServerAdmin())
         return GetAccessDenied();

      *pVal = antiVirusConfiguration_.GetCustomScannerEnabled() ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiVirus::put_CustomScannerEnabled(VARIANT_BOOL newVal)
{
   try
   {
      if (!GetIsServerAdmin())
         return GetAccessDenied();

      antiVirusConfiguration_.SetCustomScannerEnabled (newVal == VARIANT_TRUE ? true : false);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiVirus::get_CustomScannerExecutable(BSTR *pVal)
{
   try
   {
      if (!GetIsServerAdmin())
         return GetAccessDenied();

      *pVal = antiVirusConfiguration_.GetCustomScannerExecutable().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiVirus::put_CustomScannerExecutable(BSTR newVal)
{
   try
   {
      if (!GetIsServerAdmin())
         return GetAccessDenied();

      antiVirusConfiguration_.SetCustomScannerExecutable(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiVirus::get_CustomScannerReturnValue(long *pVal)
{
   try
   {
      if (!GetIsServerAdmin())
         return GetAccessDenied();

      *pVal = antiVirusConfiguration_.GetCustomScannerReturnValue();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiVirus::put_CustomScannerReturnValue(long  newVal)
{
   try
   {
      if (!GetIsServerAdmin())
         return GetAccessDenied();

      antiVirusConfiguration_.SetCustomScannerReturnValue(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiVirus::get_Action(eAntivirusAction *pVal)
{
   try
   {
      if (!GetIsServerAdmin())
         return GetAccessDenied();

      switch (antiVirusConfiguration_.AVAction())
      {
      case HM::AntiVirusConfiguration::ActionDelete:
         *pVal = hDeleteEmail;
         break;
      case HM::AntiVirusConfiguration::ActionStripAttachments:
         *pVal = hDeleteAttachments;
         break;
      }
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiVirus::put_Action(eAntivirusAction newVal)
{
   try
   {
      if (!GetIsServerAdmin())
         return GetAccessDenied();

      HM::AntiVirusConfiguration::eAVAction iAction;
      switch (newVal)
      {
      case hDeleteEmail:
         iAction = HM::AntiVirusConfiguration::ActionDelete;
         break;
      case hDeleteAttachments:
         iAction = HM::AntiVirusConfiguration::ActionStripAttachments;
      }
   
      antiVirusConfiguration_.AVAction (iAction);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiVirus::get_NotifySender(VARIANT_BOOL *pVal)
{
   try
   {
      if (!GetIsServerAdmin())
         return GetAccessDenied();

      *pVal = antiVirusConfiguration_.AVNotifySender() ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiVirus::put_NotifySender(VARIANT_BOOL newVal)
{
   try
   {
      if (!GetIsServerAdmin())
         return GetAccessDenied();

      antiVirusConfiguration_.AVNotifySender(newVal == VARIANT_TRUE ? true : false);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiVirus::get_NotifyReceiver(VARIANT_BOOL *pVal)
{
   try
   {
      if (!GetIsServerAdmin())
         return GetAccessDenied();

      *pVal = antiVirusConfiguration_.AVNotifyReceiver() ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiVirus::put_NotifyReceiver(VARIANT_BOOL newVal)
{
   try
   {
      if (!GetIsServerAdmin())
         return GetAccessDenied();

      antiVirusConfiguration_.AVNotifyReceiver(newVal == VARIANT_TRUE ? true : false);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiVirus::get_MaximumMessageSize(long *pVal)
{
   try
   {
      if (!GetIsServerAdmin())
         return GetAccessDenied();

      *pVal = antiVirusConfiguration_.GetVirusScanMaxSize();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiVirus::put_MaximumMessageSize(long  newVal)
{
   try
   {
      if (!GetIsServerAdmin())
         return GetAccessDenied();

      antiVirusConfiguration_.SetVirusScanMaxSize(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiVirus::get_BlockedAttachments(IInterfaceBlockedAttachments **pVal)
{
   try
   {
      if (!GetIsServerAdmin())
         return GetAccessDenied();

      CComObject<InterfaceBlockedAttachments>* pInterface = new CComObject<InterfaceBlockedAttachments>;
      pInterface->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::BlockedAttachments> pBlockedAttachments = HM::Configuration::Instance()->GetBlockedAttachments();
   
      pInterface->Attach(pBlockedAttachments);
      pInterface->AddRef();
   
      *pVal = pInterface;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiVirus::put_EnableAttachmentBlocking(VARIANT_BOOL newVal)
{
   try
   {
      if (!GetIsServerAdmin())
         return GetAccessDenied();
   
      antiVirusConfiguration_.SetEnableAttachmentBlocking(newVal == VARIANT_TRUE ? true : false);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiVirus::get_EnableAttachmentBlocking(VARIANT_BOOL *pVal)
{
   try
   {
      if (!GetIsServerAdmin())
         return GetAccessDenied();

      *pVal = antiVirusConfiguration_.GetEnableAttachmentBlocking() ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiVirus::get_ClamAVEnabled(VARIANT_BOOL *pVal)
{
   try
   {
      if (!GetIsServerAdmin())
         return GetAccessDenied();

      *pVal = antiVirusConfiguration_.GetClamAVEnabled() ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiVirus::put_ClamAVEnabled(VARIANT_BOOL newVal)
{
   try
   {
      if (!GetIsServerAdmin())
         return GetAccessDenied();

      antiVirusConfiguration_.SetClamAVEnabled (newVal == VARIANT_TRUE ? true : false);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiVirus::get_ClamAVHost(BSTR *pVal)
{
   try
   {
      if (!GetIsServerAdmin())
         return GetAccessDenied();

      *pVal = antiVirusConfiguration_.GetClamAVHost().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiVirus::put_ClamAVHost(BSTR host)
{
   try
   {
      if (!GetIsServerAdmin())
         return GetAccessDenied();

   
      antiVirusConfiguration_.SetClamAVHost(host);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiVirus::get_ClamAVPort(long *pVal)
{
   try
   {
      if (!GetIsServerAdmin())
         return GetAccessDenied();

      *pVal = antiVirusConfiguration_.GetClamAVPort();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiVirus::put_ClamAVPort(long newVal)
{
   try
   {
      if (!GetIsServerAdmin())
         return GetAccessDenied();

      antiVirusConfiguration_.SetClamAVPort(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiVirus::TestCustomerScanner(BSTR hostname, long port, BSTR *messageText, VARIANT_BOOL *pResult)
{
   try
   {
      if (!GetIsServerAdmin())
         return GetAccessDenied();

      HM::VirusScannerTester testClient;
   
      HM::String text;
      *pResult = testClient.TestCustomVirusScanner(hostname, port, text) ? VARIANT_TRUE : VARIANT_FALSE;
      *messageText = text.AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiVirus::TestClamWinScanner(BSTR clamWinExecutable, BSTR clamWinDatabase, BSTR *messageText, VARIANT_BOOL *pResult)
{
   try
   {
      if (!GetIsServerAdmin())
         return GetAccessDenied();

      HM::VirusScannerTester testClient;
   
      HM::String text;
      *pResult = testClient.TestClamWinVirusScanner(clamWinExecutable, clamWinDatabase, text) ? VARIANT_TRUE : VARIANT_FALSE;
      *messageText = text.AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiVirus::TestClamAVScanner(BSTR hostname, long port, BSTR *messageText, VARIANT_BOOL *pResult)
{
   try
   {
      if (!GetIsServerAdmin())
         return GetAccessDenied();

      HM::VirusScannerTester testClient;
   
      HM::String text;
      *pResult = testClient.TestClamAVConnect(hostname, port, text) ? VARIANT_TRUE : VARIANT_FALSE;
      *messageText = text.AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


