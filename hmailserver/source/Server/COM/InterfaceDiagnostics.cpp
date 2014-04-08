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
      if (!m_pAuthentication)
         return GetAccessDenied();

      if (!m_pAuthentication->GetIsServerAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      HM::String str;
      
      vector<HM::DiagnosticResult> results = _diagnostics.PerformTests();
      
      CComObject<InterfaceDiagnosticResults>* pResult = new CComObject<InterfaceDiagnosticResults>();
      pResult->SetAuthentication(m_pAuthentication);
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
      if (!m_pAuthentication)
         return GetAccessDenied();

      if (!m_pAuthentication->GetIsServerAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      *pVal = _diagnostics.GetLocalDomain().AllocSysString();
   
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
      if (!m_pAuthentication)
         return GetAccessDenied();

      if (!m_pAuthentication->GetIsServerAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      HM::String localDomainName = newVal;
      _diagnostics.SetLocalDomain(localDomainName);
   
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
      if (!m_pAuthentication)
         return GetAccessDenied();

      if (!m_pAuthentication->GetIsServerAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      *pVal = _diagnostics.GetTestDomain().AllocSysString();
   
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
      if (!m_pAuthentication)
         return GetAccessDenied();

      if (!m_pAuthentication->GetIsServerAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      HM::String TestDomainName = newVal;
      _diagnostics.SetTestDomain(TestDomainName);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

