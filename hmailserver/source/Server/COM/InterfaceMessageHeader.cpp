// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceMessageHeader.h"

#include "../Common/Mime/Mime.h"

void 
InterfaceMessageHeader::AttachItem (shared_ptr<HM::MimeHeader> pHeader, HM::MimeField *pField)
{
   m_pHeader = pHeader;
   m_pObject = pField;
}


STDMETHODIMP 
InterfaceMessageHeader::Delete()
{
   try
   {
      m_pHeader->DeleteField(m_pObject);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessageHeader::put_Name(BSTR newVal)
{
   try
   {
      HM::AnsiString sName = newVal;
      m_pObject->SetName(sName);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessageHeader::get_Name(BSTR *pVal)
{
   try
   {
      HM::String sName = m_pObject->GetName();
      *pVal = sName.AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessageHeader::put_Value(BSTR newVal)
{
   try
   {
      HM::AnsiString sValue = newVal;
      m_pObject->SetValue(sValue);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessageHeader::get_Value(BSTR *pVal)
{
   try
   {
      HM::String sValue = m_pObject->GetValue();
      *pVal = sValue.AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


