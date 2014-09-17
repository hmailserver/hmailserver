// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceMessageHeaders.h"
#include "InterfaceMessageHeader.h"

#include "../Common/Mime/Mime.h"

void 
InterfaceMessageHeaders::AttachItem(std::shared_ptr<HM::MimeHeader> pHeader)
{
   header_ = pHeader;
}

STDMETHODIMP InterfaceMessageHeaders::get_Count(long *pVal)
{
   try
   {
      if (!header_)
         return DISP_E_BADINDEX;
   
      *pVal = header_->GetFieldCount();
   
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
      if (!header_)
         return DISP_E_BADINDEX;
   
      HM::MimeField *pField = header_->GetField(Index);
   
      if (!pField)
         return DISP_E_BADINDEX;
   
      CComObject<InterfaceMessageHeader>* pInterfaceMessageHeader = new CComObject<InterfaceMessageHeader>();
      pInterfaceMessageHeader->AttachItem(header_, pField);
      
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
      if (!header_)
         return DISP_E_BADINDEX;
   
      HM::AnsiString sFieldName = sName;
      HM::MimeField *pField = header_->GetField(sFieldName);
   
      if (!pField)
         return DISP_E_BADINDEX;
   
      CComObject<InterfaceMessageHeader>* pInterfaceMessageHeader = new CComObject<InterfaceMessageHeader>();
      pInterfaceMessageHeader->AttachItem(header_, pField);
      pInterfaceMessageHeader->AddRef();
   
      *pVal = pInterfaceMessageHeader;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


