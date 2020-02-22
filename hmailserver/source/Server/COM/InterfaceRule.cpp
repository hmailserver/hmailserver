// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "InterfaceRule.h"

#include "..\Common\Bo\Rule.h"
#include "..\Common\Bo\Rules.h"
#include "..\Common\Persistence\PersistentRule.h"

#include "InterfaceRuleCriterias.h"
#include "InterfaceRuleActions.h"

#include "COMError.h"

STDMETHODIMP InterfaceRule::get_ID(long *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = (long) object_->GetID();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRule::get_AccountID(LONG* pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = (long) object_->GetAccountID();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRule::put_AccountID(LONG newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetAccountID(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRule::get_Name(BSTR* pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetName().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRule::put_Name(BSTR newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetName(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRule::get_Active(VARIANT_BOOL* pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetActive() ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRule::put_Active(VARIANT_BOOL newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetActive(newVal == VARIANT_TRUE);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRule::get_UseAND(VARIANT_BOOL* pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetUseAND() ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRule::put_UseAND(VARIANT_BOOL newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetUseAND(newVal == VARIANT_TRUE);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRule::get_Criterias(IInterfaceRuleCriterias **pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      CComObject<InterfaceRuleCriterias>* pItem = new CComObject<InterfaceRuleCriterias >();
      pItem->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::RuleCriterias> pRuleCriterias = object_->GetCriterias();
   
      if (pRuleCriterias)
      {
         pItem->Attach(pRuleCriterias);
         pItem->AddRef();
         *pVal = pItem;
      }
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRule::get_Actions(IInterfaceRuleActions **pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      CComObject<InterfaceRuleActions>* pItem = new CComObject<InterfaceRuleActions >();
   
      std::shared_ptr<HM::RuleActions> pRuleActions = object_->GetActions();
   
      if (pRuleActions)
      {
         pItem->Attach(pRuleActions);
         pItem->AddRef();
         *pVal = pItem;
      }
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRule::MoveUp()
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      if (object_->GetID() <= 0)
         return COMError::GenerateError("Object not yet saved.");
   
      parent_collection_->MoveUp(object_->GetID());
      return S_OK;
   
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRule::MoveDown()
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      if (object_->GetID() <= 0)
         return COMError::GenerateError("Object not yet saved.");
         
      parent_collection_->MoveDown(object_->GetID());
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRule::Save()
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      // Set the sort order of the rule.
      if (object_->GetID() == 0 && object_->GetSortOrder() == 0)
      {
         std::vector<std::shared_ptr<HM::Rule> > vecExistingRules = parent_collection_->GetVector();
   
         // Determine the highest SortOrder.
         if (vecExistingRules.size() == 0)
            object_->SetSortOrder(1);
         else
         {
            std::shared_ptr<HM::Rule> pLastRule = vecExistingRules[vecExistingRules.size() -1];
            object_->SetSortOrder(pLastRule->GetSortOrder() +1);
         }
      }
   
      if (HM::PersistentRule::SaveObject(object_))
      {
         // Add to parent collection
         AddToParentCollection();
   
         return S_OK;
      }
   
      return COMError::GenerateError("Failed to save object. See hMailServer error log.");
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRule::Delete()
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      if (!parent_collection_)
         return HM::PersistentRule::DeleteObject(object_) ? S_OK : S_FALSE;
   
      parent_collection_->DeleteItemByDBID(object_->GetID());
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

