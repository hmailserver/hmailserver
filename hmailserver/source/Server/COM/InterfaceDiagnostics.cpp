// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceDiagnostics.h"
#include "InterfaceDiagnosticResults.h"


// InterfaceDiagnostics

STDMETHODIMP InterfaceDiagnostics::PerformTests(IInterfaceDiagnosticResults **pVal)
{
   try
   {
      if (!authentication_)
         return GetAccessDenied();

      if (!authentication_->GetIsServerAdmin())
         return authentication_->GetAccessDenied();
   
      HM::String str;
      
      std::vector<HM::DiagnosticResult> results = diagnostics_.PerformTests();
      
      CComObject<InterfaceDiagnosticResults>* pResult = new CComObject<InterfaceDiagnosticResults>();
      pResult->SetAuthentication(authentication_);
      pResult->AttachResults(results);
      pResult->AddRef();
      *pVal = pResult;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDiagnostics::get_LocalDomainName(BSTR *pVal)
{
   try
   {
      if (!authentication_)
         return GetAccessDenied();

      if (!authentication_->GetIsServerAdmin())
         return authentication_->GetAccessDenied();
   
      *pVal = diagnostics_.GetLocalDomain().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDiagnostics::put_LocalDomainName(BSTR newVal)
{
   try
   {
      if (!authentication_)
         return GetAccessDenied();

      if (!authentication_->GetIsServerAdmin())
         return authentication_->GetAccessDenied();
   
      HM::String localDomainName = newVal;
      diagnostics_.SetLocalDomain(localDomainName);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDiagnostics::get_TestDomainName(BSTR *pVal)
{
   try
   {
      if (!authentication_)
         return GetAccessDenied();

      if (!authentication_->GetIsServerAdmin())
         return authentication_->GetAccessDenied();
   
      *pVal = diagnostics_.GetTestDomain().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDiagnostics::put_TestDomainName(BSTR newVal)
{
   try
   {
      if (!authentication_)
         return GetAccessDenied();

      if (!authentication_->GetIsServerAdmin())
         return authentication_->GetAccessDenied();
   
      HM::String TestDomainName = newVal;
      diagnostics_.SetTestDomain(TestDomainName);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

