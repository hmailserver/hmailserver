// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "RouteAddress.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   RouteAddress::RouteAddress()
   {
      route_id_ = 0;
   }

   RouteAddress::~RouteAddress()
   {

   }

   bool 
   RouteAddress::XMLStore(XNode *pParentNode, int iOptions)
   {
      XNode *pNode = pParentNode->AppendChild(_T("RouteAddress"));

      pNode->AppendAttr(_T("Name"), address_);

      return true;
   }

   bool 
   RouteAddress::XMLLoad(XNode *pNode, int iOptions)
   {
      address_ = pNode->GetAttrValue(_T("Name"));

      return true;
   }

}
