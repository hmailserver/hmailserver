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
      m_bDoBackup(bDoBackup)
   {
   }

   BackupTask::~BackupTask(void)
   {
   }

   void
   BackupTask::DoWork()
   {
      BackupExecuter oBE;
      if (m_bDoBackup)
      {
         oBE.StartBackup();
      }
      else
      {
         oBE.StartRestore(m_pBackup);
      }

      Application::Instance()->GetBackupManager()->OnThreadStopped();
   }

   void 
   BackupTask::StopWork()
   {
      
      
   }

   void 
   BackupTask::SetBackupToRestore(shared_ptr<Backup> pBackup)
   {
      m_pBackup = pBackup;
   }
}