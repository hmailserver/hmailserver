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
      m_iRouteID(iRouteID)
   {
      
   }

   RouteAddresses::~RouteAddresses()
   {

   }

   void 
   RouteAddresses::Refresh()
   {
      String sSQL;
      sSQL.Format(_T("select * from hm_routeaddresses where routeaddressrouteid = %I64d"), m_iRouteID);;

      _DBLoad(sSQL);
   }

   void
   RouteAddresses::DeleteByAddress(const String &sAddress)
   {
      std::vector<shared_ptr<RouteAddress> >::iterator iterRoute = vecObjects.begin();

      while (iterRoute != vecObjects.end())
      {  
         shared_ptr<RouteAddress> pRoute = (*iterRoute);

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
   RouteAddresses::PreSaveObject(shared_ptr<RouteAddress> routeAddress, XNode *node)
   {
      routeAddress->SetRouteID(m_iRouteID);

      return true;
   }

}
