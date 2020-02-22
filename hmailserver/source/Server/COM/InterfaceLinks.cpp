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
      if (!authentication_->GetIsAuthenticated())
         return authentication_->GetAccessDenied();
   
      bool domainAdminOK = authentication_->GetIsDomainAdmin() && authentication_->GetDomainID() == DBID;
      
      if (!authentication_->GetIsServerAdmin() && !domainAdminOK)
         return authentication_->GetAccessDenied();
   
      std::shared_ptr<HM::Domain> pDomain = std::shared_ptr<HM::Domain>(new HM::Domain);
      if (!HM::PersistentDomain::ReadObject(pDomain, DBID))
         return DISP_E_BADINDEX;
   
      CComObject<InterfaceDomain>* pInterface = new CComObject<InterfaceDomain>();
      pInterface->SetAuthentication(authentication_);
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
      if (!authentication_->GetIsAuthenticated())
         return authentication_->GetAccessDenied();
   
      std::shared_ptr<HM::Account> pAccount = std::shared_ptr<HM::Account>(new HM::Account);
      if (!HM::PersistentAccount::ReadObject(pAccount, DBID))
         return DISP_E_BADINDEX;
   
      bool accountAdminOK =! authentication_->GetIsDomainAdmin() && authentication_->GetAccountID() == pAccount->GetID();
      bool domainAdminOK = authentication_->GetIsDomainAdmin() && authentication_->GetDomainID() == pAccount->GetDomainID();
   
      if (!authentication_->GetIsServerAdmin() && !domainAdminOK && !accountAdminOK)
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceAccount>* pInterface = new CComObject<InterfaceAccount>();
      pInterface->SetAuthentication(authentication_);
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
      if (!authentication_->GetIsAuthenticated())
         return authentication_->GetAccessDenied();
   
      std::shared_ptr<HM::Alias> pAlias = std::shared_ptr<HM::Alias>(new HM::Alias);
      if (!HM::PersistentAlias::ReadObject(pAlias, DBID))
         return DISP_E_BADINDEX;
   
      bool domainAdminOK = authentication_->GetIsDomainAdmin() && authentication_->GetDomainID() == pAlias->GetDomainID();
   
      if (!authentication_->GetIsServerAdmin() && !domainAdminOK)
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceAlias>* pInterface = new CComObject<InterfaceAlias>();
      pInterface->SetAuthentication(authentication_);
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
      if (!authentication_->GetIsAuthenticated())
         return authentication_->GetAccessDenied();
   
      std::shared_ptr<HM::DistributionList> pList = std::shared_ptr<HM::DistributionList>(new HM::DistributionList);
      if (!HM::PersistentDistributionList::ReadObject(pList, DBID))
         return DISP_E_BADINDEX;
   
      bool domainAdminOK = authentication_->GetIsDomainAdmin() && authentication_->GetDomainID() == pList->GetDomainID();
   
      if (!authentication_->GetIsServerAdmin() && !domainAdminOK)
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceDistributionList>* pInterface = new CComObject<InterfaceDistributionList>();
      pInterface->SetAuthentication(authentication_);
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


