// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IncomingRelays.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IncomingRelays::IncomingRelays()
   {

   }

   IncomingRelays::~IncomingRelays()
   {

   }


   bool
   IncomingRelays::Refresh()
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // Refreshes the collection from the database.
   //---------------------------------------------------------------------------
   {
      String sSQL;
      sSQL.Format(_T("select * from hm_incoming_relays order by relayname asc"));

      return DBLoad_(sSQL);
   }

   bool
   IncomingRelays::IsIncomingRelay(const IPAddress &address) const
   {
      if (vecObjects.size() == 0)
      {
         // If no security ranges exist, default is deny.
         return false;
      }

      for(std::shared_ptr<IncomingRelay> relay : vecObjects)
      {
         if (relay->IPIsWithinRange(address))
         {
            return true;
         }
      }

      return false;
   }

}
