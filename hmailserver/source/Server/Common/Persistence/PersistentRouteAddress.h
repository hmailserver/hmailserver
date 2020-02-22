// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class RouteAddress;
   enum PersistenceMode;

   class PersistentRouteAddress
   {
   public:
	   PersistentRouteAddress();
	   virtual ~PersistentRouteAddress();

      static bool SaveObject(std::shared_ptr<RouteAddress> pRA);
      static bool SaveObject(std::shared_ptr<RouteAddress> pRA, String &errorMessage, PersistenceMode mode);
      static bool DeleteObject(std::shared_ptr<RouteAddress> pRA);
      static bool DeleteByRoute(__int64 RouteID);

      static bool ReadObject(std::shared_ptr<RouteAddress> pRA, long lID);
      static bool ReadObject(std::shared_ptr<RouteAddress> pRA, std::shared_ptr<DALRecordset> pRS);


   };

}
