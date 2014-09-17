// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "..\Threading\Task.h"

namespace HM
{
   class BackupTask : public Task
   {
   public:
      BackupTask(bool bDoBackup);
      ~BackupTask(void);

      virtual void DoWork();

      void SetBackupToRestore(std::shared_ptr<Backup> pBackup);

   private:

      bool do_backup_;
      
      std::shared_ptr<Backup> backup_;
   };
}