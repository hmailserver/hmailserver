// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class PropertySet;
   class Settings;
   class IMAPFolders;
   class Groups;

   class IMAPConfiguration  
   {
   public:
	   IMAPConfiguration();
	   virtual ~IMAPConfiguration();

      bool Load();

      String GetWelcomeMessage() const;
      void SetWelcomeMessage(const String &sMessage);

      bool GetUseIMAPQuota() const;
      void SetUseIMAPQuota(bool bValue);

      bool GetUseIMAPIdle() const;
      void SetUseIMAPIdle(bool bValue);

      bool GetUseIMAPSort() const;
      void SetUseIMAPSort(bool bValue);

      bool GetUseIMAPACL() const;
      void SetUseIMAPACL(bool bValue);

      bool GetUseIMAPSASLPlain() const;
      void SetUseIMAPSASLPlain(bool bValue);

      bool GetUseIMAPSASLInitialResponse() const;
      void SetUseIMAPSASLInitialResponse(bool bValue);

      long GetMaxIMAPConnections() const;
      void SetMaxIMAPConnections(int newVal);

      String GetIMAPPublicFolderName() const;
      void SetIMAPPublicFolderName(const String &newVal);

      String GetIMAPMasterUser() const;
      void SetIMAPMasterUser(const String &newVal);

      static String GetPublicFolderDiskName();
      std::shared_ptr<IMAPFolders> GetPublicFolders();
      std::shared_ptr<Groups> GetGroups();

      bool XMLStore(XNode *pBackupNode, int Options);
      bool XMLLoad(XNode *pBackupNode, int iRestoreOptions);

      String GetHierarchyDelimiter();
      bool SetHierarchyDelimiter(const String &newVal);

   private:
      std::shared_ptr<PropertySet> GetSettings_() const;

      std::shared_ptr<IMAPFolders> public_folders_;
      std::shared_ptr<Groups> groups_;
   };

}
