// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceDiagnosticResults.h"
#include "InterfaceDiagnosticResult.h"



STDMETHODIMP InterfaceDiagnosticResults::get_Count(long* count)
{
   try
   {
      if (!authentication_)
         return GetAccessDenied();

      if (!authentication_->GetIsServerAdmin())
         return authentication_->GetAccessDenied();
   
      *count =  (int) _results.size();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDiagnosticResults::get_Item(long Index, IInterfaceDiagnosticResult* *pVal)
{
   try
   {
      if (!authentication_)
         return GetAccessDenied();

      if (!authentication_->GetIsServerAdmin())
         return authentication_->GetAccessDenied();
   
      if (Index >= (long) _results.size())
         return DISP_E_BADINDEX;
   
      CComObject<InterfaceDiagnosticResult>* pResult = new CComObject<InterfaceDiagnosticResult>();
      pResult->SetAuthentication(authentication_);
      pResult->AttachResult(_results[Index]);
      pResult->AddRef();
      *pVal = pResult;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


