// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "..\COM\InterfaceResult.h"


// InterfaceResult

void 
InterfaceResult::AttachItem(std::shared_ptr<HM::Result> pResult)
{
   result_ = pResult;
}

STDMETHODIMP InterfaceResult::get_Value(long *pVal)
{
   try
   {
      *pVal = result_->GetValue();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceResult::put_Value(long newVal)
{
   try
   {
      result_->SetValue(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceResult::get_Parameter(long *pVal)
{
   try
   {
      *pVal = result_->GetParameter();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceResult::put_Parameter(long newVal)
{
   try
   {
      result_->SetParameter(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceResult::get_Message(BSTR *pVal)
{
   try
   {
      *pVal = result_->GetMessage().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceResult::put_Message(BSTR newVal)
{
   try
   {
      result_->SetMessage(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


