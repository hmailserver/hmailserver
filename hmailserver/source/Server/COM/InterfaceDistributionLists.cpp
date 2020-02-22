// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceDistributionLists.h"

#include "InterfaceDistributionList.h"

#include "../common/bo/distributionlists.h"
#include "../common/persistence/PersistentDistributionList.h"

void 
InterfaceDistributionLists::Attach(std::shared_ptr<HM::DistributionLists> pDistributionLists)
{
   distribution_lists_= pDistributionLists;
}

STDMETHODIMP
InterfaceDistributionLists::get_Count(long *pVal)
{
   try
   {
      if (!distribution_lists_)
         return GetAccessDenied();

      *pVal = distribution_lists_->GetCount();
   
      return S_OK;
   
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDistributionLists::DeleteByDBID(long DBID)
{
   try
   {
      if (!distribution_lists_)
         return GetAccessDenied();

      distribution_lists_->DeleteItemByDBID(DBID);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDistributionLists::Add(IInterfaceDistributionList **pVal)
{
   try
   {
      if (!distribution_lists_)
         return GetAccessDenied();

      if (!distribution_lists_)
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceDistributionList>* pList = new CComObject<InterfaceDistributionList>();
      pList->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::DistributionList> pPersList = std::shared_ptr<HM::DistributionList>(new HM::DistributionList);
      pPersList->SetDomainID(domain_id_);
   
      pList->AttachItem(pPersList);
      pList->AttachParent(distribution_lists_, false);
      pList->AddRef();
   
      *pVal = pList;
   
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDistributionLists::Delete(long Index)
{
   try
   {
      if (!distribution_lists_)
         return GetAccessDenied();

      std::shared_ptr<HM::DistributionList> pPersList = distribution_lists_->GetItem(Index);
   
      HM::PersistentDistributionList::DeleteObject(pPersList);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDistributionLists::get_Item(long Index, IInterfaceDistributionList **pVal)
{
   try
   {
      if (!distribution_lists_)
         return GetAccessDenied();

      CComObject<InterfaceDistributionList>* pList = new CComObject<InterfaceDistributionList>();
      pList->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::DistributionList> pPersList = distribution_lists_->GetItem(Index);
   
      if (!pPersList)
         return DISP_E_BADINDEX;  
   
      pList->AttachItem(pPersList);
      pList->AttachParent(distribution_lists_, true);
      pList->AddRef();
      *pVal = pList;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDistributionLists::Refresh()
{
   try
   {
      if (!distribution_lists_)
         return GetAccessDenied();

      distribution_lists_->Refresh();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDistributionLists::get_ItemByDBID(long DBID, IInterfaceDistributionList **pVal)
{
   try
   {
      if (!distribution_lists_)
         return GetAccessDenied();

      CComObject<InterfaceDistributionList>* pList = new CComObject<InterfaceDistributionList>();
      pList->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::DistributionList> pPersList = distribution_lists_->GetItemByDBID(DBID);
   
      if (!pPersList)
         return DISP_E_BADINDEX;  
   
      pList->AttachItem(pPersList);
      pList->AttachParent(distribution_lists_, true);
      pList->AddRef();
      *pVal = pList;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDistributionLists::get_ItemByAddress(BSTR sAddress, IInterfaceDistributionList **pVal)
{
   try
   {
      if (!distribution_lists_)
         return GetAccessDenied();

      CComObject<InterfaceDistributionList>* pList = new CComObject<InterfaceDistributionList>();
      pList->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::DistributionList> pPersList = distribution_lists_->GetItemByAddress(sAddress);
   
      if (!pPersList)
         return DISP_E_BADINDEX;  
   
      pList->AttachItem(pPersList);
      pList->AttachParent(distribution_lists_, true);
      pList->AddRef();
      *pVal = pList;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


