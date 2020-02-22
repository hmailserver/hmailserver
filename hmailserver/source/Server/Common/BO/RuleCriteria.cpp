// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include ".\rulecriteria.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   RuleCriteria::RuleCriteria(void) :
      field_type_(FTFrom),
      match_type_(Equals),
      rule_id_(0),
      use_predefined_field_(true)
   {
   }

   RuleCriteria::~RuleCriteria(void)
   {
   }

   bool 
   RuleCriteria::XMLStore(XNode *pRuleNode, int iOptions)
   {
      XNode *pNode = pRuleNode->AppendChild(_T("Criteria"));

      pNode->AppendAttr(_T("MatchString"), match_string_);
      pNode->AppendAttr(_T("FieldType"), StringParser::IntToString(field_type_));
      pNode->AppendAttr(_T("MatchType"), StringParser::IntToString(match_type_));
      pNode->AppendAttr(_T("HeaderField"), header_field_);
      pNode->AppendAttr(_T("UsePredefinedField"), use_predefined_field_ ? _T("1") : _T("0"));

      return true;
   }

   bool
   RuleCriteria::XMLLoad(XNode *pNode, int iOptions)
   {
      match_string_ = pNode->GetAttrValue(_T("MatchString"));
      field_type_ = (PredefinedField) _ttoi(pNode->GetAttrValue(_T("FieldType")));
      match_type_ = (MatchType) _ttoi(pNode->GetAttrValue(_T("MatchType")));
      header_field_ = pNode->GetAttrValue(_T("HeaderField"));
      use_predefined_field_ = (pNode->GetAttrValue(_T("UsePredefinedField")) == _T("1"));

      return true;
   }

}