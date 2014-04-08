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
      m_iRouteID = 0;
   }

   RouteAddress::~RouteAddress()
   {

   }

   bool 
   RouteAddress::XMLStore(XNode *pParentNode, int iOptions)
   {
      XNode *pNode = pParentNode->AppendChild(_T("RouteAddress"));

      pNode->AppendAttr(_T("Name"), m_sAddress);

      return true;
   }

   bool 
   RouteAddress::XMLLoad(XNode *pNode, int iOptions)
   {
      m_sAddress = pNode->GetAttrValue(_T("Name"));

      return true;
   }

}
