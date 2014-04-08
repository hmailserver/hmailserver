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
      m_eFieldType(FTFrom),
      m_eMatchType(Equals),
      m_iRuleID(0),
      m_bUsePredefinedField(true)
   {
   }

   RuleCriteria::~RuleCriteria(void)
   {
   }

   bool 
   RuleCriteria::XMLStore(XNode *pRuleNode, int iOptions)
   {
      XNode *pNode = pRuleNode->AppendChild(_T("Criteria"));

      pNode->AppendAttr(_T("MatchString"), m_sMatchString);
      pNode->AppendAttr(_T("FieldType"), StringParser::IntToString(m_eFieldType));
      pNode->AppendAttr(_T("MatchType"), StringParser::IntToString(m_eMatchType));
      pNode->AppendAttr(_T("HeaderField"), m_sHeaderField);
      pNode->AppendAttr(_T("UsePredefinedField"), m_bUsePredefinedField ? _T("1") : _T("0"));

      return true;
   }

   bool
   RuleCriteria::XMLLoad(XNode *pNode, int iOptions)
   {
      m_sMatchString = pNode->GetAttrValue(_T("MatchString"));
      m_eFieldType = (PredefinedField) _ttoi(pNode->GetAttrValue(_T("FieldType")));
      m_eMatchType = (MatchType) _ttoi(pNode->GetAttrValue(_T("MatchType")));
      m_sHeaderField = pNode->GetAttrValue(_T("HeaderField"));
      m_bUsePredefinedField = (pNode->GetAttrValue(_T("UsePredefinedField")) == _T("1"));

      return true;
   }

}