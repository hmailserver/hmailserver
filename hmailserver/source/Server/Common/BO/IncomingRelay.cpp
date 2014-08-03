// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IncomingRelay.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{

   IncomingRelay::IncomingRelay()
   {

   }

   IncomingRelay::~IncomingRelay()
   {

   }

   String 
   IncomingRelay::GetLowerIPString() const 
   {
	   return lower_ip_.ToString();
   }

   bool
   IncomingRelay::SetLowerIPString(const String & sValue)
   {
      return lower_ip_.TryParse(sValue);
   }

   String 
   IncomingRelay::GetUpperIPString() const 
   {
	   return upper_ip_.ToString();
   }


   bool
   IncomingRelay::SetUpperIPString(const String & sValue)
   {
      return upper_ip_.TryParse(sValue);
   }

   bool
   IncomingRelay::IPIsWithinRange(const IPAddress &address) const
   {  
      if (address.GetAddress1() >= lower_ip_.GetAddress1() && address.GetAddress1() <= upper_ip_.GetAddress1())
      {
         if (address.GetType() == IPAddress::IPV4)
            return true;

         if (address.GetAddress2() >= lower_ip_.GetAddress2() && address.GetAddress2() <= upper_ip_.GetAddress2())
            return true;
      }

      return false;
   }

   bool 
   IncomingRelay::XMLStore(XNode *pParentNode, int iOptions)
   {
      XNode *pNode = pParentNode->AppendChild(_T("IncomingRelay"));

      pNode->AppendAttr(_T("Name"), name_);
      pNode->AppendAttr(_T("LowerIP"), String(lower_ip_.ToString()));
      pNode->AppendAttr(_T("UpperIP"), String(upper_ip_.ToString()));

      return true;
   }

   bool 
   IncomingRelay::XMLLoad(XNode *pIncomingRelayNode, int iOptions)
   {
      name_ = pIncomingRelayNode->GetAttrValue(_T("Name"));
      
      lower_ip_.TryParse(pIncomingRelayNode->GetAttrValue(_T("LowerIP")));
      upper_ip_.TryParse(pIncomingRelayNode->GetAttrValue(_T("UpperIP")));

      return true;
   }
}