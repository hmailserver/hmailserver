// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceAccounts.h"

#include "InterfaceAccount.h"
#include "../common/Persistence/PersistentAccount.h"
#include "../common/BO/Accounts.h"
#include "../common/BO/Account.h"

#include "COMAuthentication.h"

#ifdef _DEBUG
   long InterfaceAccounts::counter = 0;
#endif

void
InterfaceAccounts::Attach(std::shared_ptr<HM::Accounts> pAccounts)
{
   accounts_ = pAccounts;
}


STDMETHODIMP InterfaceAccounts::get_Count(long *pVal)
{
   try
   {
      if (!accounts_)
         return GetAccessDenied();

      *pVal = (int) accounts_->GetCount();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccounts::Add(IInterfaceAccount **pVal)
{
   try
   {
      if (!accounts_)
         return GetAccessDenied();

      if (!authentication_->GetIsDomainAdmin())
         return authentication_->GetAccessDenied();
   
      if (!accounts_)
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceAccount>* pAccountInterface = new CComObject<InterfaceAccount>();
      pAccountInterface->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::Account> pAccount = std::shared_ptr<HM::Account>(new HM::Account);
   
      pAccount->SetDomainID(domain_id_);
      
      pAccountInterface->AttachItem(pAccount);
      pAccountInterface->AttachParent(accounts_, false);
      pAccountInterface->SetAuthentication(authentication_);
      pAccountInterface->AddRef();
   
      *pVal = pAccountInterface;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccounts::Delete(long Index)
{
   try
   {
      if (!accounts_)
         return GetAccessDenied();

      if (!authentication_->GetIsDomainAdmin())
         return authentication_->GetAccessDenied();
   
      std::shared_ptr<HM::Account> pAccount = accounts_->GetItem(Index);
      HM::PersistentAccount::DeleteObject(pAccount);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccounts::get_Item(long Index, IInterfaceAccount **pVal)
{
   try
   {
      if (!accounts_)
         return GetAccessDenied();

      CComObject<InterfaceAccount>* pAccountInt = new CComObject<InterfaceAccount>();
      pAccountInt->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::Account> pAccount = accounts_->GetItem(Index);
   
      if (!pAccount)
         return DISP_E_BADINDEX;
      
      pAccountInt->AttachItem(pAccount);
      pAccountInt->AttachParent(accounts_, true);
      pAccountInt->SetAuthentication(authentication_);
      pAccountInt->AddRef();
      *pVal = pAccountInt;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccounts::Refresh()
{
   try
   {
      if (!accounts_)
         return GetAccessDenied();

      accounts_->Refresh();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccounts::get_ItemByDBID(long DBID, IInterfaceAccount **pVal)
{
   try
   {
      if (!accounts_)
         return GetAccessDenied();

      CComObject<InterfaceAccount>* pAccountInt = new CComObject<InterfaceAccount>();
      pAccountInt->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::Account> pAccount = accounts_->GetItemByDBID(DBID);
   
      if (!pAccount)
         return DISP_E_BADINDEX;  
   
      pAccountInt->AttachItem(pAccount);
      pAccountInt->SetAuthentication(authentication_);
      pAccountInt->AttachParent(accounts_, true);
      pAccountInt->AddRef();
      *pVal = pAccountInt;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccounts::get_ItemByAddress(BSTR Address, IInterfaceAccount **pVal)
{
   try
   {
      if (!accounts_)
         return GetAccessDenied();

   
      CComObject<InterfaceAccount>* pAccountInt = new CComObject<InterfaceAccount>();
      pAccountInt->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::Account> pAccount = accounts_->GetItemByName(Address);
   
      if (!pAccount)
         return DISP_E_BADINDEX; 
   
      pAccountInt->AttachItem(pAccount);
      pAccountInt->SetAuthentication(authentication_);
      pAccountInt->AttachParent(accounts_, true);
      pAccountInt->AddRef();
      *pVal = pAccountInt;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccounts::DeleteByDBID(long DBID)
{
   try
   {
      if (!accounts_)
         return GetAccessDenied();

      if (!authentication_->GetIsDomainAdmin())
         return authentication_->GetAccessDenied();
   
      accounts_->DeleteItemByDBID(DBID);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


