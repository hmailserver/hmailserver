// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"

#include "InterfaceRuleActions.h"

#include "InterfaceRuleAction.h"

#include "../Common/BO/RuleActions.h"
#include "../Common/BO/RuleAction.h"


void 
InterfaceRuleActions::Attach(shared_ptr<HM::RuleActions> pActions)
{
   m_pRuleActions = pActions;
}


STDMETHODIMP InterfaceRuleActions::get_ItemByDBID(long lDBID, IInterfaceRuleAction** pVal)
{
   try
   {
      if (!m_pRuleActions)
         return GetAccessDenied();

      CComObject<InterfaceRuleAction>* pInterfaceRuleAction = new CComObject<InterfaceRuleAction>();
      pInterfaceRuleAction->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::RuleAction> pRule = m_pRuleActions->GetItemByDBID(lDBID);
      if (!pRule)
         return DISP_E_BADINDEX;
   
      pInterfaceRuleAction->AttachItem(pRule);
      pInterfaceRuleAction->AttachParent(m_pRuleActions, true);
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
      if (!m_pRuleActions)
         return GetAccessDenied();

      CComObject<InterfaceRuleAction>* pInterfaceRuleAction = new CComObject<InterfaceRuleAction>();
      pInterfaceRuleAction->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::RuleAction> pRule = m_pRuleActions->GetItem(lIndex);
      if (!pRule)
         return DISP_E_BADINDEX;
   
      pInterfaceRuleAction->AttachItem(pRule);
      pInterfaceRuleAction->AttachParent(m_pRuleActions, true);
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
      if (!m_pRuleActions)
         return GetAccessDenied();

      *pVal = m_pRuleActions->GetCount();
   
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
      if (!m_pRuleActions)
         return GetAccessDenied();

      if (!m_pRuleActions)
         return m_pAuthentication->GetAccessDenied();
   
      CComObject<InterfaceRuleAction>* pIntRA = new CComObject<InterfaceRuleAction>();
      pIntRA->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::RuleAction> pRuleAction = shared_ptr<HM::RuleAction>(new HM::RuleAction);
   
      pRuleAction->SetRuleID(m_pRuleActions->GetRuleID());
   
      pIntRA->AttachItem(pRuleAction);
      pIntRA->AttachParent(m_pRuleActions, false);
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
      if (!m_pRuleActions)
         return GetAccessDenied();

      m_pRuleActions->DeleteItemByDBID(DBID);
   
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
      if (!m_pRuleActions)
         return GetAccessDenied();

      m_pRuleActions->DeleteItem(DBID);
   
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
      if (!m_pRuleActions)
         return GetAccessDenied();

      m_pRuleActions->Refresh();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


