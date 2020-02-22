// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "Collection.h"

namespace HM
{
   class ServerMessage;
   class PersistentServerMessage;

   class ServerMessages : public Collection<ServerMessage, PersistentServerMessage>
   {
   public:
      ServerMessages();
      ~ServerMessages(void);

      // Refreshes this collection from the database.
      void Refresh();

      String GetMessage(const String &sName) const;
   
   protected:
      virtual String GetCollectionName() const {return "ServerMessages"; }
   private:
     
   };
}