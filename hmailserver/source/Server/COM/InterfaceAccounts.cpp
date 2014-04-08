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
InterfaceAccounts::Attach(shared_ptr<HM::Accounts> pAccounts)
{
   m_pAccounts = pAccounts;
}


STDMETHODIMP InterfaceAccounts::get_Count(long *pVal)
{
   try
   {
      if (!m_pAccounts)
         return GetAccessDenied();

      *pVal = (int) m_pAccounts->GetCount();
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
      if (!m_pAccounts)
         return GetAccessDenied();

      if (!m_pAuthentication->GetIsDomainAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      if (!m_pAccounts)
         return m_pAuthentication->GetAccessDenied();
   
      CComObject<InterfaceAccount>* pAccountInterface = new CComObject<InterfaceAccount>();
      pAccountInterface->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::Account> pAccount = shared_ptr<HM::Account>(new HM::Account);
   
      pAccount->SetDomainID(m_iDomainID);
      
      pAccountInterface->AttachItem(pAccount);
      pAccountInterface->AttachParent(m_pAccounts, false);
      pAccountInterface->SetAuthentication(m_pAuthentication);
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
      if (!m_pAccounts)
         return GetAccessDenied();

      if (!m_pAuthentication->GetIsDomainAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      shared_ptr<HM::Account> pAccount = m_pAccounts->GetItem(Index);
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
      if (!m_pAccounts)
         return GetAccessDenied();

      CComObject<InterfaceAccount>* pAccountInt = new CComObject<InterfaceAccount>();
      pAccountInt->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::Account> pAccount = m_pAccounts->GetItem(Index);
   
      if (!pAccount)
         return DISP_E_BADINDEX;
      
      pAccountInt->AttachItem(pAccount);
      pAccountInt->AttachParent(m_pAccounts, true);
      pAccountInt->SetAuthentication(m_pAuthentication);
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
      if (!m_pAccounts)
         return GetAccessDenied();

      m_pAccounts->Refresh();
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
      if (!m_pAccounts)
         return GetAccessDenied();

      CComObject<InterfaceAccount>* pAccountInt = new CComObject<InterfaceAccount>();
      pAccountInt->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::Account> pAccount = m_pAccounts->GetItemByDBID(DBID);
   
      if (!pAccount)
         return DISP_E_BADINDEX;  
   
      pAccountInt->AttachItem(pAccount);
      pAccountInt->SetAuthentication(m_pAuthentication);
      pAccountInt->AttachParent(m_pAccounts, true);
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
      if (!m_pAccounts)
         return GetAccessDenied();

   
      CComObject<InterfaceAccount>* pAccountInt = new CComObject<InterfaceAccount>();
      pAccountInt->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::Account> pAccount = m_pAccounts->GetItemByName(Address);
   
      if (!pAccount)
         return DISP_E_BADINDEX; 
   
      pAccountInt->AttachItem(pAccount);
      pAccountInt->SetAuthentication(m_pAuthentication);
      pAccountInt->AttachParent(m_pAccounts, true);
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
      if (!m_pAccounts)
         return GetAccessDenied();

      if (!m_pAuthentication->GetIsDomainAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      m_pAccounts->DeleteItemByDBID(DBID);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


