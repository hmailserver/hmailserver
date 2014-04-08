// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "InterfaceAttachments.h"
#include "InterfaceAttachment.h"

#include "COMError.h"

STDMETHODIMP InterfaceAttachments::InterfaceSupportsErrorInfo(REFIID riid)
{
   try
   {
      static const IID* arr[] = 
      {
         &IID_IInterfaceAttachments,
      };
   
      for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
      {
         if (InlineIsEqualGUID(*arr[i],riid))
            return S_OK;
      }
      return S_FALSE;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }

}   
   
void
InterfaceAttachments::Attach(shared_ptr<HM::Attachments> pAttachments)
{
   m_pAttachments = pAttachments;
}

STDMETHODIMP InterfaceAttachments::get_Count(long *pVal)
{
   try
   {
      if (!m_pAttachments)
         return GetAccessDenied();

      *pVal = (int) m_pAttachments->GetCount();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAttachments::get_Item(long Index, IInterfaceAttachment **pVal)
{
   try
   {
      if (!m_pAttachments)
         return GetAccessDenied();

      CComObject<InterfaceAttachment>* pInterfaceAttachment = new CComObject<InterfaceAttachment>();
      pInterfaceAttachment->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::Attachment> pAttachment = m_pAttachments->GetItem(Index);
   
      if (!pAttachment)
         return DISP_E_BADINDEX;  
   
      pInterfaceAttachment->Attach(pAttachment);
      pInterfaceAttachment->AddRef();
      *pVal = pInterfaceAttachment;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceAttachments::Clear()
{
   try
   {
      if (!m_pAttachments)
         return GetAccessDenied();

      m_pAttachments->Clear();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceAttachments::Add(BSTR sFilename)
{
   try
   {
      if (!m_pAttachments)
         return GetAccessDenied();

      if (!m_pAttachments->Add(sFilename))
      {
         return COMError::GenerateError("Failed to attach file.");
      }
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

