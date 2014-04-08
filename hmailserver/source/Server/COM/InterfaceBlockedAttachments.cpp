// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "..\COM\InterfaceBlockedAttachments.h"

#include "..\Common\BO\BlockedAttachment.h"
#include "InterfaceBlockedAttachment.h"

void 
InterfaceBlockedAttachments::Attach(shared_ptr<HM::BlockedAttachments> pBA) 
{ 
   m_pBlockedAttachments = pBA; 
}

STDMETHODIMP 
InterfaceBlockedAttachments::Refresh()
{
   try
   {
      if (!m_pBlockedAttachments)
         return S_FALSE;
   
      m_pBlockedAttachments->Refresh();
   
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
      *pVal = m_pBlockedAttachments->GetCount();
   
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
      pInterfaceBlockedAttachment->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::BlockedAttachment> pBA = m_pBlockedAttachments->GetItem(Index);
   
      if (!pBA)
         return DISP_E_BADINDEX;
   
      pInterfaceBlockedAttachment->AttachItem(pBA);
      pInterfaceBlockedAttachment->AttachParent(m_pBlockedAttachments, true);
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
      if (!m_pBlockedAttachments)
         return GetAccessDenied();

      m_pBlockedAttachments->DeleteItemByDBID(DBID);
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
      if (!m_pBlockedAttachments)
         return GetAccessDenied();

      CComObject<InterfaceBlockedAttachment>* pInterfaceBlockedAttachment = new CComObject<InterfaceBlockedAttachment>();
      pInterfaceBlockedAttachment->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::BlockedAttachment> pBA = m_pBlockedAttachments->GetItemByDBID(lDBID);
   
      if (!pBA)
         return DISP_E_BADINDEX;
   
      pInterfaceBlockedAttachment->AttachItem(pBA);
      pInterfaceBlockedAttachment->AttachParent(m_pBlockedAttachments, true);
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
      if (!m_pBlockedAttachments)
         return GetAccessDenied();

      if (!m_pBlockedAttachments)
         return m_pAuthentication->GetAccessDenied();
   
      CComObject<InterfaceBlockedAttachment>* pInterfaceBlockedAttachment = new CComObject<InterfaceBlockedAttachment>();
      pInterfaceBlockedAttachment->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::BlockedAttachment> pBA = shared_ptr<HM::BlockedAttachment>(new HM::BlockedAttachment);
   
      pInterfaceBlockedAttachment->AttachItem(pBA);
      pInterfaceBlockedAttachment->AttachParent(m_pBlockedAttachments, false);
   
      pInterfaceBlockedAttachment->AddRef();
   
      *pVal = pInterfaceBlockedAttachment;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


