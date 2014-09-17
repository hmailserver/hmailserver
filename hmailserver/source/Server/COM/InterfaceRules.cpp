// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceRules.h"

#include "InterfaceRule.h"

#include "../Common/BO/Rules.h"
#include "../Common/BO/Rule.h"

void 
InterfaceRules::Attach(std::shared_ptr<HM::Rules> pRules)
{
   rules_ = pRules;
}

STDMETHODIMP InterfaceRules::get_ItemByDBID(long lDBID, IInterfaceRule** pVal)
{
   try
   {
      if (!rules_)
         return GetAccessDenied();

      CComObject<InterfaceRule>* pInterfaceRule = new CComObject<InterfaceRule>();
      pInterfaceRule->SetAuthentication(authentication_);
   
   
      std::shared_ptr<HM::Rule> pRule = rules_->GetItemByDBID(lDBID);
      if (!pRule)
         return DISP_E_BADINDEX;
   
      pInterfaceRule->AttachParent(rules_, true);
      pInterfaceRule->AttachItem(pRule);
      pInterfaceRule->AddRef();
      *pVal = pInterfaceRule;   
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRules::get_Item(long lIndex, IInterfaceRule** pVal)
{
   try
   {
      if (!rules_)
         return GetAccessDenied();

      CComObject<InterfaceRule>* pInterfaceRule = new CComObject<InterfaceRule>();
      pInterfaceRule->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::Rule> pRule = rules_->GetItem(lIndex);
      if (!pRule)
         return DISP_E_BADINDEX;
   
      pInterfaceRule->AttachParent(rules_, true);
      pInterfaceRule->AttachItem(pRule);
      pInterfaceRule->AddRef();
      *pVal = pInterfaceRule;   
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRules::get_Count(LONG* pVal)
{
   try
   {
      if (!rules_)
         return GetAccessDenied();

      *pVal = rules_->GetCount();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRules::Add(IInterfaceRule** pVal)
{
   try
   {
      if (!rules_)
         return GetAccessDenied();

      if (!rules_)
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceRule>* pIntDA = new CComObject<InterfaceRule>();
      pIntDA->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::Rule> pDA = std::shared_ptr<HM::Rule>(new HM::Rule);
   
      // Make sure that the new rule is
      // added to the right account.
      pDA->SetAccountID(rules_->GetAccountID());
   
      pIntDA->AttachItem(pDA);
      pIntDA->AttachParent(rules_, false);
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

STDMETHODIMP InterfaceRules::DeleteByDBID(LONG DBID)
{
   try
   {
      if (!rules_)
         return GetAccessDenied();

      rules_->DeleteItemByDBID(DBID);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRules::Refresh(void)
{
   try
   {
      if (!rules_)
         return GetAccessDenied();

      rules_->Refresh();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


