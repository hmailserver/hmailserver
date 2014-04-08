// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"

#include "InterfaceLinks.h"
#include "InterfaceDomain.h"
#include "InterfaceAccount.h"
#include "InterfaceAlias.h"
#include "InterfaceDistributionList.h"

#include "../Common/BO/Domain.h"
#include "../Common/BO/Account.h"
#include "../Common/BO/Alias.h"
#include "../Common/BO/DistributionList.h"
#include "../Common/Persistence/PersistentDomain.h"
#include "../Common/Persistence/PersistentAccount.h"
#include "../Common/Persistence/PersistentAlias.h"
#include "../Common/Persistence/PersistentDistributionList.h"

STDMETHODIMP InterfaceLinks::get_Domain(long DBID, IInterfaceDomain **pVal)
{
   try
   {
      if (!m_pAuthentication->GetIsAuthenticated())
         return m_pAuthentication->GetAccessDenied();
   
      bool domainAdminOK = m_pAuthentication->GetIsDomainAdmin() && m_pAuthentication->GetDomainID() == DBID;
      
      if (!m_pAuthentication->GetIsServerAdmin() && !domainAdminOK)
         return m_pAuthentication->GetAccessDenied();
   
      shared_ptr<HM::Domain> pDomain = shared_ptr<HM::Domain>(new HM::Domain);
      if (!HM::PersistentDomain::ReadObject(pDomain, DBID))
         return DISP_E_BADINDEX;
   
      CComObject<InterfaceDomain>* pInterface = new CComObject<InterfaceDomain>();
      pInterface->SetAuthentication(m_pAuthentication);
      pInterface->AttachItem(pDomain);
      pInterface->AddRef();
      *pVal = pInterface;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceLinks::get_Account(long DBID, IInterfaceAccount **pVal)
{
   try
   {
      if (!m_pAuthentication->GetIsAuthenticated())
         return m_pAuthentication->GetAccessDenied();
   
      shared_ptr<HM::Account> pAccount = shared_ptr<HM::Account>(new HM::Account);
      if (!HM::PersistentAccount::ReadObject(pAccount, DBID))
         return DISP_E_BADINDEX;
   
      bool accountAdminOK =! m_pAuthentication->GetIsDomainAdmin() && m_pAuthentication->GetAccountID() == pAccount->GetID();
      bool domainAdminOK = m_pAuthentication->GetIsDomainAdmin() && m_pAuthentication->GetDomainID() == pAccount->GetDomainID();
   
      if (!m_pAuthentication->GetIsServerAdmin() && !domainAdminOK && !accountAdminOK)
         return m_pAuthentication->GetAccessDenied();
   
      CComObject<InterfaceAccount>* pInterface = new CComObject<InterfaceAccount>();
      pInterface->SetAuthentication(m_pAuthentication);
      pInterface->AttachItem(pAccount);
      pInterface->AddRef();
      *pVal = pInterface;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceLinks::get_Alias(long DBID, IInterfaceAlias **pVal)
{
   try
   {
      if (!m_pAuthentication->GetIsAuthenticated())
         return m_pAuthentication->GetAccessDenied();
   
      shared_ptr<HM::Alias> pAlias = shared_ptr<HM::Alias>(new HM::Alias);
      if (!HM::PersistentAlias::ReadObject(pAlias, DBID))
         return DISP_E_BADINDEX;
   
      bool domainAdminOK = m_pAuthentication->GetIsDomainAdmin() && m_pAuthentication->GetDomainID() == pAlias->GetDomainID();
   
      if (!m_pAuthentication->GetIsServerAdmin() && !domainAdminOK)
         return m_pAuthentication->GetAccessDenied();
   
      CComObject<InterfaceAlias>* pInterface = new CComObject<InterfaceAlias>();
      pInterface->SetAuthentication(m_pAuthentication);
      pInterface->AttachItem(pAlias);
      pInterface->AddRef();
      *pVal = pInterface;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceLinks::get_DistributionList(long DBID, IInterfaceDistributionList **pVal)
{
   try
   {
      if (!m_pAuthentication->GetIsAuthenticated())
         return m_pAuthentication->GetAccessDenied();
   
      shared_ptr<HM::DistributionList> pList = shared_ptr<HM::DistributionList>(new HM::DistributionList);
      if (!HM::PersistentDistributionList::ReadObject(pList, DBID))
         return DISP_E_BADINDEX;
   
      bool domainAdminOK = m_pAuthentication->GetIsDomainAdmin() && m_pAuthentication->GetDomainID() == pList->GetDomainID();
   
      if (!m_pAuthentication->GetIsServerAdmin() && !domainAdminOK)
         return m_pAuthentication->GetAccessDenied();
   
      CComObject<InterfaceDistributionList>* pInterface = new CComObject<InterfaceDistributionList>();
      pInterface->SetAuthentication(m_pAuthentication);
      pInterface->AttachItem(pList);
      pInterface->AddRef();
    
      *pVal = pInterface;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


