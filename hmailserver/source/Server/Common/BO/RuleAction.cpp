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
      m_iType(Unknown),
      m_iRuleID(0),
      m_iSortOrder(0),
      m_iRouteID(0)
   {
   }

   RuleAction::~RuleAction(void)
   {
   }

   bool 
   RuleAction::XMLStore(XNode *pRuleNode, int iOptions)
   {

      XNode *pNode = pRuleNode->AppendChild(_T("Action"));

      pNode->AppendAttr(_T("Type"), StringParser::IntToString(m_iType));
      pNode->AppendAttr(_T("Subject"), m_sSubject);
      pNode->AppendAttr(_T("Body"), m_sBody);
      pNode->AppendAttr(_T("FromAddress"), m_sFromAddress);
      pNode->AppendAttr(_T("FromName"), m_sFromName);
      pNode->AppendAttr(_T("IMAPFolder"), m_sIMAPFolder);
      pNode->AppendAttr(_T("FileName"), m_sFilename);
      pNode->AppendAttr(_T("To"), m_sTo);
      pNode->AppendAttr(_T("ScriptFunction"), m_sScriptFunction);
      pNode->AppendAttr(_T("SortOrder"), StringParser::IntToString(m_iSortOrder));
      pNode->AppendAttr(_T("Header"), m_sHeaderName);
      pNode->AppendAttr(_T("Value"), m_sValue);
      pNode->AppendAttr(_T("RouteID"), StringParser::IntToString(m_iRouteID));

      return true;
   }

   bool
   RuleAction::XMLLoad(XNode *pNode, int iOptions)
   {
      m_iType = (RuleAction::Type) _ttoi(pNode->GetAttrValue(_T("Type")));
      m_sSubject = pNode->GetAttrValue(_T("Subject"));
      m_sBody = pNode->GetAttrValue(_T("Body"));
      m_sFromAddress = pNode->GetAttrValue(_T("FromAddress"));
      m_sFromName = pNode->GetAttrValue(_T("FromName"));
      m_sIMAPFolder = pNode->GetAttrValue(_T("IMAPFolder"));
      m_sFilename = pNode->GetAttrValue(_T("FileName"));
      m_sTo = pNode->GetAttrValue(_T("To"));
      m_sScriptFunction = pNode->GetAttrValue(_T("ScriptFunction"));
      m_iSortOrder = _ttoi(pNode->GetAttrValue(_T("SortOrder")));
      m_sHeaderName = pNode->GetAttrValue(_T("Header"));
      m_sValue = pNode->GetAttrValue(_T("Value"));
      m_iRouteID = _ttoi(pNode->GetAttrValue(_T("RouteID")));

      return true;
   }
}