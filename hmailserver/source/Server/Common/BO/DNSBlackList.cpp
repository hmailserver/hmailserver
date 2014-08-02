// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include "DNSBlackList.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   DNSBlackList::DNSBlackList(void) :
      score_(0),
      active_(false)
   {
      
   }

   DNSBlackList::~DNSBlackList(void)
   {
   }

   bool 
   DNSBlackList::XMLStore(XNode *pParentNode, int iOptions)
   {
      XNode *pNode = pParentNode->AppendChild(_T("DNSBlackList"));

      pNode->AppendAttr(_T("Name"), dnshost_);
      pNode->AppendAttr(_T("Active"), active_ ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("RejectMessage"), reject_message_);
      pNode->AppendAttr(_T("ExpectedResult"), expected_result_);
      pNode->AppendAttr(_T("Score"), StringParser::IntToString(score_));

      return true;
   }

   bool 
   DNSBlackList::XMLLoad(XNode *pNode, int iOptions)
   {
      dnshost_ = pNode->GetAttrValue(_T("Name"));
      active_ = pNode->GetAttrValue(_T("Active")) == _T("1");
      reject_message_ = pNode->GetAttrValue(_T("RejectMessage"));
      expected_result_ = pNode->GetAttrValue(_T("ExpectedResult"));
      score_ = _ttoi(pNode->GetAttrValue(_T("Score")));

      return true;

   }
}