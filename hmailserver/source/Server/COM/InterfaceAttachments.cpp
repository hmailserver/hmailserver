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
InterfaceAttachments::Attach(std::shared_ptr<HM::Attachments> pAttachments)
{
   attachments_ = pAttachments;
}

STDMETHODIMP InterfaceAttachments::get_Count(long *pVal)
{
   try
   {
      if (!attachments_)
         return GetAccessDenied();

      *pVal = (int) attachments_->GetCount();
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
      if (!attachments_)
         return GetAccessDenied();

      CComObject<InterfaceAttachment>* pInterfaceAttachment = new CComObject<InterfaceAttachment>();
      pInterfaceAttachment->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::Attachment> pAttachment = attachments_->GetItem(Index);
   
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
      if (!attachments_)
         return GetAccessDenied();

      attachments_->Clear();
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
      if (!attachments_)
         return GetAccessDenied();

      if (!attachments_->Add(sFilename))
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

