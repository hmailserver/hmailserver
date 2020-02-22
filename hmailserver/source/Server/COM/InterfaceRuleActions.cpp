// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"

#include "InterfaceRuleActions.h"

#include "InterfaceRuleAction.h"

#include "../Common/BO/RuleActions.h"
#include "../Common/BO/RuleAction.h"


void 
InterfaceRuleActions::Attach(std::shared_ptr<HM::RuleActions> pActions)
{
   rule_actions_ = pActions;
}


STDMETHODIMP InterfaceRuleActions::get_ItemByDBID(long lDBID, IInterfaceRuleAction** pVal)
{
   try
   {
      if (!rule_actions_)
         return GetAccessDenied();

      CComObject<InterfaceRuleAction>* pInterfaceRuleAction = new CComObject<InterfaceRuleAction>();
      pInterfaceRuleAction->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::RuleAction> pRule = rule_actions_->GetItemByDBID(lDBID);
      if (!pRule)
         return DISP_E_BADINDEX;
   
      pInterfaceRuleAction->AttachItem(pRule);
      pInterfaceRuleAction->AttachParent(rule_actions_, true);
      pInterfaceRuleAction->AddRef();
      *pVal = pInterfaceRuleAction;   
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleActions::get_Item(long lIndex, IInterfaceRuleAction** pVal)
{
   try
   {
      if (!rule_actions_)
         return GetAccessDenied();

      CComObject<InterfaceRuleAction>* pInterfaceRuleAction = new CComObject<InterfaceRuleAction>();
      pInterfaceRuleAction->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::RuleAction> pRule = rule_actions_->GetItem(lIndex);
      if (!pRule)
         return DISP_E_BADINDEX;
   
      pInterfaceRuleAction->AttachItem(pRule);
      pInterfaceRuleAction->AttachParent(rule_actions_, true);
      pInterfaceRuleAction->AddRef();
      *pVal = pInterfaceRuleAction;   
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleActions::get_Count(LONG* pVal)
{
   try
   {
      if (!rule_actions_)
         return GetAccessDenied();

      *pVal = rule_actions_->GetCount();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleActions::Add(IInterfaceRuleAction** pVal)
{
   try
   {
      if (!rule_actions_)
         return GetAccessDenied();

      if (!rule_actions_)
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceRuleAction>* pIntRA = new CComObject<InterfaceRuleAction>();
      pIntRA->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::RuleAction> pRuleAction = std::shared_ptr<HM::RuleAction>(new HM::RuleAction);
   
      pRuleAction->SetRuleID(rule_actions_->GetRuleID());
   
      pIntRA->AttachItem(pRuleAction);
      pIntRA->AttachParent(rule_actions_, false);
      pIntRA->AddRef();
   
      *pVal = pIntRA;
   
      return S_OK;   
   
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleActions::DeleteByDBID(LONG DBID)
{
   try
   {
      if (!rule_actions_)
         return GetAccessDenied();

      rule_actions_->DeleteItemByDBID(DBID);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleActions::Delete(LONG DBID)
{
   try
   {
      if (!rule_actions_)
         return GetAccessDenied();

      rule_actions_->DeleteItem(DBID);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleActions::Refresh(void)
{
   try
   {
      if (!rule_actions_)
         return GetAccessDenied();

      rule_actions_->Refresh();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


