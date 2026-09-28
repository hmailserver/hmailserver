// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "stdafx.h"
#include "ARCTrustedSealer.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   ARCTrustedSealer::ARCTrustedSealer(void)
   {

   }

   ARCTrustedSealer::~ARCTrustedSealer(void)
   {
   }

   bool
   ARCTrustedSealer::XMLStore(XNode *pParentNode, int iOptions)
   {
      XNode *pNode = pParentNode->AppendChild(_T("ARCTrustedSealer"));

      pNode->AppendAttr(_T("Name"), domain_);
      pNode->AppendAttr(_T("Description"), description_);

      return true;
   }

   bool
   ARCTrustedSealer::XMLLoad(XNode *pNode, int iOptions)
   {
      domain_ = pNode->GetAttrValue(_T("Name"));
      description_ = pNode->GetAttrValue(_T("Description"));

      return true;
   }
}
