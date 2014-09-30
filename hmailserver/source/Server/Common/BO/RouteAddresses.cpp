// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "RouteAddresses.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   RouteAddresses::RouteAddresses(__int64 iRouteID) :
      route_id_(iRouteID)
   {
      
   }

   RouteAddresses::~RouteAddresses()
   {

   }

   void 
   RouteAddresses::Refresh()
   {
      String sSQL;
      sSQL.Format(_T("select * from hm_routeaddresses where routeaddressrouteid = %I64d"), route_id_);;

      DBLoad_(sSQL);
   }

   void
   RouteAddresses::DeleteByAddress(const String &sAddress)
   {
      auto iterRoute = vecObjects.begin();

      while (iterRoute != vecObjects.end())
      {  
         std::shared_ptr<RouteAddress> pRoute = (*iterRoute);

         if (pRoute->GetAddress().CompareNoCase(sAddress) == 0)
         {
            PersistentRouteAddress::DeleteObject(pRoute);
            vecObjects.erase(iterRoute);
            return;
         }

         iterRoute++;
      }
   }

   bool
   RouteAddresses::PreSaveObject(std::shared_ptr<RouteAddress> routeAddress, XNode *node)
   {
      routeAddress->SetRouteID(route_id_);

      return true;
   }

}
