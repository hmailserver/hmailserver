// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceMessageHeaders.h"
#include "InterfaceMessageHeader.h"

#include "../Common/Mime/Mime.h"

void 
InterfaceMessageHeaders::AttachItem(shared_ptr<HM::MimeHeader> pHeader)
{
   m_pHeader = pHeader;
}

STDMETHODIMP InterfaceMessageHeaders::get_Count(long *pVal)
{
   try
   {
      if (!m_pHeader)
         return DISP_E_BADINDEX;
   
      *pVal = m_pHeader->GetFieldCount();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceMessageHeaders::get_Item(long Index, IInterfaceMessageHeader **pVal)
{
   try
   {
      if (!m_pHeader)
         return DISP_E_BADINDEX;
   
      HM::MimeField *pField = m_pHeader->GetField(Index);
   
      if (!pField)
         return DISP_E_BADINDEX;
   
      CComObject<InterfaceMessageHeader>* pInterfaceMessageHeader = new CComObject<InterfaceMessageHeader>();
      pInterfaceMessageHeader->AttachItem(m_pHeader, pField);
      
      pInterfaceMessageHeader->AddRef();
   
      *pVal = pInterfaceMessageHeader;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceMessageHeaders::get_ItemByName(BSTR sName, IInterfaceMessageHeader **pVal)
{
   try
   {
      if (!m_pHeader)
         return DISP_E_BADINDEX;
   
      HM::AnsiString sFieldName = sName;
      HM::MimeField *pField = m_pHeader->GetField(sFieldName);
   
      if (!pField)
         return DISP_E_BADINDEX;
   
      CComObject<InterfaceMessageHeader>* pInterfaceMessageHeader = new CComObject<InterfaceMessageHeader>();
      pInterfaceMessageHeader->AttachItem(m_pHeader, pField);
      pInterfaceMessageHeader->AddRef();
   
      *pVal = pInterfaceMessageHeader;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


