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

      String GetName() const {return address_; }

      __int64 GetRouteID() const {return route_id_;}
      void SetRouteID(__int64 iNewVal) {route_id_ = iNewVal; }

      String GetAddress() const {return address_; }
      void SetAddress(const String &sAddress) {address_ = sAddress; } 
   
      bool XMLStore(XNode *pNode, int iOptions);
      bool XMLLoad(XNode *pNode, int iOptions);
      bool XMLLoadSubItems(XNode *pNode, int iOptions) {return true;}

   protected:

      __int64 route_id_;
      String address_;
      

   };
}