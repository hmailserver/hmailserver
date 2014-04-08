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
	   return _lowerIP.ToString();
   }

   bool
   IncomingRelay::SetLowerIPString(const String & sValue)
   {
      return _lowerIP.TryParse(sValue);
   }

   String 
   IncomingRelay::GetUpperIPString() const 
   {
	   return _upperIP.ToString();
   }


   bool
   IncomingRelay::SetUpperIPString(const String & sValue)
   {
      return _upperIP.TryParse(sValue);
   }

   bool
   IncomingRelay::IPIsWithinRange(const IPAddress &address) const
   {  
      if (address.GetAddress1() >= _lowerIP.GetAddress1() && address.GetAddress1() <= _upperIP.GetAddress1())
      {
         if (address.GetType() == IPAddress::IPV4)
            return true;

         if (address.GetAddress2() >= _lowerIP.GetAddress2() && address.GetAddress2() <= _upperIP.GetAddress2())
            return true;
      }

      return false;
   }

   bool 
   IncomingRelay::XMLStore(XNode *pParentNode, int iOptions)
   {
      XNode *pNode = pParentNode->AppendChild(_T("IncomingRelay"));

      pNode->AppendAttr(_T("Name"), _name);
      pNode->AppendAttr(_T("LowerIP"), String(_lowerIP.ToString()));
      pNode->AppendAttr(_T("UpperIP"), String(_upperIP.ToString()));

      return true;
   }

   bool 
   IncomingRelay::XMLLoad(XNode *pIncomingRelayNode, int iOptions)
   {
      _name = pIncomingRelayNode->GetAttrValue(_T("Name"));
      
      _lowerIP.TryParse(pIncomingRelayNode->GetAttrValue(_T("LowerIP")));
      _upperIP.TryParse(pIncomingRelayNode->GetAttrValue(_T("UpperIP")));

      return true;
   }
}