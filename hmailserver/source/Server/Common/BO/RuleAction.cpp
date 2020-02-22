// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include ".\ruleaction.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   RuleAction::RuleAction(void) :
      type_(Unknown),
      rule_id_(0),
      sort_order_(0),
      route_id_(0)
   {
   }

   RuleAction::~RuleAction(void)
   {
   }

   bool 
   RuleAction::XMLStore(XNode *pRuleNode, int iOptions)
   {

      XNode *pNode = pRuleNode->AppendChild(_T("Action"));

      pNode->AppendAttr(_T("Type"), StringParser::IntToString(type_));
      pNode->AppendAttr(_T("Subject"), subject_);
      pNode->AppendAttr(_T("Body"), body_);
      pNode->AppendAttr(_T("FromAddress"), from_address_);
      pNode->AppendAttr(_T("FromName"), from_name_);
      pNode->AppendAttr(_T("IMAPFolder"), imapfolder_);
      pNode->AppendAttr(_T("FileName"), filename_);
      pNode->AppendAttr(_T("To"), to_);
      pNode->AppendAttr(_T("ScriptFunction"), script_function_);
      pNode->AppendAttr(_T("SortOrder"), StringParser::IntToString(sort_order_));
      pNode->AppendAttr(_T("Header"), header_name_);
      pNode->AppendAttr(_T("Value"), value_);
      pNode->AppendAttr(_T("RouteID"), StringParser::IntToString(route_id_));

      return true;
   }

   bool
   RuleAction::XMLLoad(XNode *pNode, int iOptions)
   {
      type_ = (RuleAction::Type) _ttoi(pNode->GetAttrValue(_T("Type")));
      subject_ = pNode->GetAttrValue(_T("Subject"));
      body_ = pNode->GetAttrValue(_T("Body"));
      from_address_ = pNode->GetAttrValue(_T("FromAddress"));
      from_name_ = pNode->GetAttrValue(_T("FromName"));
      imapfolder_ = pNode->GetAttrValue(_T("IMAPFolder"));
      filename_ = pNode->GetAttrValue(_T("FileName"));
      to_ = pNode->GetAttrValue(_T("To"));
      script_function_ = pNode->GetAttrValue(_T("ScriptFunction"));
      sort_order_ = _ttoi(pNode->GetAttrValue(_T("SortOrder")));
      header_name_ = pNode->GetAttrValue(_T("Header"));
      value_ = pNode->GetAttrValue(_T("Value"));
      route_id_ = _ttoi(pNode->GetAttrValue(_T("RouteID")));

      return true;
   }
}