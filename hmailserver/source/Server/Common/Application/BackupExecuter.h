// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once


namespace HM
{
   class Domain;
   class IMAPFolders;
   class IMAPFolder;
   class Messages;
   class Message;
   class BackupManager;

   class BackupExecuter
   {
   public:
      BackupExecuter();
      ~BackupExecuter(void);

      bool StartBackup();
      bool StartRestore(std::shared_ptr<Backup> pBackup);

   private:

      void LoadSettings_();

      bool BackupDomains_(XNode *pNode);
      bool BackupDataDirectory_(const String &sDataBackupDir);

      void RestoreDataDirectory_(std::shared_ptr<Backup> pBackup, XNode *pBackupNode);
      
      int backup_mode_;
      
      // Backup properties
      String destination_;
      String xmlfile_;
   };
}