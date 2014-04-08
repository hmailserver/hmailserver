// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{

   class IMAPFolder;
   class ACL;

   class PersistentIMAPFolder
   {
   private:
	   PersistentIMAPFolder();
	   virtual ~PersistentIMAPFolder();
   public:

      static bool DeleteObject(shared_ptr<IMAPFolder> pFolder);
      static bool DeleteObject(shared_ptr<IMAPFolder> pFolder, bool forceDelete);
      static bool SaveObject(shared_ptr<IMAPFolder> pFolder, String &errorMessage);
      static bool SaveObject(shared_ptr<IMAPFolder> pFolder);
      static bool DeleteByAccount(__int64 iAccountID);

      static bool GetExistsFolderContainingCharacter(String theChar);

      static unsigned int GetUniqueMessageID(__int64 accountID, __int64 folderID);

      static __int64 GetUserInboxFolder(__int64 accountID);

   private:

      static bool _IncreaseCurrentUID(__int64 folderID);
      static unsigned int _GetCurrentUID(__int64 folderID);


   };
}
