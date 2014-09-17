// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "Collection.h"

#include "ACLPermission.h"
#include "../Persistence/PersistentACLPermission.h"

namespace HM
{
   class ACLPermissions : public Collection<ACLPermission, PersistentACLPermission>
   {
   public:
      ACLPermissions();
      ACLPermissions(__int64 iFolderID);
      ~ACLPermissions(void);

      // Refreshes this collection from the database.
      void Refresh();

      std::shared_ptr<ACLPermission> GetPermissionForAccount(__int64 iAccountID);
      std::shared_ptr<ACLPermission> GetPermissionForGroup(__int64 iGroupID);
      bool DeletePermissionsForAccount(__int64 iAccountID);

      bool PreSaveObject(std::shared_ptr<ACLPermission> pPermission, XNode *pNode);
   protected:
      
      virtual String GetCollectionName() const {return "ACLs"; }

   private:
 
      __int64 folder_id_;
   };
}