// Copyright (c) 2006 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "IMAPFolder.h"
#include "Collection.h"
#include "../Persistence/PersistentIMAPFolder.h"

namespace HM
{
   class IMAPFolders : public Collection<IMAPFolder, PersistentIMAPFolder>
   {
   public:
	   IMAPFolders(__int64 iAccountID, __int64 iParentFolderID);
      IMAPFolders();

	   virtual ~IMAPFolders();
      void Refresh();

      std::shared_ptr<IMAPFolder> GetItemByDBIDRecursive(__int64 lFolderID);
      std::shared_ptr<IMAPFolder> GetFolderByName(const String &sName, bool bRecursive = false);
      std::shared_ptr<IMAPFolder> GetFolderByFullPath(const String &sPath);
      std::shared_ptr<IMAPFolder> GetFolderByFullPath(const std::vector<String> &vecFolders);
      
      void RemoveFolder(std::shared_ptr<IMAPFolder> pFolder);

      void CreatePath(std::shared_ptr<IMAPFolders> pParentContainer, const std::vector<String> &vecFolderPath, bool bAutoSubscribe);
      
      virtual bool PreSaveObject(std::shared_ptr<IMAPFolder> pObject, XNode *node);

      __int64 GetParentID();
      // Returns the ID of the IMAP folder in which these folder exists. If this is
      // a top level collection, -1 is returned.

      __int64 GetAccountID();
      // Returns the ID of the account in which these folders exist

   protected:
      virtual String GetCollectionName() const;

   private:

      bool GetIsPublicFolders_() const;

      __int64 account_id_;
      __int64 parent_folder_id_;
   };

}
