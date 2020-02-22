// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include ".\domainalias.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   DomainAlias::DomainAlias(void) :
      domain_id_(0)
   {

   }

   DomainAlias::~DomainAlias(void)
   {

   }

   bool 
   DomainAlias::XMLStore(XNode *pParentNode, int iOptions)
   {
      XNode *pNode = pParentNode->AppendChild(_T("DomainAlias"));
      pNode->AppendAttr(_T("Name"), alias_);

      return true;

   }

   bool 
   DomainAlias::XMLLoad(XNode *pAliasNode, int iOptions)
   {
      alias_ = pAliasNode->GetAttrValue(_T("Name"));

      return true;
   }

}