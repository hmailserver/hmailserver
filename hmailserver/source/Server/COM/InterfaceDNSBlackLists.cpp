// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceDNSBlackLists.h"
#include "InterfaceDNSBlackList.h"

#include "../SMTP/SMTPConfiguration.h"


bool 
InterfaceDNSBlackLists::LoadSettings()
{
   if (!GetIsServerAdmin())
      return false;

   black_lists_ = HM::Configuration::Instance()->GetAntiSpamConfiguration().GetDNSBlackLists();

   return true;
}


STDMETHODIMP 
InterfaceDNSBlackLists::Refresh()
{
   try
   {
      if (!black_lists_)
         return GetAccessDenied();

      if (!black_lists_)
         return S_FALSE;
   
      black_lists_->Refresh();
      
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDNSBlackLists::get_Count(long *pVal)
{
   try
   {
      if (!black_lists_)
         return GetAccessDenied();

      *pVal = black_lists_->GetCount();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceDNSBlackLists::get_Item(long Index, IInterfaceDNSBlackList **pVal)
{
   try
   {
      if (!black_lists_)
         return GetAccessDenied();

      CComObject<InterfaceDNSBlackList>* pInterfaceDNSBlackList = new CComObject<InterfaceDNSBlackList>();
      pInterfaceDNSBlackList->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::DNSBlackList> pDNSBlackList = black_lists_->GetItem(Index);
   
      if (!pDNSBlackList)
         return DISP_E_BADINDEX;
   
      pInterfaceDNSBlackList->AttachItem(pDNSBlackList);
      pInterfaceDNSBlackList->AttachParent(black_lists_, true);
      pInterfaceDNSBlackList->AddRef();
      *pVal = pInterfaceDNSBlackList;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceDNSBlackLists::DeleteByDBID(long DBID)
{
   try
   {
      if (!black_lists_)
         return GetAccessDenied();

      black_lists_->DeleteItemByDBID(DBID);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceDNSBlackLists::get_ItemByDBID(long lDBID, IInterfaceDNSBlackList **pVal)
{
   try
   {
      if (!black_lists_)
         return GetAccessDenied();

      CComObject<InterfaceDNSBlackList>* pInterfaceDNSBlackList = new CComObject<InterfaceDNSBlackList>();
      pInterfaceDNSBlackList->SetAuthentication(authentication_);
      std::shared_ptr<HM::DNSBlackList> pDNSBlackList = black_lists_->GetItemByDBID(lDBID);
   
      if (!pDNSBlackList)
         return DISP_E_BADINDEX;
   
      pInterfaceDNSBlackList->AttachItem(pDNSBlackList);
      pInterfaceDNSBlackList->AttachParent(black_lists_, true);
      pInterfaceDNSBlackList->AddRef();
   
      *pVal = pInterfaceDNSBlackList;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceDNSBlackLists::Add(IInterfaceDNSBlackList **pVal)
{
   try
   {
      if (!black_lists_)
         return GetAccessDenied();

      if (!black_lists_)
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceDNSBlackList>* pInterfaceDNSBlackList = new CComObject<InterfaceDNSBlackList>();
      pInterfaceDNSBlackList->SetAuthentication(authentication_);
      std::shared_ptr<HM::DNSBlackList> pDNSBL = std::shared_ptr<HM::DNSBlackList>(new HM::DNSBlackList);
   
      pInterfaceDNSBlackList->AttachItem(pDNSBL);
      pInterfaceDNSBlackList->AttachParent(black_lists_, false);
   
      pInterfaceDNSBlackList->AddRef();
   
      *pVal = pInterfaceDNSBlackList;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceDNSBlackLists::get_ItemByDNSHost(BSTR ItemName, IInterfaceDNSBlackList **pVal)
{
   try
   {
      if (!black_lists_)
         return GetAccessDenied();

      CComObject<InterfaceDNSBlackList>* pInterfaceDNSBlackList = new CComObject<InterfaceDNSBlackList>();
      pInterfaceDNSBlackList->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::DNSBlackList> pDNSBL = black_lists_->GetItemByName(ItemName);
      if (!pDNSBL)
         return S_FALSE;
   
      pInterfaceDNSBlackList->AttachItem(pDNSBL);
      pInterfaceDNSBlackList->AttachParent(black_lists_, true);
      pInterfaceDNSBlackList->AddRef();
   
      *pVal = pInterfaceDNSBlackList;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


