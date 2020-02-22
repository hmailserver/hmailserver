// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "InterfaceRuleCriteria.h"

#include "../Common/BO/RuleCriteria.h"
#include "../Common/BO/RuleCriterias.h"
#include "../Common/Persistence/PersistentRuleCriteria.h"

#include "COMError.h"

STDMETHODIMP InterfaceRuleCriteria::get_ID(long *pVal)
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

STDMETHODIMP InterfaceRuleCriteria::Save()
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      if (parent_collection_->GetRuleID() > 0)
      {
         if (!HM::PersistentRuleCriteria::SaveObject(object_))
         {
            // Saving failed.
            return COMError::GenerateError("Failed to save object. See hMailServer error log.");
         }
      }
   
      AddToParentCollection();
         
      return S_OK;
      
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleCriteria::get_RuleID(long *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = (long) object_->GetRuleID();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleCriteria::put_RuleID(long newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetRuleID(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleCriteria::get_UsePredefined(VARIANT_BOOL *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetUsePredefined() ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleCriteria::put_UsePredefined(VARIANT_BOOL newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetUsePredefined(newVal == VARIANT_TRUE);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleCriteria::get_HeaderField(BSTR *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetHeaderField().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleCriteria::put_HeaderField(BSTR newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetHeaderField(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleCriteria::get_MatchValue(BSTR *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetMatchValue().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleCriteria::put_MatchValue(BSTR newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetMatchValue(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleCriteria::get_PredefinedField(eRulePredefinedField *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = (eRulePredefinedField) object_->GetPredefinedField();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleCriteria::put_PredefinedField(eRulePredefinedField newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetPredefinedField((HM::RuleCriteria::PredefinedField)newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleCriteria::get_MatchType(eRuleMatchType *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = (eRuleMatchType) object_->GetMatchType();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleCriteria::put_MatchType(eRuleMatchType newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetMatchType((HM::RuleCriteria::MatchType)newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleCriteria::Delete()
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      if (!parent_collection_)
         return HM::PersistentRuleCriteria::DeleteObject(object_) ? S_OK : S_FALSE;
   
      parent_collection_->DeleteItemByDBID(object_->GetID());
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

