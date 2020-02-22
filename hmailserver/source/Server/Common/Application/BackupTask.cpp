// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include ".\backuptask.h"
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

   void
   BackupTask::DoWork()
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

      Application::Instance()->GetBackupManager()->OnThreadStopped();
   }


   void 
   BackupTask::SetBackupToRestore(std::shared_ptr<Backup> pBackup)
   {
      backup_ = pBackup;
   }
}