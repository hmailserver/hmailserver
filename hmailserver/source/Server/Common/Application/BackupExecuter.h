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
      bool StartRestore(shared_ptr<Backup> pBackup);

   private:

      void _LoadSettings();

      bool _BackupDomains(XNode *pNode);
      bool _BackupDataDirectory(const String &sDataBackupDir);

      void _RestoreDataDirectory(shared_ptr<Backup> pBackup, XNode *pBackupNode);
      
      int m_iBackupMode;
      
      // Backup properties
      String m_sDestination;
      String m_sXMLFile;
   };
}