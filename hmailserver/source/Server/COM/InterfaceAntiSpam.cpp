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

InterfaceAntiSpam::InterfaceAntiSpam() :
   config_(nullptr)
{

}

bool 
InterfaceAntiSpam::LoadSettings()
{
   if (!GetIsServerAdmin())
      return false;

   config_ = HM::Configuration::Instance();

   return true;
}


STDMETHODIMP InterfaceAntiSpam::get_SURBLServers(IInterfaceSURBLServers **pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      CComObject<InterfaceSURBLServers>* pSURBLServersInterface = new CComObject<InterfaceSURBLServers>();
      pSURBLServersInterface->SetAuthentication(authentication_);
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
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetAntiSpamConfiguration().GetCheckHostInHelo() ? VARIANT_TRUE : VARIANT_FALSE;
   
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
      if (!config_)
         return GetAccessDenied();

      config_->GetAntiSpamConfiguration().SetCheckHostInHelo(newVal == VARIANT_TRUE ? true : false);
   
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
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetAntiSpamConfiguration().GetCheckHostInHeloScore();
   
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
      if (!config_)
         return GetAccessDenied();

      config_->GetAntiSpamConfiguration().SetCheckHostInHeloScore(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::get_CheckPTR(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetAntiSpamConfiguration().GetCheckPTR() ? VARIANT_TRUE : VARIANT_FALSE;

      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::put_CheckPTR(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->GetAntiSpamConfiguration().SetCheckPTR(newVal == VARIANT_TRUE ? true : false);

      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::get_CheckPTRScore(long *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetAntiSpamConfiguration().GetCheckPTRScore();

      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAntiSpam::put_CheckPTRScore(long newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->GetAntiSpamConfiguration().SetCheckPTRScore(newVal);

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
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetAntiSpamConfiguration().GetSpamMarkThreshold();
   
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
      if (!config_)
         return GetAccessDenied();

      config_->GetAntiSpamConfiguration().SetSpamMarkThreshold(newVal);
   
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
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetAntiSpamConfiguration().GetSpamDeleteThreshold();
   
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
      if (!config_)
         return GetAccessDenied();

      config_->GetAntiSpamConfiguration().SetSpamDeleteThreshold(newVal);
   
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
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetAntiSpamConfiguration().GetUseGreyListing() ? VARIANT_TRUE : VARIANT_FALSE;
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
      if (!config_)
         return GetAccessDenied();

      config_->GetAntiSpamConfiguration().SetUseGreyListing(newVal == VARIANT_TRUE);
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
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetAntiSpamConfiguration().GetGreyListingInitialDelay();
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
      if (!config_)
         return GetAccessDenied();

      config_->GetAntiSpamConfiguration().SetGreyListingInitialDelay(newVal);
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
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetAntiSpamConfiguration().GetGreyListingInitialDelete();
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
      if (!config_)
         return GetAccessDenied();

      config_->GetAntiSpamConfiguration().SetGreyListingInitialDelete(newVal);
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
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetAntiSpamConfiguration().GetGreyListingFinalDelete();
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
      if (!config_)
         return GetAccessDenied();

      config_->GetAntiSpamConfiguration().SetGreyListingFinalDelete(newVal);
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
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetAntiSpamConfiguration().GetAddHeaderSpam() ? VARIANT_TRUE : VARIANT_FALSE;
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
      if (!config_)
         return GetAccessDenied();

      config_->GetAntiSpamConfiguration().SetAddHeaderSpam(newVal == VARIANT_TRUE);
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
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetAntiSpamConfiguration().GetAddHeaderReason() ? VARIANT_TRUE : VARIANT_FALSE;
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
      if (!config_)
         return GetAccessDenied();

      config_->GetAntiSpamConfiguration().SetAddHeaderReason(newVal == VARIANT_TRUE);
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
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetAntiSpamConfiguration().GetPrependSubject() ? VARIANT_TRUE : VARIANT_FALSE;
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
      if (!config_)
         return GetAccessDenied();

      config_->GetAntiSpamConfiguration().SetPrependSubject(newVal == VARIANT_TRUE);
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
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetAntiSpamConfiguration().GetPrependSubjectText().AllocSysString();
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
      if (!config_)
         return GetAccessDenied();

      config_->GetAntiSpamConfiguration().SetPrependSubjectText(newVal);
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
      if (!config_)
         return GetAccessDenied();

      if (!config_)
         return DISP_E_BADINDEX;
   
      CComObject<InterfaceGreyListingWhiteAddresses>* pGLWhiteList = new CComObject<InterfaceGreyListingWhiteAddresses>();
      pGLWhiteList->SetAuthentication(authentication_);
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
      if (!config_)
         return GetAccessDenied();

      if (!config_)
         return DISP_E_BADINDEX;
   
      CComObject<InterfaceWhiteListAddresses>* pGLWhiteList = new CComObject<InterfaceWhiteListAddresses>();
      pGLWhiteList->SetAuthentication(authentication_);
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
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetAntiSpamConfiguration().GetUseSPF() ? VARIANT_TRUE : VARIANT_FALSE;
   
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
      if (!config_)
         return GetAccessDenied();

      config_->GetAntiSpamConfiguration().SetUseSPF(newVal == VARIANT_TRUE ? true : false);
   
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
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetAntiSpamConfiguration().GetUseSPFScore();
   
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
      if (!config_)
         return GetAccessDenied();

      config_->GetAntiSpamConfiguration().SetUseSPFScore(newVal);
   
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
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetAntiSpamConfiguration().GetUseMXChecks() ? VARIANT_TRUE : VARIANT_FALSE;
   
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
      if (!config_)
         return GetAccessDenied();

      config_->GetAntiSpamConfiguration().SetUseMXChecks(newVal == VARIANT_TRUE ? true : false);
   
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
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetAntiSpamConfiguration().GetUseMXChecksScore();
   
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
      if (!config_)
         return GetAccessDenied();

      config_->GetAntiSpamConfiguration().SetUseMXChecksScore(newVal);
   
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
      if (!config_)
         return GetAccessDenied();

      CComObject<InterfaceDNSBlackLists>* pDNSBlackListsInterface = new CComObject<InterfaceDNSBlackLists>();
      pDNSBlackListsInterface->SetAuthentication(authentication_);
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
      if (!config_)
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
      if (!config_)
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
      if (!config_)
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
      if (!config_)
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
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetAntiSpamConfiguration().GetSpamAssassinEnabled() ? VARIANT_TRUE : VARIANT_FALSE;
   
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
      if (!config_)
         return GetAccessDenied();

      config_->GetAntiSpamConfiguration().SetSpamAssassinEnabled(newVal == VARIANT_TRUE ? true : false);
   
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
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetAntiSpamConfiguration().GetSpamAssassinScore();
   
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
      if (!config_)
         return GetAccessDenied();

      config_->GetAntiSpamConfiguration().SetSpamAssassinScore(newVal);
   
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
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetAntiSpamConfiguration().GetSpamAssassinMergeScore() ? VARIANT_TRUE : VARIANT_FALSE;
   
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
      if (!config_)
         return GetAccessDenied();

      config_->GetAntiSpamConfiguration().SetSpamAssassinMergeScore(newVal == VARIANT_TRUE);
   
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
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetAntiSpamConfiguration().GetSpamAssassinHost().AllocSysString();
   
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
      if (!config_)
         return GetAccessDenied();

      config_->GetAntiSpamConfiguration().SetSpamAssassinHost(newVal);
   
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
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetAntiSpamConfiguration().GetSpamAssassinPort();
   
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
      if (!config_)
         return GetAccessDenied();

      config_->GetAntiSpamConfiguration().SetSpamAssassinPort(newVal);
   
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
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetAntiSpamConfiguration().GetAntiSpamMaxSizeKB();
   
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
      if (!config_)
         return GetAccessDenied();

      config_->GetAntiSpamConfiguration().SetAntiSpamMaxSizeKB(newVal);
   
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
      if (!config_)
         return GetAccessDenied();

      config_->GetAntiSpamConfiguration().ClearGreyListingTriplets();
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
      if (!config_)
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
      if (!config_)
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
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetAntiSpamConfiguration().GetDKIMVerificationEnabled() ? VARIANT_TRUE : VARIANT_FALSE;
   
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
      if (!config_)
         return GetAccessDenied();

      config_->GetAntiSpamConfiguration().SetDKIMVerificationEnabled(newVal == VARIANT_TRUE);
   
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
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetAntiSpamConfiguration().GetDKIMVerificationFailureScore();
   
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
      if (!config_)
         return GetAccessDenied();

      config_->GetAntiSpamConfiguration().SetDKIMVerificationFailureScore(newVal);
   
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
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetAntiSpamConfiguration().GetBypassGreyListingOnSPFSuccess() ? VARIANT_TRUE : VARIANT_FALSE;
   
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
      if (!config_)
         return GetAccessDenied();

      config_->GetAntiSpamConfiguration().SetBypassGreyListingOnSPFSuccess(newValue == VARIANT_TRUE);
   
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
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetAntiSpamConfiguration().GetBypassGreyListingOnMailFromMX() ? VARIANT_TRUE : VARIANT_FALSE;
   
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
      if (!config_)
         return GetAccessDenied();

      config_->GetAntiSpamConfiguration().SetBypassGreyListingOnMailFromMX(newValue == VARIANT_TRUE);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


