// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "Collection.h"

#include "../Persistence/PersistentRoute.h"
#include "Route.h"

#include "../Cache/Cacheable.h"

namespace HM
{
   class Routes : public Collection<Route, PersistentRoute>
   {
   public:
	   Routes();
	   virtual ~Routes();

      // Refreshes this collection from the database.
      void Refresh();

      vector<shared_ptr<Route> > GetItemsByName(const String &sRouteName);

   protected:

      virtual String GetCollectionName() const {return "Routes"; }

   };

}
