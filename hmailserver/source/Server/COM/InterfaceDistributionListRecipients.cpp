// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceDistributionListRecipients.h"

#include "InterfaceDistributionListRecipient.h"

#include "../common/BO/DistributionListRecipients.h"
#include "../common/Persistence/PersistentDistributionListRecipient.h"

void
InterfaceDistributionListRecipients::Attach(std::shared_ptr<HM::DistributionListRecipients> pRecipients)
{
   recipients_= pRecipients;
}

STDMETHODIMP InterfaceDistributionListRecipients::get_Count(long *pVal)
{
   try
   {
      if (!recipients_)
         return GetAccessDenied();

   
      *pVal = recipients_->GetCount();
   
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
      if (!recipients_)
         return GetAccessDenied();

      recipients_->DeleteItemByDBID(DBID);
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
      if (!recipients_)
         return GetAccessDenied();

      if (!recipients_)
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceDistributionListRecipient>* pList = new CComObject<InterfaceDistributionListRecipient>();
      pList->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::DistributionListRecipient> pRecipient = std::shared_ptr<HM::DistributionListRecipient>(new HM::DistributionListRecipient);
      
      pRecipient->SetListID(list_id_);
   
      pList->AttachItem(pRecipient);
      pList->AttachParent(recipients_, false);
   
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
      if (!recipients_)
         return GetAccessDenied();

      std::shared_ptr<HM::DistributionListRecipient> pPersList = recipients_->GetItem(Index);
   
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
      if (!recipients_)
         return GetAccessDenied();

      CComObject<InterfaceDistributionListRecipient>* pList = new CComObject<InterfaceDistributionListRecipient>();
      pList->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::DistributionListRecipient> pPersList = recipients_->GetItem(Index);
   
      if (pPersList)
      {
         pList->AttachItem(pPersList);
         pList->AttachParent(recipients_, true);
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
      if (!recipients_)
         return GetAccessDenied();

      recipients_->Refresh();
   
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
      if (!recipients_)
         return GetAccessDenied();

   
   
      CComObject<InterfaceDistributionListRecipient>* pInterfaceRecipient = new CComObject<InterfaceDistributionListRecipient>();
      pInterfaceRecipient->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::DistributionListRecipient> pRecipient = recipients_->GetItemByDBID(DBID);
   
      if (pRecipient)
      {
         pInterfaceRecipient->AttachItem(pRecipient);
         pInterfaceRecipient->AttachParent(recipients_, true);
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


