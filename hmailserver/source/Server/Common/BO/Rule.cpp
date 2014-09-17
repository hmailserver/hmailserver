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
      active_(true),
      account_id_(0),
      use_and_(true),
      sort_order_(0)
   {

   }

   Rule::~Rule(void)
   {

   }

   std::shared_ptr<RuleCriterias>
   Rule::GetCriterias()
   {
      if (!criterias_)
      {
         criterias_ = std::shared_ptr<RuleCriterias>(new RuleCriterias(id_));
         criterias_->Refresh();
      }

      return criterias_;
   }

   std::shared_ptr<RuleActions>
   Rule::GetActions()
   {
      if (!actions_)
      {
         actions_ = std::shared_ptr<RuleActions>(new RuleActions(id_));
         actions_->Refresh();
      }

      return actions_;
   }

   bool
   Rule::XMLStore(XNode *pParentNode, int iOptions)
   {
      XNode *pNode = pParentNode->AppendChild(_T("Rule"));

      pNode->AppendAttr(_T("Name"), name_);
      pNode->AppendAttr(_T("Active"), active_ ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("UseAND"), use_and_ ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("SortOrder"), StringParser::IntToString(sort_order_));
   
      GetCriterias()->XMLStore(pNode, iOptions);
      GetActions()->XMLStore(pNode, iOptions);

      return true;
   }

   bool
   Rule::XMLLoad(XNode *pNode, int iOptions)
   {
      name_ = pNode->GetAttrValue(_T("Name"));
      active_ = (pNode->GetAttrValue(_T("Active")) == _T("1"));
      use_and_ = (pNode->GetAttrValue(_T("UseAND")) == _T("1"));
      sort_order_ = _ttoi(pNode->GetAttrValue(_T("SortOrder")));

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