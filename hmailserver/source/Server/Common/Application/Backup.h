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

      void SetBackupFile(const String &sBackupFile) {backup_file_ = sBackupFile; }
      String GetBackupFile() {return backup_file_; }

      void SetContains(int iContains) {contains_ = iContains;}
      int GetContains() const {return contains_; }

      bool GetContains(int iOption) const;

      void SetRestoreOptions(int iRestoreOptions) {restore_options_ = iRestoreOptions; }
      int GetRestoreOptions() const {return restore_options_; }
      
      int GetRestoreOption(int iRestoreOption) const;
      void SetRestoreOption(int iOption, bool bSet);
      
   private:

      int contains_;
      // Contains information on what parts that are backuped.
      
      int restore_options_;
      // Contains the properties that should be restored when running a restore
      // For example, it may be a combination of settings, domains and compression.

      String backup_file_;
      // The path to the XML backup file.
   };
}