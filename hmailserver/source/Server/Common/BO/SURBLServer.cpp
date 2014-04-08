// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include "SURBLServer.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   SURBLServer::SURBLServer(void) :
      m_iScore(0),
      m_bActive(false)
   {
       
   }

   SURBLServer::~SURBLServer(void)
   {
   }

   bool 
   SURBLServer::XMLStore(XNode *pParentNode, int iOptions)
   {
      XNode *pNode = pParentNode->AppendChild(_T("SURBLServer"));

      pNode->AppendAttr(_T("Name"), m_sDNSHost);
      pNode->AppendAttr(_T("Active"), m_bActive ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("RejectMessage"), m_sRejectMessage);
      pNode->AppendAttr(_T("Score"), StringParser::IntToString(m_iScore));

      return true;
   }

   bool 
   SURBLServer::XMLLoad(XNode *pNode, int iOptions)
   {
      m_sDNSHost = pNode->GetAttrValue(_T("Name"));
      m_bActive = pNode->GetAttrValue(_T("Active")) == _T("1");
      m_sRejectMessage = pNode->GetAttrValue(_T("RejectMessage"));
      m_iScore = _ttoi(pNode->GetAttrValue(_T("Score")));

      return true;
   }
}