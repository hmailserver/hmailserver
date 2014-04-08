// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class RouteAddress : public BusinessObject<RouteAddress>
   {
   public:
	   RouteAddress();
	   virtual ~RouteAddress();

      String GetName() const {return m_sAddress; }

      __int64 GetRouteID() const {return m_iRouteID;}
      void SetRouteID(__int64 iNewVal) {m_iRouteID = iNewVal; }

      String GetAddress() const {return m_sAddress; }
      void SetAddress(const String &sAddress) {m_sAddress = sAddress; } 
   
      bool XMLStore(XNode *pNode, int iOptions);
      bool XMLLoad(XNode *pNode, int iOptions);
      bool XMLLoadSubItems(XNode *pNode, int iOptions) {return true;}

   protected:

      __int64 m_iRouteID;
      String m_sAddress;
      

   };
}