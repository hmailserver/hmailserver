// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "Collection.h"

#include "../Persistence/PersistentRouteAddress.h"
#include "RouteAddress.h"

namespace HM
{
   class RouteAddresses : public Collection<RouteAddress, PersistentRouteAddress>
   {
   public:
	   RouteAddresses(__int64 iRouteID);
	   virtual ~RouteAddresses();

      void Refresh();

      void DeleteByAddress(const String &sAddress);

      __int64 GetRouteID() {return route_id_; }
      
   protected:
      virtual String GetCollectionName() const {return "RouteAddresses"; }
      bool PreSaveObject(std::shared_ptr<RouteAddress> routeAddress, XNode *node);

   private:
      __int64 route_id_;
   };

}
