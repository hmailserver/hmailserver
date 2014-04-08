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
      m_iDomainID(0),
      m_bActive(false)
   {

   }

   Alias::~Alias()
   {

   }

   bool 
   Alias::XMLStore(XNode *pParentNode, int iOptions)
   {
      XNode *pNode = pParentNode->AppendChild(_T("Alias"));

      pNode->AppendAttr(_T("Name"), m_sName);
      pNode->AppendAttr(_T("Value"), m_sValue);
      pNode->AppendAttr(_T("Active"), m_bActive ? _T("1") : _T("0"));

      return true;
   }

   bool
   Alias::XMLLoad(XNode *pNode, int iOptions)
   {
      m_sName = pNode->GetAttrValue(_T("Name"));
      m_sValue = pNode->GetAttrValue(_T("Value"));
      m_bActive = (pNode->GetAttrValue(_T("Active")) == _T("1"));

      return true;
   }

}
