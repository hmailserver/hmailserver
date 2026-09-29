// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "./BackupTask.h"
#include "BackupExecuter.h"
#include "BackupManager.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   BackupTask::BackupTask(bool bDoBackup) :
      Task("BackupTask"),
      do_backup_(bDoBackup)
   {
   }

   BackupTask::~BackupTask(void)
   {
   }

   namespace
   {
      // Tells the backup manager that the task is done, whichever way it ends.
      struct BackupThreadStoppedNotifier
      {
         ~BackupThreadStoppedNotifier()
         {
            Application::Instance()->GetBackupManager()->OnThreadStopped();
         }
      };
   }

   void
   BackupTask::DoWork()
   {
      bool succeeded = false;
      String errorMessage;

      {
         BackupThreadStoppedNotifier notifier;

         try
         {
            BackupExecuter oBE;
            if (do_backup_)
               succeeded = oBE.StartBackup(errorMessage);
            else
               succeeded = oBE.StartRestore(backup_, errorMessage);
         }
         catch (boost::thread_interrupted&)
         {
            throw;
         }
         catch (std::exception& error)
         {
            errorMessage = error.what();
         }
         catch (...)
         {
            errorMessage = "Unknown error.";
         }
      }

      // Report the result only after the backup manager knows the task has stopped.
      // Otherwise, a backup started as soon as the result is logged is refused as already started.
      auto backupManager = Application::Instance()->GetBackupManager();

      if (!succeeded)
         backupManager->OnBackupFailed(errorMessage);
      else if (do_backup_)
         backupManager->OnBackupCompleted();
      else
         Logger::Instance()->LogBackup("Restore completed successfully.");
   }


   void 
   BackupTask::SetBackupToRestore(std::shared_ptr<Backup> pBackup)
   {
      backup_ = pBackup;
   }
}