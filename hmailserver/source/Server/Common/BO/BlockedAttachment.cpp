// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include "BlockedAttachment.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   BlockedAttachment::BlockedAttachment(void)
   {
      
   }

   BlockedAttachment::~BlockedAttachment(void)
   {
   }

   bool 
   BlockedAttachment::XMLStore(XNode *pParentNode, int iOptions)
   {
      XNode *pNode = pParentNode->AppendChild(_T("BlockedAttachment"));

      pNode->AppendAttr(_T("Name"), m_sWildcard);
      pNode->AppendAttr(_T("Description"), m_sDescription);

      return true;
   }

   bool 
   BlockedAttachment::XMLLoad(XNode *pNode, int iOptions)
   {
      m_sWildcard = pNode->GetAttrValue(_T("Name"));
      m_sDescription = pNode->GetAttrValue(_T("Description"));

      return true;
   }
}