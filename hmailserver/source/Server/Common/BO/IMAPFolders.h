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

      shared_ptr<IMAPFolder> GetItemByDBIDRecursive(__int64 lFolderID);
      shared_ptr<IMAPFolder> GetFolderByName(const String &sName, bool bRecursive = false);
      shared_ptr<IMAPFolder> GetFolderByFullPath(const String &sPath);
      shared_ptr<IMAPFolder> GetFolderByFullPath(const std::vector<String> &vecFolders);
      
      void RemoveFolder(shared_ptr<IMAPFolder> pFolder);

      void CreatePath(shared_ptr<IMAPFolders> pParentContainer, const std::vector<String> &vecFolderPath, bool bAutoSubscribe);
      
      virtual bool PreSaveObject(shared_ptr<IMAPFolder> pObject, XNode *node);

      __int64 GetParentID();
      // Returns the ID of the IMAP folder in which these folder exists. If this is
      // a top level collection, -1 is returned.

      __int64 GetAccountID();
      // Returns the ID of the account in which these folders exist

   protected:
      virtual String GetCollectionName() const;

   private:

      bool _GetIsPublicFolders() const;

      __int64 m_iAccountID;
      __int64 m_iParentFolderID;
   };

}
