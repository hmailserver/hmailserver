// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceRuleCriterias.h"

#include "InterfaceRuleCriteria.h"

#include "../Common/BO/RuleCriterias.h"
#include "../Common/BO/RuleCriteria.h"

void 
InterfaceRuleCriterias::Attach(std::shared_ptr<HM::RuleCriterias> pCriterias)
{
   rule_criterias = pCriterias;
}

STDMETHODIMP InterfaceRuleCriterias::get_ItemByDBID(long lDBID, IInterfaceRuleCriteria** pVal)
{
   try
   {
      if (!rule_criterias)
         return GetAccessDenied();

      CComObject<InterfaceRuleCriteria>* pInterfaceRuleCriteria = new CComObject<InterfaceRuleCriteria>();
      pInterfaceRuleCriteria->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::RuleCriteria> pRule = rule_criterias->GetItemByDBID(lDBID);
      if (!pRule)
         return DISP_E_BADINDEX;
   
      pInterfaceRuleCriteria->AttachItem(pRule);
      pInterfaceRuleCriteria->AttachParent(rule_criterias, true);
      pInterfaceRuleCriteria->AddRef();
      *pVal = pInterfaceRuleCriteria;   
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleCriterias::get_Item(long lIndex, IInterfaceRuleCriteria** pVal)
{
   try
   {
      if (!rule_criterias)
         return GetAccessDenied();

      CComObject<InterfaceRuleCriteria>* pInterfaceRuleCriteria = new CComObject<InterfaceRuleCriteria>();
      pInterfaceRuleCriteria->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::RuleCriteria> pRule = rule_criterias->GetItem(lIndex);
      if (!pRule)
         return DISP_E_BADINDEX;
   
      pInterfaceRuleCriteria->AttachItem(pRule);
      pInterfaceRuleCriteria->AttachParent(rule_criterias, true);
   
      pInterfaceRuleCriteria->AddRef();
      *pVal = pInterfaceRuleCriteria;   
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleCriterias::get_Count(LONG* pVal)
{
   try
   {
      if (!rule_criterias)
         return GetAccessDenied();

      *pVal = rule_criterias->GetCount();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleCriterias::Add(IInterfaceRuleCriteria** pVal)
{
   try
   {
      if (!rule_criterias)
         return GetAccessDenied();

      if (!rule_criterias)
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceRuleCriteria>* pIntDA = new CComObject<InterfaceRuleCriteria>();
      pIntDA->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::RuleCriteria> pDA = std::shared_ptr<HM::RuleCriteria>(new HM::RuleCriteria);
   
      pDA->SetRuleID(rule_criterias->GetRuleID());
   
      pIntDA->AttachItem(pDA);
      pIntDA->AttachParent(rule_criterias, false);
      pIntDA->AddRef();
   
      *pVal = pIntDA;
   
      return S_OK;   
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleCriterias::DeleteByDBID(LONG DBID)
{
   try
   {
      if (!rule_criterias)
         return GetAccessDenied();

      rule_criterias->DeleteItemByDBID(DBID);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleCriterias::Delete(LONG DBID)
{
   try
   {
      if (!rule_criterias)
         return GetAccessDenied();

      rule_criterias->DeleteItem(DBID);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleCriterias::Refresh(void)
{
   try
   {
      if (!rule_criterias)
         return GetAccessDenied();

      rule_criterias->Refresh();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


