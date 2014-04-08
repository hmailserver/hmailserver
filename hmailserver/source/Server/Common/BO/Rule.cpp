// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "Rule.h"

#include "RuleActions.h"
#include "RuleAction.h"
#include "../Persistence/PersistentRuleAction.h"

#include "RuleCriterias.h"
#include "RuleCriteria.h"
#include "../Persistence/PersistentRuleCriteria.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   Rule::Rule(void) :
      m_bActive(true),
      m_iAccountID(0),
      m_bUseAND(true),
      m_iSortOrder(0)
   {

   }

   Rule::~Rule(void)
   {

   }

   shared_ptr<RuleCriterias>
   Rule::GetCriterias()
   {
      if (!m_pCriterias)
      {
         m_pCriterias = shared_ptr<RuleCriterias>(new RuleCriterias(m_iID));
         m_pCriterias->Refresh();
      }

      return m_pCriterias;
   }

   shared_ptr<RuleActions>
   Rule::GetActions()
   {
      if (!m_pActions)
      {
         m_pActions = shared_ptr<RuleActions>(new RuleActions(m_iID));
         m_pActions->Refresh();
      }

      return m_pActions;
   }

   bool
   Rule::XMLStore(XNode *pParentNode, int iOptions)
   {
      XNode *pNode = pParentNode->AppendChild(_T("Rule"));

      pNode->AppendAttr(_T("Name"), m_sName);
      pNode->AppendAttr(_T("Active"), m_bActive ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("UseAND"), m_bUseAND ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("SortOrder"), StringParser::IntToString(m_iSortOrder));
   
      GetCriterias()->XMLStore(pNode, iOptions);
      GetActions()->XMLStore(pNode, iOptions);

      return true;
   }

   bool
   Rule::XMLLoad(XNode *pNode, int iOptions)
   {
      m_sName = pNode->GetAttrValue(_T("Name"));
      m_bActive = (pNode->GetAttrValue(_T("Active")) == _T("1"));
      m_bUseAND = (pNode->GetAttrValue(_T("UseAND")) == _T("1"));
      m_iSortOrder = _ttoi(pNode->GetAttrValue(_T("SortOrder")));

      return true;
   }

   bool
   Rule::XMLLoadSubItems(XNode *pNode, int iOptions)
   {
      if (GetCriterias()->GetCount() == 0)
         if (!GetCriterias()->XMLLoad(pNode, iOptions))
            return false;

      if (GetActions()->GetCount() == 0)
         if (!GetActions()->XMLLoad(pNode, iOptions))
            return false;

      return true;
   }
}