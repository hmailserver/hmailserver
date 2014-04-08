// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"


#include "../Common/AntiSpam/SpamAssassin/SpamAssassinTestConnect.h"

#include "../Common/AntiSpam/AntiSpamConfiguration.h"
#include "../Common/AntiSpam/DKIM/DKIM.h"

#include "..\Common\BO\WhiteListAddresses.h"

#include "InterfaceAntiSpam.h"
#include "InterfaceGreyListingWhiteAddresses.h"
#include "InterfaceWhiteListAddresses.h"
#include "InterfaceSURBLServers.h"
#include "InterfaceDNSBlackLists.h"

bool 
InterfaceAntiSpam::LoadSettings()
{
   if (!GetIsServerAdmin())
      return false;

   m_pConfig = HM::Configuration::Instance();

   return true;
}


STDMETHODIMP InterfaceAntiSpam::get_SURBLServers(IInterfaceSURBLServers **pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      CComObject<InterfaceSURBLServers>* pSURBLServersInterface = new CComObject<InterfaceSURBLServers>();
      pSURBLServersInterface->SetAuthentication(m_pAuthentication);
      pSURBLServersInterface->LoadSettings();
   
      pSURBLServersInterface->AddRef();
      *pVal = pSURBLServersInterface;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::get_CheckHostInHelo(VARIANT_BOOL *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetAntiSpamConfiguration().GetCheckHostInHelo() ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::put_CheckHostInHelo(VARIANT_BOOL newVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetAntiSpamConfiguration().SetCheckHostInHelo(newVal == VARIANT_TRUE ? true : false);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::get_CheckHostInHeloScore(long *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetAntiSpamConfiguration().GetCheckHostInHeloScore();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::put_CheckHostInHeloScore(long newVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetAntiSpamConfiguration().SetCheckHostInHeloScore(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::get_SpamMarkThreshold(long *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetAntiSpamConfiguration().GetSpamMarkThreshold();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::put_SpamMarkThreshold(long newVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetAntiSpamConfiguration().SetSpamMarkThreshold(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::get_SpamDeleteThreshold(long *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetAntiSpamConfiguration().GetSpamDeleteThreshold();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::put_SpamDeleteThreshold(long newVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetAntiSpamConfiguration().SetSpamDeleteThreshold(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::get_GreyListingEnabled(VARIANT_BOOL *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetAntiSpamConfiguration().GetUseGreyListing() ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::put_GreyListingEnabled(VARIANT_BOOL newVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetAntiSpamConfiguration().SetUseGreyListing(newVal == VARIANT_TRUE);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::get_GreyListingInitialDelay(long *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetAntiSpamConfiguration().GetGreyListingInitialDelay();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::put_GreyListingInitialDelay(long newVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetAntiSpamConfiguration().SetGreyListingInitialDelay(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::get_GreyListingInitialDelete(long *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetAntiSpamConfiguration().GetGreyListingInitialDelete();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::put_GreyListingInitialDelete(long newVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetAntiSpamConfiguration().SetGreyListingInitialDelete(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::get_GreyListingFinalDelete(long *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetAntiSpamConfiguration().GetGreyListingFinalDelete();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::put_GreyListingFinalDelete(long newVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetAntiSpamConfiguration().SetGreyListingFinalDelete(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::get_AddHeaderSpam(VARIANT_BOOL *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetAntiSpamConfiguration().GetAddHeaderSpam() ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::put_AddHeaderSpam(VARIANT_BOOL newVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetAntiSpamConfiguration().SetAddHeaderSpam(newVal == VARIANT_TRUE);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::get_AddHeaderReason(VARIANT_BOOL *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetAntiSpamConfiguration().GetAddHeaderReason() ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::put_AddHeaderReason(VARIANT_BOOL newVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetAntiSpamConfiguration().SetAddHeaderReason(newVal == VARIANT_TRUE);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::get_PrependSubject(VARIANT_BOOL *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetAntiSpamConfiguration().GetPrependSubject() ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::put_PrependSubject(VARIANT_BOOL newVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetAntiSpamConfiguration().SetPrependSubject(newVal == VARIANT_TRUE);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::get_PrependSubjectText(BSTR *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetAntiSpamConfiguration().GetPrependSubjectText().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::put_PrependSubjectText(BSTR newVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetAntiSpamConfiguration().SetPrependSubjectText(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::get_GreyListingWhiteAddresses(IInterfaceGreyListingWhiteAddresses **pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      if (!m_pConfig)
         return DISP_E_BADINDEX;
   
      CComObject<InterfaceGreyListingWhiteAddresses>* pGLWhiteList = new CComObject<InterfaceGreyListingWhiteAddresses>();
      pGLWhiteList->SetAuthentication(m_pAuthentication);
      pGLWhiteList->Attach(HM::Configuration::Instance()->GetAntiSpamConfiguration().GetGreyListingWhiteAddresses());
      pGLWhiteList->AddRef();
   
      *pVal = pGLWhiteList;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::get_WhiteListAddresses(IInterfaceWhiteListAddresses **pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      if (!m_pConfig)
         return DISP_E_BADINDEX;
   
      CComObject<InterfaceWhiteListAddresses>* pGLWhiteList = new CComObject<InterfaceWhiteListAddresses>();
      pGLWhiteList->SetAuthentication(m_pAuthentication);
      pGLWhiteList->Attach(HM::Configuration::Instance()->GetAntiSpamConfiguration().GetWhiteListAddresses());
      pGLWhiteList->AddRef();
   
      *pVal = pGLWhiteList;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::get_UseSPF(VARIANT_BOOL *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetAntiSpamConfiguration().GetUseSPF() ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::put_UseSPF(VARIANT_BOOL newVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetAntiSpamConfiguration().SetUseSPF(newVal == VARIANT_TRUE ? true : false);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::get_UseSPFScore(long *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetAntiSpamConfiguration().GetUseSPFScore();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::put_UseSPFScore(long newVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetAntiSpamConfiguration().SetUseSPFScore(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::get_UseMXChecks(VARIANT_BOOL *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetAntiSpamConfiguration().GetUseMXChecks() ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::put_UseMXChecks(VARIANT_BOOL newVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetAntiSpamConfiguration().SetUseMXChecks(newVal == VARIANT_TRUE ? true : false);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::get_UseMXChecksScore(long *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetAntiSpamConfiguration().GetUseMXChecksScore();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::put_UseMXChecksScore(long newVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetAntiSpamConfiguration().SetUseMXChecksScore(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::get_DNSBlackLists(IInterfaceDNSBlackLists **pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      CComObject<InterfaceDNSBlackLists>* pDNSBlackListsInterface = new CComObject<InterfaceDNSBlackLists>();
      pDNSBlackListsInterface->SetAuthentication(m_pAuthentication);
      pDNSBlackListsInterface->LoadSettings();
   
      pDNSBlackListsInterface->AddRef();
      *pVal = pDNSBlackListsInterface;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::get_TarpitDelay(long *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = 0;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::put_TarpitDelay(long newVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      // OBSOLETE: To be removed in v6.
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::get_TarpitCount(long *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      // OBSOLETE: To be removed in v6.
      *pVal = 0;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::put_TarpitCount(long newVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      // OBSOLETE: To be removed in v6.
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::get_SpamAssassinEnabled(VARIANT_BOOL *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetAntiSpamConfiguration().GetSpamAssassinEnabled() ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::put_SpamAssassinEnabled(VARIANT_BOOL newVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetAntiSpamConfiguration().SetSpamAssassinEnabled(newVal == VARIANT_TRUE ? true : false);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::get_SpamAssassinScore(long *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetAntiSpamConfiguration().GetSpamAssassinScore();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::put_SpamAssassinScore(long newVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetAntiSpamConfiguration().SetSpamAssassinScore(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::get_SpamAssassinMergeScore(VARIANT_BOOL *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetAntiSpamConfiguration().GetSpamAssassinMergeScore() ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::put_SpamAssassinMergeScore(VARIANT_BOOL newVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetAntiSpamConfiguration().SetSpamAssassinMergeScore(newVal == VARIANT_TRUE);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::get_SpamAssassinHost(BSTR *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetAntiSpamConfiguration().GetSpamAssassinHost().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::put_SpamAssassinHost(BSTR newVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetAntiSpamConfiguration().SetSpamAssassinHost(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::get_SpamAssassinPort(long *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetAntiSpamConfiguration().GetSpamAssassinPort();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::put_SpamAssassinPort(long newVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetAntiSpamConfiguration().SetSpamAssassinPort(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::get_MaximumMessageSize(long *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetAntiSpamConfiguration().GetAntiSpamMaxSizeKB();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::put_MaximumMessageSize(long newVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetAntiSpamConfiguration().SetAntiSpamMaxSizeKB(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::ClearGreyListingTriplets()
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetAntiSpamConfiguration().ClearGreyListingTriplets();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::DKIMVerify(BSTR pVal, eDKIMResult *pResult)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      HM::DKIM dkim;
      *pResult = (eDKIMResult) dkim.Verify(pVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::TestSpamAssassinConnection(BSTR hostname, long port, BSTR *messageText, VARIANT_BOOL *pResult)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      HM::SpamAssassinTestConnect testClient;
      
      HM::String text;
      *pResult = testClient.TestConnect(hostname, port, text) ? VARIANT_TRUE : VARIANT_FALSE;
      *messageText = text.AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::get_DKIMVerificationEnabled(VARIANT_BOOL *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetAntiSpamConfiguration().GetDKIMVerificationEnabled() ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::put_DKIMVerificationEnabled(VARIANT_BOOL newVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetAntiSpamConfiguration().SetDKIMVerificationEnabled(newVal == VARIANT_TRUE);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::get_DKIMVerificationFailureScore(long *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetAntiSpamConfiguration().GetDKIMVerificationFailureScore();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::put_DKIMVerificationFailureScore(long newVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetAntiSpamConfiguration().SetDKIMVerificationFailureScore(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::get_BypassGreylistingOnSPFSuccess(VARIANT_BOOL *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetAntiSpamConfiguration().GetBypassGreyListingOnSPFSuccess() ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::put_BypassGreylistingOnSPFSuccess(VARIANT_BOOL newValue)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetAntiSpamConfiguration().SetBypassGreyListingOnSPFSuccess(newValue == VARIANT_TRUE);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::get_BypassGreylistingOnMailFromMX(VARIANT_BOOL *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pConfig->GetAntiSpamConfiguration().GetBypassGreyListingOnMailFromMX() ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::put_BypassGreylistingOnMailFromMX(VARIANT_BOOL newValue)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      m_pConfig->GetAntiSpamConfiguration().SetBypassGreyListingOnMailFromMX(newValue == VARIANT_TRUE);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


