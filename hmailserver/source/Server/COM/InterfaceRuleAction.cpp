// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "InterfaceRuleAction.h"

#include "../Common/BO/RuleAction.h"
#include "../Common/BO/RuleActions.h"
#include "../Common/Persistence/PersistentRuleAction.h"
#include "../Common/Util/Encoding/ModifiedUTF7.h"

#include "COMError.h"

STDMETHODIMP InterfaceRuleAction::get_ID(long *pVal)
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

STDMETHODIMP InterfaceRuleAction::Save()
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      // Set the sort order of the rule.
      if (object_->GetID() == 0 && object_->GetSortOrder() == 0)
      {
         std::vector<std::shared_ptr<HM::RuleAction> > vecExistingRuleActions = parent_collection_->GetVector();
   
         // Determine the highest SortOrder.
         if (vecExistingRuleActions.size() == 0)
            object_->SetSortOrder(1);
         else
         {
            std::shared_ptr<HM::RuleAction> pLastRuleAction = vecExistingRuleActions[vecExistingRuleActions.size() -1];
            object_->SetSortOrder(pLastRuleAction->GetSortOrder() +1);
         }
      }
   
      if (parent_collection_->GetRuleID() > 0)
      {
         if (!HM::PersistentRuleAction::SaveObject(object_))
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

STDMETHODIMP InterfaceRuleAction::MoveUp()
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      parent_collection_->MoveUp(object_);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleAction::MoveDown()
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      parent_collection_->MoveDown(object_);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleAction::get_RuleID(long *pVal)
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

STDMETHODIMP InterfaceRuleAction::put_RuleID(long newVal)
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

STDMETHODIMP InterfaceRuleAction::get_Type(eRuleActionType *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = (eRuleActionType) object_->GetType();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleAction::put_Type(eRuleActionType newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetType((HM::RuleAction::Type) newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleAction::get_RouteID(long *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = (long) object_->GetRouteID();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleAction::put_RouteID(long newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetRouteID(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleAction::get_Subject(BSTR *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetSubject().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleAction::put_Subject(BSTR newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetSubject(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleAction::get_Body(BSTR *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetBody().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleAction::put_Body(BSTR newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetBody(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleAction::get_FromName(BSTR *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetFromName().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleAction::put_FromName(BSTR newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetFromName(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleAction::get_FromAddress(BSTR *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetFromAddress().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleAction::put_FromAddress(BSTR newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetFromAddress(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleAction::get_Filename(BSTR *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetFilename().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleAction::put_Filename(BSTR newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetFilename(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleAction::get_To(BSTR *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetTo().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleAction::put_To(BSTR newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetTo(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleAction::get_ScriptFunction(BSTR *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetScriptFunction().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleAction::put_ScriptFunction(BSTR newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetScriptFunction(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleAction::get_IMAPFolder(BSTR *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      USES_CONVERSION;
   
      // Convert the modified UTF7 string into unicode.
      // Since hMailServer core don't know unicode, we
      // have to do it out here in the interface.
      
      HM::String sValue = object_->GetIMAPFolder();
      HM::String sResult = HM::ModifiedUTF7::Decode(T2A(sValue.GetBuffer()));
   
      *pVal = sResult.AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleAction::put_IMAPFolder(BSTR newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      HM::String sUnicode (newVal);
   
      // Convert the unciode string to a modified UTF7.
      // Since hMailServer core don't know unicode, we
      // have to do it out here in the interface.
      HM::AnsiString sRetVal = HM::ModifiedUTF7::Encode(sUnicode);
      object_->SetIMAPFolder(sRetVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleAction::get_HeaderName(BSTR *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetHeaderName().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleAction::put_HeaderName(BSTR newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetHeaderName(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleAction::get_Value(BSTR *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetValue().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleAction::put_Value(BSTR newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetValue(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRuleAction::Delete()
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      if (!parent_collection_)
         return HM::PersistentRuleAction::DeleteObject(object_) ? S_OK : S_FALSE;
   
      parent_collection_->DeleteItemByDBID(object_->GetID());
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

