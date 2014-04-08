// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Backup
   {
   public:
      Backup(void);
      ~Backup(void);

      enum BackupOptions
      {
         BOSettings = 1,
         BODomains = 2,
         BOMessages = 4,
         BOCompression = 8,
      };      

      void SetBackupFile(const String &sBackupFile) {m_sBackupFile = sBackupFile; }
      String GetBackupFile() {return m_sBackupFile; }

      void SetContains(int iContains) {m_iContains = iContains;}
      int GetContains() const {return m_iContains; }

      bool GetContains(int iOption) const;

      void SetRestoreOptions(int iRestoreOptions) {m_iRestoreOptions = iRestoreOptions; }
      int GetRestoreOptions() const {return m_iRestoreOptions; }
      
      int GetRestoreOption(int iRestoreOption) const;
      void SetRestoreOption(int iOption, bool bSet);
      
   private:

      int m_iContains;
      // Contains information on what parts that are backuped.
      
      int m_iRestoreOptions;
      // Contains the properties that should be restored when running a restore
      // For example, it may be a combination of settings, domains and compression.

      String m_sBackupFile;
      // The path to the XML backup file.
   };
}