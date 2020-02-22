// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "..\COM\InterfaceBlockedAttachments.h"

#include "..\Common\BO\BlockedAttachment.h"
#include "InterfaceBlockedAttachment.h"

void 
InterfaceBlockedAttachments::Attach(std::shared_ptr<HM::BlockedAttachments> pBA) 
{ 
   blocked_attachments_ = pBA; 
}

STDMETHODIMP 
InterfaceBlockedAttachments::Refresh()
{
   try
   {
      if (!blocked_attachments_)
         return S_FALSE;
   
      blocked_attachments_->Refresh();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceBlockedAttachments::get_Count(long *pVal)
{
   try
   {
      *pVal = blocked_attachments_->GetCount();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceBlockedAttachments::get_Item(long Index, IInterfaceBlockedAttachment **pVal)
{
   try
   {
      CComObject<InterfaceBlockedAttachment>* pInterfaceBlockedAttachment = new CComObject<InterfaceBlockedAttachment>();
      pInterfaceBlockedAttachment->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::BlockedAttachment> pBA = blocked_attachments_->GetItem(Index);
   
      if (!pBA)
         return DISP_E_BADINDEX;
   
      pInterfaceBlockedAttachment->AttachItem(pBA);
      pInterfaceBlockedAttachment->AttachParent(blocked_attachments_, true);
      pInterfaceBlockedAttachment->AddRef();
      *pVal = pInterfaceBlockedAttachment;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceBlockedAttachments::DeleteByDBID(long DBID)
{
   try
   {
      if (!blocked_attachments_)
         return GetAccessDenied();

      blocked_attachments_->DeleteItemByDBID(DBID);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceBlockedAttachments::get_ItemByDBID(long lDBID, IInterfaceBlockedAttachment **pVal)
{
   try
   {
      if (!blocked_attachments_)
         return GetAccessDenied();

      CComObject<InterfaceBlockedAttachment>* pInterfaceBlockedAttachment = new CComObject<InterfaceBlockedAttachment>();
      pInterfaceBlockedAttachment->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::BlockedAttachment> pBA = blocked_attachments_->GetItemByDBID(lDBID);
   
      if (!pBA)
         return DISP_E_BADINDEX;
   
      pInterfaceBlockedAttachment->AttachItem(pBA);
      pInterfaceBlockedAttachment->AttachParent(blocked_attachments_, true);
      pInterfaceBlockedAttachment->AddRef();
   
      *pVal = pInterfaceBlockedAttachment;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceBlockedAttachments::Add(IInterfaceBlockedAttachment **pVal)
{
   try
   {
      if (!blocked_attachments_)
         return GetAccessDenied();

      if (!blocked_attachments_)
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceBlockedAttachment>* pInterfaceBlockedAttachment = new CComObject<InterfaceBlockedAttachment>();
      pInterfaceBlockedAttachment->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::BlockedAttachment> pBA = std::shared_ptr<HM::BlockedAttachment>(new HM::BlockedAttachment);
   
      pInterfaceBlockedAttachment->AttachItem(pBA);
      pInterfaceBlockedAttachment->AttachParent(blocked_attachments_, false);
   
      pInterfaceBlockedAttachment->AddRef();
   
      *pVal = pInterfaceBlockedAttachment;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


