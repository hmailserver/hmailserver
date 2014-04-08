// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include "WhiteListAddress.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   WhiteListAddress::WhiteListAddress(void)
   {
      
   }

   WhiteListAddress::~WhiteListAddress(void)
   {
   }
   
   String
   WhiteListAddress::GetName() const
   {  
      String sName = m_iWhiteLowerIPAddress.ToString() + m_iWhiteUpperIPAddress.ToString();
      return sName;
   }

   bool 
   WhiteListAddress::XMLStore(XNode *pParentNode, int iOptions)
   {
      XNode *pNode = pParentNode->AppendChild(_T("WhiteAddress"));

      pNode->AppendAttr(_T("Name"), GetName());
      pNode->AppendAttr(_T("LowerIPAddress"), String(m_iWhiteLowerIPAddress.ToString()));
      pNode->AppendAttr(_T("UpperIPAddress"), String(m_iWhiteUpperIPAddress.ToString()));
      pNode->AppendAttr(_T("EMailAddress"), m_sWhiteEmailAddress);
      pNode->AppendAttr(_T("Description"), m_sWhiteDescription);

      return true;

      
   }

   bool 
   WhiteListAddress::XMLLoad(XNode *pNode, int iOptions)
   {
      m_iWhiteLowerIPAddress.TryParse(pNode->GetAttrValue(_T("LowerIPAddress")));
      m_iWhiteUpperIPAddress.TryParse(pNode->GetAttrValue(_T("UpperIPAddress")));
      m_sWhiteEmailAddress = pNode->GetAttrValue(_T("EMailAddress"));
      m_sWhiteDescription = pNode->GetAttrValue(_T("Description"));

      return true;
   }

   bool
   WhiteListAddress::SetLowerIPAddress(const String &iIPAddress)
   {
      return m_iWhiteLowerIPAddress.TryParse(iIPAddress);
   }

   bool
   WhiteListAddress::SetUpperIPAddress(const String &iIPAddress)
   {
      return m_iWhiteUpperIPAddress.TryParse(iIPAddress);
   }
}
