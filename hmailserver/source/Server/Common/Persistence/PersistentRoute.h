// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Route;

   class PersistentRoute
   {
   public:
	   PersistentRoute();
	   virtual ~PersistentRoute();

      static bool DeleteObject(shared_ptr<Route> pRoute);

      static bool SaveObject(shared_ptr<Route> pRoute);
      static bool SaveObject(shared_ptr<Route> pRoute, String &sErrorMessage);
      static bool ReadObject(shared_ptr<Route> pRoute, long lID);
      static bool ReadObject(shared_ptr<Route> pRoute, shared_ptr<DALRecordset> pRS);
   };
   
}

