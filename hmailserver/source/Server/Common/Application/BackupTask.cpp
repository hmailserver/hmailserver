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
      BackupThreadStoppedNotifier notifier;

      try
      {
         BackupExecuter oBE;
         if (do_backup_)
         {
            oBE.StartBackup();
         }
         else
         {
            oBE.StartRestore(backup_);
         }
      }
      catch (boost::thread_interrupted&)
      {
         throw;
      }
      catch (std::exception& error)
      {
         Application::Instance()->GetBackupManager()->OnBackupFailed(error.what());
      }
      catch (...)
      {
         Application::Instance()->GetBackupManager()->OnBackupFailed("Unknown error.");
      }
   }


   void 
   BackupTask::SetBackupToRestore(std::shared_ptr<Backup> pBackup)
   {
      backup_ = pBackup;
   }
}