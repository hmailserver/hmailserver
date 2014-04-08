// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceDistributionListRecipients.h"

#include "InterfaceDistributionListRecipient.h"

#include "../common/BO/DistributionListRecipients.h"
#include "../common/Persistence/PersistentDistributionListRecipient.h"

void
InterfaceDistributionListRecipients::Attach(shared_ptr<HM::DistributionListRecipients> pRecipients)
{
   m_pRecipients= pRecipients;
}

STDMETHODIMP InterfaceDistributionListRecipients::get_Count(long *pVal)
{
   try
   {
      if (!m_pRecipients)
         return GetAccessDenied();

   
      *pVal = m_pRecipients->GetCount();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDistributionListRecipients::DeleteByDBID(long DBID)
{
   try
   {
      if (!m_pRecipients)
         return GetAccessDenied();

      m_pRecipients->DeleteItemByDBID(DBID);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDistributionListRecipients::Add(IInterfaceDistributionListRecipient **pVal)
{
   try
   {
      if (!m_pRecipients)
         return GetAccessDenied();

      if (!m_pRecipients)
         return m_pAuthentication->GetAccessDenied();
   
      CComObject<InterfaceDistributionListRecipient>* pList = new CComObject<InterfaceDistributionListRecipient>();
      pList->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::DistributionListRecipient> pRecipient = shared_ptr<HM::DistributionListRecipient>(new HM::DistributionListRecipient);
      
      pRecipient->SetListID(m_lListID);
   
      pList->AttachItem(pRecipient);
      pList->AttachParent(m_pRecipients, false);
   
      pList->AddRef();
      *pVal = pList;
   
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDistributionListRecipients::Delete(long Index)
{
   try
   {
      if (!m_pRecipients)
         return GetAccessDenied();

      shared_ptr<HM::DistributionListRecipient> pPersList = m_pRecipients->GetItem(Index);
   
      HM::PersistentDistributionListRecipient::DeleteObject(pPersList);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDistributionListRecipients::get_Item(long Index, IInterfaceDistributionListRecipient **pVal)
{
   try
   {
      if (!m_pRecipients)
         return GetAccessDenied();

      CComObject<InterfaceDistributionListRecipient>* pList = new CComObject<InterfaceDistributionListRecipient>();
      pList->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::DistributionListRecipient> pPersList = m_pRecipients->GetItem(Index);
   
      if (pPersList)
      {
         pList->AttachItem(pPersList);
         pList->AttachParent(m_pRecipients, true);
         pList->AddRef();
         *pVal = pList;
      }
      else
      {
         return DISP_E_BADINDEX;  
      }
   
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDistributionListRecipients::Refresh()
{
   try
   {
      if (!m_pRecipients)
         return GetAccessDenied();

      m_pRecipients->Refresh();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDistributionListRecipients::get_ItemByDBID(long DBID, IInterfaceDistributionListRecipient **pVal)
{
   try
   {
      if (!m_pRecipients)
         return GetAccessDenied();

   
   
      CComObject<InterfaceDistributionListRecipient>* pInterfaceRecipient = new CComObject<InterfaceDistributionListRecipient>();
      pInterfaceRecipient->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::DistributionListRecipient> pRecipient = m_pRecipients->GetItemByDBID(DBID);
   
      if (pRecipient)
      {
         pInterfaceRecipient->AttachItem(pRecipient);
         pInterfaceRecipient->AttachParent(m_pRecipients, true);
         pInterfaceRecipient->AddRef();
         *pVal = pInterfaceRecipient;
      }
      else
      {
         return DISP_E_BADINDEX;  
      }
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


