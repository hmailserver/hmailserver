
// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include ".\comerror.h"

COMError::COMError(void)
{
}

COMError::~COMError(void)
{
}

HRESULT
COMError::GenerateGenericMessage()
{
   return GenerateError("An error occurred processing the request.");
}

HRESULT
COMError::GenerateError(HM::String sDescription)
{
   if (sDescription == _T(""))
   {
      sDescription = "The operation failed. Please check the hMailServer log for details.";
   }

   // Get the ICreateErrorInfo Interface
   ICreateErrorInfo *pCreateErrorInfo = NULL;
   HRESULT hSuccess = CreateErrorInfo(&pCreateErrorInfo);
   ATLASSERT(SUCCEEDED(hSuccess));
   // pCreateErrorInfo->SetGUID(CLSID_BaseApp);
   pCreateErrorInfo->SetDescription(sDescription.AllocSysString());
   HM::String sSource = "hMailServer COM library";
   pCreateErrorInfo->SetSource(sSource.AllocSysString());

   IErrorInfo *pErrInfo;

   if (SUCCEEDED(pCreateErrorInfo->QueryInterface(IID_IErrorInfo, 
      reinterpret_cast<void**>(&pErrInfo))))
   {
      SetErrorInfo(0, pErrInfo);
      pErrInfo->Release();
   }

   pCreateErrorInfo->Release();

   return MAKE_HRESULT(1, FACILITY_ITF, 1001);
}