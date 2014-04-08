// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "..\COM\InterfaceResult.h"


// InterfaceResult

void 
InterfaceResult::AttachItem(shared_ptr<HM::Result> pResult)
{
   m_pResult = pResult;
}

STDMETHODIMP InterfaceResult::get_Value(long *pVal)
{
   try
   {
      *pVal = m_pResult->GetValue();
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
      m_pResult->SetValue(newVal);
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
      *pVal = m_pResult->GetParameter();
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
      m_pResult->SetParameter(newVal);
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
      *pVal = m_pResult->GetMessage().AllocSysString();
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
      m_pResult->SetMessage(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


