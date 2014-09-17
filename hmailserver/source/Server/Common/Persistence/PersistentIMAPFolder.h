// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{

   class IMAPFolder;
   class ACL;
   enum PersistenceMode;

   class PersistentIMAPFolder
   {
   private:
	   PersistentIMAPFolder();
	   virtual ~PersistentIMAPFolder();
   public:

      static bool DeleteObject(std::shared_ptr<IMAPFolder> pFolder);
      static bool DeleteObject(std::shared_ptr<IMAPFolder> pFolder, bool forceDelete);
      static bool SaveObject(std::shared_ptr<IMAPFolder> pFolder, String &errorMessage, PersistenceMode mode);
      static bool SaveObject(std::shared_ptr<IMAPFolder> pFolder);
      static bool DeleteByAccount(__int64 iAccountID);

      static bool GetExistsFolderContainingCharacter(String theChar);

      static unsigned int GetUniqueMessageID(__int64 accountID, __int64 folderID);

      static __int64 GetUserInboxFolder(__int64 accountID);

   private:

      static bool IncreaseCurrentUID_(__int64 folderID);
      static unsigned int GetCurrentUID_(__int64 folderID);


   };
}
