// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include "GreyListingWhiteAddress.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   GreyListingWhiteAddress::GreyListingWhiteAddress(void) 
   {
      
   }

   GreyListingWhiteAddress::~GreyListingWhiteAddress(void)
   {
   }

   
   String
   GreyListingWhiteAddress::GetName() const
   {  
      return white_ipaddress_;
   }

   bool 
   GreyListingWhiteAddress::XMLStore(XNode *pParentNode, int iOptions)
   {
      XNode *pNode = pParentNode->AppendChild(_T("WhiteAddress"));

      pNode->AppendAttr(_T("Name"), GetName());
      pNode->AppendAttr(_T("WhiteIPDescription"), white_ipdescription_);

      return true;
   }

   bool 
   GreyListingWhiteAddress::XMLLoad(XNode *pNode, int iOptions)
   {
      white_ipaddress_ = pNode->GetAttrValue(_T("Name"));
      white_ipdescription_ = pNode->GetAttrValue(_T("WhiteIPDescription"));

      return true;
   }

   String 
   GreyListingWhiteAddress::GetIPAddress() const
   {
      return white_ipaddress_;
   }


   void
   GreyListingWhiteAddress::SetIPAddress(const String &sIPAddress)
   {
      white_ipaddress_ = sIPAddress;
   }

   String 
   GreyListingWhiteAddress::GetUserEditableIPAddress() const
   {
      return SQLStatement::ConvertLikeToWildcard(white_ipaddress_);
   }


   void
   GreyListingWhiteAddress::SetUserEditableIPAddress(const String &sIPAddress)
   {
      white_ipaddress_ = SQLStatement::ConvertWildcardToLike(sIPAddress);
   }
}
