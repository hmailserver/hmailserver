// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "Collection.h"

#include "Group.h"
#include "../Persistence/PersistentGroup.h"

namespace HM
{
   class Groups : public Collection<Group, PersistentGroup>
   {
   public:
      Groups();

      ~Groups(void);

      // Refreshes this collection from the database.
      void Refresh();

   protected:
      
      virtual String GetCollectionName() const {return "Groups"; }

   private:

   };
}