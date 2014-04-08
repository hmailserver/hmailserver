// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "Collection.h"
#include "TCPIPPort.h"

#include "../Persistence/PersistentTCPIPPort.h"

#include "../Cache/Cacheable.h"

namespace HM
{
   class TCPIPPorts : public Collection<TCPIPPort, PersistentTCPIPPort>
   {
   public:
      TCPIPPorts();
      ~TCPIPPorts(void);

      shared_ptr<TCPIPPort> GetPort(const IPAddress &iIPAddress, int iPort);

      void Refresh();
      // Refreshes this collection from the database.

      void SetDefault();
      // Generates a default set of items in this collection.

   protected:
      virtual String GetCollectionName() const {return "TCPIPPorts"; }

   private:
      shared_ptr<TCPIPPort> _GetPrimaryPort(SessionType st);
     
   };
}