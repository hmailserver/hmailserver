// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "BackupManager.h"

#include "Backup.h"

#include "ScriptingHost/ScriptServer.h"
#include "ScriptingHost/ScriptObjectContainer.h"
#include "ScriptingHost/Result.h"

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
      m_bIsRunning = false;
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
      if (m_bIsRunning)
      {
         LOG_DEBUG("BackupManager::~StartBackup() - E1");
         OnBackupFailed("Backup or restore operation is already started");
         return false;
      }

      m_bIsRunning = true;

      shared_ptr<BackupTask> pBackupTask = shared_ptr<BackupTask>(new BackupTask(true));

      shared_ptr<WorkQueue> pWorkQueue = Application::Instance()->GetRandomWorkQueue();
      if (!pWorkQueue)
      {
         m_bIsRunning = false;

         LOG_DEBUG("BackupManager::~StartBackup() - E2");
         OnBackupFailed("Backup operation failed because random work queue did not exist.");
         return false;
      }
      
      pWorkQueue->AddTask(pBackupTask);
      
      LOG_DEBUG("BackupManager::~StartBackup() - E3");

      return true;
   }


   bool
   BackupManager::StartRestore(shared_ptr<Backup> pBackup)
   {
      Logger::Instance()->LogBackup("Restore started");
      LOG_DEBUG("BackupManager::StartRestore()");

      // Start the backup thread, if we aren't
      // already running a backup or restore.
      if (m_bIsRunning)
      {
         OnBackupFailed("Backup or restore operation is already started");
         LOG_DEBUG("BackupManager::~StartRestore() - E1");
         return false;
      }

      m_bIsRunning = true;

      shared_ptr<BackupTask> pBackupTask = shared_ptr<BackupTask>(new BackupTask(false));
      pBackupTask->SetBackupToRestore(pBackup);
      
      Application::Instance()->GetRandomWorkQueue()->AddTask(pBackupTask);
      
      LOG_DEBUG("BackupManager::~StartRestore() - E2");
      return true;
   }

   shared_ptr<Backup>
   BackupManager::LoadBackup(const String &sZipFile) const
   {
      LOG_DEBUG("BackupManager::LoadBackup");

      shared_ptr<Backup> pResult = shared_ptr<Backup>(new Backup);

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
      m_bIsRunning = false;
   }

   void 
   BackupManager::SetStatus(const String &sStatus)
   {
      CriticalSectionScope scope(m_oStatusCritSec);
      m_sLog += sStatus + "\r\n";
   }

   String 
   BackupManager::GetStatus()
   {
      CriticalSectionScope scope(m_oStatusCritSec);
      String sVal = m_sLog;
      return sVal;
   }

   void 
   BackupManager::OnBackupCompleted()
   {
      LOG_DEBUG("BackupManager::OnBackupCompleted()");
      Logger::Instance()->LogBackup("Backup completed successfully");

      if (Configuration::Instance()->GetUseScriptServer())
      {
         shared_ptr<ScriptObjectContainer> pContainer = shared_ptr<ScriptObjectContainer>(new ScriptObjectContainer);
         shared_ptr<Result> pResult = shared_ptr<Result>(new Result);
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
         shared_ptr<ScriptObjectContainer> pContainer = shared_ptr<ScriptObjectContainer>(new ScriptObjectContainer);
         shared_ptr<Result> pResult = shared_ptr<Result>(new Result);
         pContainer->AddObject("Result", pResult, ScriptObject::OTResult);
         String sEventCaller = "OnBackupFailed(\"" + sReason + "\")";
         ScriptServer::Instance()->FireEvent(ScriptServer::EventOnBackupFailed, sEventCaller, pContainer);
      }
      LOG_DEBUG("BackupManager::~OnBackupFailed()");
   }


}