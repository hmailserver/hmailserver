// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "BackupManager.h"

#include "Backup.h"

#include "../Scripting/ScriptServer.h"
#include "../Scripting/ScriptObjectContainer.h"
#include "../Scripting/Result.h"

#include "..\Util\Compression.h"

#include "../Threading/WorkQueueManager.h"
#include "BackupTask.h"


#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   BackupManager::BackupManager(void)
   {
      is_running_ = false;
   }

   BackupManager::~BackupManager(void)
   {
   }


   bool 
   BackupManager::StartBackup()
   {
      Logger::Instance()->LogBackup("Backup started");
      LOG_DEBUG("BackupManager::StartBackup()");

      // Start the backup thread, if we aren't
      // already running a backup or restore.
      if (is_running_)
      {
         LOG_DEBUG("BackupManager::~StartBackup() - E1");
         OnBackupFailed("Backup or restore operation is already started");
         return false;
      }

      is_running_ = true;

      std::shared_ptr<BackupTask> pBackupTask = std::shared_ptr<BackupTask>(new BackupTask(true));

      std::shared_ptr<WorkQueue> pWorkQueue = Application::Instance()->GetMaintenanceWorkQueue();
      if (!pWorkQueue)
      {
         is_running_ = false;

         LOG_DEBUG("BackupManager::~StartBackup() - E2");
         OnBackupFailed("Backup operation failed because random work queue did not exist.");
         return false;
      }
      
      pWorkQueue->AddTask(pBackupTask);
      
      LOG_DEBUG("BackupManager::~StartBackup() - E3");

      return true;
   }


   bool
   BackupManager::StartRestore(std::shared_ptr<Backup> pBackup)
   {
      Logger::Instance()->LogBackup("Restore started");
      LOG_DEBUG("BackupManager::StartRestore()");

      // Start the backup thread, if we aren't
      // already running a backup or restore.
      if (is_running_)
      {
         OnBackupFailed("Backup or restore operation is already started");
         LOG_DEBUG("BackupManager::~StartRestore() - E1");
         return false;
      }

      is_running_ = true;

      std::shared_ptr<BackupTask> pBackupTask = std::shared_ptr<BackupTask>(new BackupTask(false));
      pBackupTask->SetBackupToRestore(pBackup);
      
      Application::Instance()->GetMaintenanceWorkQueue()->AddTask(pBackupTask);
      
      LOG_DEBUG("BackupManager::~StartRestore() - E2");
      return true;
   }

   std::shared_ptr<Backup>
   BackupManager::LoadBackup(const String &sZipFile) const
   {
      LOG_DEBUG("BackupManager::LoadBackup");

      std::shared_ptr<Backup> pResult = std::shared_ptr<Backup>(new Backup);

      // First uncompress our specification file.
      String sTempDir = IniFileSettings::Instance()->GetTempDirectory();
      String sXMLFile = sTempDir + "\\hMailServerBackup.xml";
      FileUtilities::DeleteFile(sXMLFile);
      Compression oComp;
      oComp.Uncompress(sZipFile, sTempDir, "hMailServerBackup.xml");

      // Load the XML document
      String sXMLBuffer = FileUtilities::ReadCompleteTextFile(sXMLFile);
      XDoc oXMLDocument;
      oXMLDocument.Load(sXMLBuffer);

      XNode *pBackupNode = oXMLDocument.GetChild(_T("Backup"));
      if (!pBackupNode)
      {
         LOG_DEBUG("BackupManager::~LoadBackup - E1");
         return pResult;
      }

      int iMode = _ttoi(pBackupNode->GetChildAttr(_T("BackupInformation"), _T("Mode"))->value);

      pResult->SetBackupFile(sZipFile);
      pResult->SetContains(iMode);

      // Delete the XML file we temporarly created
      FileUtilities::DeleteFile(sXMLFile);

      LOG_DEBUG("BackupManager::~LoadBackup - E2");
      return pResult;
   }

   void 
   BackupManager::OnThreadStopped()
   {
      is_running_ = false;
   }

   void 
   BackupManager::SetStatus(const String &sStatus)
   {
      boost::lock_guard<boost::recursive_mutex> guard(mutex_);
      log_ += sStatus + "\r\n";
   }

   String 
   BackupManager::GetStatus()
   {
      boost::lock_guard<boost::recursive_mutex> guard(mutex_);
      String sVal = log_;
      return sVal;
   }

   void 
   BackupManager::OnBackupCompleted()
   {
      LOG_DEBUG("BackupManager::OnBackupCompleted()");
      Logger::Instance()->LogBackup("Backup completed successfully");

      if (Configuration::Instance()->GetUseScriptServer())
      {
         std::shared_ptr<ScriptObjectContainer> pContainer = std::shared_ptr<ScriptObjectContainer>(new ScriptObjectContainer);
         std::shared_ptr<Result> pResult = std::shared_ptr<Result>(new Result);
         pContainer->AddObject("Result", pResult, ScriptObject::OTResult);
         String sEventCaller = "OnBackupCompleted()";
         ScriptServer::Instance()->FireEvent(ScriptServer::EventOnBackupCompleted, sEventCaller, pContainer);
      }
      LOG_DEBUG("BackupManager::~OnBackupCompleted()");
   }

   void 
   BackupManager::OnBackupFailed(const String &sReason)
   {
      LOG_DEBUG("BackupManager::OnBackupFailed()");
      String sErrorMsg = "BACKUP ERROR: " + sReason;
      Logger::Instance()->LogBackup(sErrorMsg);

      ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5014, "BackupManager::OnBackupFailed", sErrorMsg);

      if (Configuration::Instance()->GetUseScriptServer())
      {
         std::shared_ptr<ScriptObjectContainer> pContainer = std::shared_ptr<ScriptObjectContainer>(new ScriptObjectContainer);
         std::shared_ptr<Result> pResult = std::shared_ptr<Result>(new Result);
         pContainer->AddObject("Result", pResult, ScriptObject::OTResult);
         String sEventCaller = "OnBackupFailed(\"" + sReason + "\")";
         ScriptServer::Instance()->FireEvent(ScriptServer::EventOnBackupFailed, sEventCaller, pContainer);
      }
      LOG_DEBUG("BackupManager::~OnBackupFailed()");
   }


}