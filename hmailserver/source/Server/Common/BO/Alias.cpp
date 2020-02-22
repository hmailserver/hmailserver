// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "Alias.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   Alias::Alias() :
      domain_id_(0),
      active_(false)
   {

   }

   Alias::~Alias()
   {

   }

   bool 
   Alias::XMLStore(XNode *pParentNode, int iOptions)
   {
      XNode *pNode = pParentNode->AppendChild(_T("Alias"));

      pNode->AppendAttr(_T("Name"), name_);
      pNode->AppendAttr(_T("Value"), value_);
      pNode->AppendAttr(_T("Active"), active_ ? _T("1") : _T("0"));

      return true;
   }

   bool
   Alias::XMLLoad(XNode *pNode, int iOptions)
   {
      name_ = pNode->GetAttrValue(_T("Name"));
      value_ = pNode->GetAttrValue(_T("Value"));
      active_ = (pNode->GetAttrValue(_T("Active")) == _T("1"));

      return true;
   }


   size_t
   Alias::GetEstimatedCachingSize()
   {
      return 1024;
   }
}
