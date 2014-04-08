// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceDiagnosticResult.h"

STDMETHODIMP InterfaceDiagnosticResult::get_Name(BSTR *pVal)
{
   try
   {
      if (!m_pAuthentication->GetIsServerAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      *pVal = _result.GetName().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDiagnosticResult::get_Description(BSTR *pVal)
{
   try
   {
      if (!m_pAuthentication->GetIsServerAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      *pVal = _result.GetDescription().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDiagnosticResult::get_ExecutionDetails(BSTR *pVal)
{
   try
   {
      if (!m_pAuthentication->GetIsServerAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      *pVal = _result.GetDetails().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDiagnosticResult::get_Result(VARIANT_BOOL *pVal)
{
   try
   {
      if (!m_pAuthentication->GetIsServerAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      *pVal = _result.GetSuccess() ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


