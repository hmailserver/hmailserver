// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class RouteAddress;

   class PersistentRouteAddress
   {
   public:
	   PersistentRouteAddress();
	   virtual ~PersistentRouteAddress();

      static bool SaveObject(shared_ptr<RouteAddress> pRA);
      static bool SaveObject(shared_ptr<RouteAddress> pRA, String &errorMessage);
      static bool DeleteObject(shared_ptr<RouteAddress> pRA);
      static bool DeleteByRoute(__int64 RouteID);

      static bool ReadObject(shared_ptr<RouteAddress> pRA, long lID);
      static bool ReadObject(shared_ptr<RouteAddress> pRA, shared_ptr<DALRecordset> pRS);


   };

}
