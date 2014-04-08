// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com


#include "stdafx.h"

#include "Application.h"

#include "BackupManager.h"
#include "Scheduler.h"
#include "OutOfMemoryHandler.h"
#include "ACLManager.h"
#include <winerror.h>

#include "MessageIndexer.h"

#include "FolderManager.h"
#include "../Cache/CacheContainer.h"

#include "../AntiSpam/SpamProtection.h"
#include "../AntiVirus/VirusScanner.h"

#include "../SQL/DALConnection.h"
#include "../SQL/DALConnectionFactory.h"
#include "../SQL/DatabaseConnectionManager.h"

#include "../Util/Time.h"
#include "../Util/ServerStatus.h"
#include "../Util/Languages.h"
#include "../Util/Utilities.h"
#include "../Mime/MimeCode.h"

#include "Property.h"
#include "Version.h"

#include "../../SMTP/SMTPDeliveryManager.h"
#include "../../IMAP/IMAPConfiguration.h"
#include "../../IMAP/IMAPFolderContainer.h"
#include "../../ExternalFetcher/ExternalFetchManager.h"

#include "../Threading/WorkQueueManager.h"
#include "../Tracking/NotificationServer.h"
#include "SessionManager.h"

#include "../../SMTP/GreyListCleanerTask.h"
#include "../TCPIP/IOCPServer.h"

#include "../BO/Message.h"
#include "../BO/Domain.h"
#include "../Persistence/PersistentMessage.h"
#include "../Persistence/PersistentDomain.h"
#include "RemoveExpiredRecords.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif



namespace HM
{
   Application::Application() :
      m_sServerWorkQueue("Server queue"),
      m_sRandomWorkQueue("Random queue"),
      m_sAsynchronousTasksQueue("Aynchronous task queue"),
      m_iUniqueID(0)
   {
      m_sProdName = _T("hMailServer");
      m_sVersion = Formatter::Format("{0}-B{1}", HMAILSERVER_VERSION, HMAILSERVER_BUILD);
      m_sStartTime = Time::GetCurrentDateTime();
   }

   String
   Application::GetVersion() const
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Returns the version of this application.
   //---------------------------------------------------------------------------()
   {
      String sVersion;
      sVersion.Format(_T("%s %s"), m_sProdName, m_sVersion);

      return sVersion;
   }

   Application::~Application()
   {

   }


   bool
   Application::InitInstance(String &sErrorMessage)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Application initializing stuff.
   //---------------------------------------------------------------------------()
   {
      VirusScanner::ResetCounter();

      OutOfMemoryHandler::Initialize();

      MimeEnvironment::SetAutoFolding(true);

      ServerStatus::Instance()->SetState(ServerStatus::StateStopped);

      // Initialize objects that should always exist, regardless of anything.
      IniFileSettings::Instance()->LoadSettings();

      if (!IniFileSettings::Instance()->GetDatabaseSettingsExists())
      {
         return false;
      }

      // Start a random thread queue that can run different 
      // types of background tasks, such as backups etc.
      WorkQueueManager::Instance()->CreateWorkQueue(100, m_sRandomWorkQueue, WorkQueue::eQTRandom);

      // Language needed for COM API
      Languages::Instance()->Load();

      // Create the backup manager that manages backup tasks...
      m_pBackupManager = shared_ptr<BackupManager>(new BackupManager);

      LOG_DEBUG("Application::InitInstance - Connecting to database...");
      if (!OpenDatabase(sErrorMessage))
      {
         // We could not open the database. We can just
         // as well stop now since there's nothing to do.
         return false;
      }
      
      if (!OnDatabaseConnected(sErrorMessage))
      {
         // This database is not compatible. 
         sErrorMessage = "Database initialization failed.";
         ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5010, "Application::InitInstance", sErrorMessage);
         return false;
      }

      // Load the caches...
      LOG_DEBUG("Application::InitInstance - Creating caches...");
      CacheContainer::Instance()->CreateCaches();

      LOG_DEBUG("Application::InitInstance - Loading configuration...");
      if (!Configuration::Instance()->Load())
         return false;

      LOG_DEBUG("Application::InitInstance - Configuration loaded.");

      // Start an asynch workqueue which processes asynchronous tasks from clients.
      WorkQueueManager::Instance()->CreateWorkQueue(Configuration::Instance()->GetAsynchronousThreads(), 
                                                    m_sAsynchronousTasksQueue, WorkQueue::eQTFixedSize);

      return true;
   }

   bool
   Application::OpenDatabase(String &sErrorMessage)
   {
      String sIniFileCheckError;
      if (!IniFileSettings::Instance()->CheckSettings(sIniFileCheckError))
      {
         String sError;
         sError.Format(_T("Loading of ini file settings failed. Error: %s"), sIniFileCheckError);

         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5005, "Application::OpenDatabase", sError);
         return false;
      }

      m_pDBManager = shared_ptr<DatabaseConnectionManager>(new DatabaseConnectionManager);
      bool bConnectedSuccessfully = m_pDBManager->CreateConnections(sErrorMessage);

      m_sLastConnectErrorMessage = sErrorMessage;

      if (!bConnectedSuccessfully)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Critical, 4354, "Application::OpenDatabase", "hMailServer failed to connect to the database server. Error message: " + sErrorMessage);
         return false;
      }

      return true;
   }


   bool
   Application::OnDatabaseConnected(String &sErrorMessage)
   {
      LOG_DEBUG("Application::OnDatabaseConnected()");

      int iDBVersion = GetDBManager()->GetCurrentDatabaseVersion();

      if (iDBVersion == 0)
      {
         sErrorMessage = "Database version could not be detected.";
         m_sLastConnectErrorMessage = sErrorMessage;
         return false;
      }

      if (iDBVersion < Configuration::Instance()->GetRequiredDBVersion() 
          || iDBVersion > Configuration::Instance()->GetRequiredDBVersion())
      {
         String sErrorMessage;

         if (iDBVersion > Configuration::Instance()->GetRequiredDBVersion())
            sErrorMessage = "The database is too new for this version of hMailServer. Please upgrade hMailServer.";
         else
            sErrorMessage = "The database is too old for this version of hMailServer. Please run hMailServer Database updater (DBUpdater.exe) to upgrade it.";

         String sVersionInfo;
         sVersionInfo.Format(_T(" Database version: %d, Required database version: %d"), iDBVersion, Configuration::Instance()->GetRequiredDBVersion());

         sErrorMessage += sVersionInfo;
         
         m_sLastConnectErrorMessage = sErrorMessage;

         ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5011, "Application::OnDatabaseConnected", sErrorMessage);

         return false;
      }

      LOG_DEBUG("Application::~OnDatabaseConnected() - E2");
      return true;
   }

   bool
   Application::ExitInstance()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Do uninitialization.
   //---------------------------------------------------------------------------()
   {
      // Close work queue
      WorkQueueManager::Instance()->RemoveQueue(m_sRandomWorkQueue);

      WorkQueueManager::Instance()->RemoveQueue(m_sAsynchronousTasksQueue);

      // Backup manager is created by initinstance so should be destroyed here.
      if (m_pBackupManager) 
         m_pBackupManager.reset();

      LOG_DEBUG("Application::ExitInstance - Closing database connection...");
      CloseDatabase();

      LOG_DEBUG("Application::ExitInstance - Deleting caches...");
      CacheContainer::Instance()->DeleteCaches();

      MimeEnvironment::SetAutoFolding(false);

      OutOfMemoryHandler::Terminate();
 
      return 0;
    
   }

   void
   Application::CloseDatabase()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Close the database connection.
   //---------------------------------------------------------------------------()
   {
      if (m_pDBManager)
      {
         try
         {
            m_pDBManager->Disconnect();
         }
         catch (...)
         {

         }

         m_pDBManager.reset();
      }
   }

   String
   Application::GetExecutableName()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Returns the name of the executable (h*.exe)
   //---------------------------------------------------------------------------()
   {
      LPTSTR szPath = (LPTSTR)alloca( 2048 );
      DWORD  dwPathLength;
   
      HMODULE handle = GetModuleHandle(_T("hmailserver.exe"));
      dwPathLength = GetModuleFileName(handle, szPath, 2048 );
 
      szPath[ dwPathLength ] = 0; // --- nullterminated.
   
      String sPath(szPath);
      return sPath;
   }

   bool
   Application::StartServers()
   {
      LOG_APPLICATION("Starting servers...")

      if (ServerStatus::Instance()->GetState() != ServerStatus::StateStopped)
      {
         LOG_DEBUG("Application::~StartServers() - E1");
         return false;
      }

      ServerStatus::Instance()->SetState(ServerStatus::StateStarting);

      SpamProtection::Instance()->Load();

      // Create queue for server tasks, such as responsing to SMTP and 
      // POP3 and IMAP connections
      m_pIOCPServer = shared_ptr<IOCPServer>(new IOCPServer);

      _RegisterSessionTypes();

      // Create the main work queue.
      int iMainServerQueue = WorkQueueManager::Instance()->CreateWorkQueue(4, m_sServerWorkQueue, WorkQueue::eQTPreLoad);

      m_pNotificationServer = shared_ptr<NotificationServer>(new NotificationServer());
      _folderManager = shared_ptr<FolderManager>(new FolderManager());
      
      // Create the scheduler. This is always in use.
      m_pScheduler = shared_ptr<Scheduler>(new Scheduler);
      WorkQueueManager::Instance()->AddTask(iMainServerQueue, m_pScheduler);

      // Always run the IOCP server.
      WorkQueueManager::Instance()->AddTask(iMainServerQueue, m_pIOCPServer);

      // Always run delivery manager. Software useless without it.
      m_pSMTPDeliveryManager = shared_ptr<SMTPDeliveryManager>(new SMTPDeliveryManager);
      WorkQueueManager::Instance()->AddTask(iMainServerQueue, m_pSMTPDeliveryManager);

      // ... and the external account fetch manager.
      m_pExternalFetchManager = shared_ptr<ExternalFetchManager> (new ExternalFetchManager);
      WorkQueueManager::Instance()->AddTask(iMainServerQueue, m_pExternalFetchManager);

      _CreateScheduledTasks();

      if (Configuration::Instance()->GetMessageIndexing())
      {
         MessageIndexer::Start();
      }

      ServerStatus::Instance()->SetState(ServerStatus::StateRunning);

      m_sStartTime = Time::GetCurrentDateTime();

      // Wait for the IOCP server to signal start-up.
      _serverStartEvent.Wait();
      _serverStartEvent.Reset();

      LOG_APPLICATION("Servers started.")

      return true;
   }

   void 
   Application::_RegisterSessionTypes()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Registers the different session types...
   //---------------------------------------------------------------------------()
   {

      // Start SMTP server and delivery threads.
      if (Configuration::Instance()->GetUseSMTP())
      {
         m_pIOCPServer->RegisterSessionType(STSMTP);
      }

      if (Configuration::Instance()->GetUsePOP3())
      {
         m_pIOCPServer->RegisterSessionType(STPOP3);
      }

      if (Configuration::Instance()->GetUseIMAP())
      {
         m_pIOCPServer->RegisterSessionType(STIMAP);
      }
   }

   void 
   Application::_CreateScheduledTasks()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Create scheduled tasks.
   //---------------------------------------------------------------------------()
   {
      if (Configuration::Instance()->GetUseSMTP())
      {
         shared_ptr<GreyListCleanerTask> pCleanerTask = shared_ptr<GreyListCleanerTask>(new GreyListCleanerTask);
         pCleanerTask->SetReoccurance(ScheduledTask::RunInfinitely);
         pCleanerTask->SetMinutesBetweenRun(IniFileSettings::Instance()->GetGreylistingExpirationInterval());
         m_pScheduler->ScheduleTask(pCleanerTask);
      }

      // cleaning of expired IP ranges.
      shared_ptr<RemoveExpiredRecords> removeExpiredRecordsTask = shared_ptr<RemoveExpiredRecords>(new RemoveExpiredRecords);
      removeExpiredRecordsTask->SetReoccurance(ScheduledTask::RunInfinitely);
      removeExpiredRecordsTask->SetMinutesBetweenRun(1);
      m_pScheduler->ScheduleTask(removeExpiredRecordsTask);

   }

   void 
   Application::StopServers()
   {
      LOG_APPLICATION("Stopping servers...")

      m_sStartTime = "";

      if (ServerStatus::Instance()->GetState() != ServerStatus::StateRunning)
      {
         LOG_DEBUG("Application::~StopServers() - E1");
         return;
      }

      ServerStatus::Instance()->SetState(ServerStatus::StateStopping);

      // Then remove the main server.
      WorkQueueManager::Instance()->RemoveQueue(m_sServerWorkQueue);

      // Unload the message list cache.
      IMAPFolderContainer::Instance()->Clear();

      // Deinitialize servers
      if (m_pSMTPDeliveryManager) m_pSMTPDeliveryManager.reset();
      if (m_pIOCPServer) m_pIOCPServer.reset();
      if (m_pExternalFetchManager) m_pExternalFetchManager.reset();
      if (m_pScheduler) m_pScheduler.reset();
      
      if (m_pNotificationServer) m_pNotificationServer.reset();
      if (_folderManager) _folderManager.reset();

      ServerStatus::Instance()->SetState(ServerStatus::StateStopped);

      LOG_APPLICATION("Servers stopped.")

   }

   String
   Application::Reinitialize()
   {
      StopServers();
      ExitInstance();

      String sErrorMessage;
      if (!InitInstance(sErrorMessage))
         return sErrorMessage;

      if (!StartServers())
         return "Failed to start servers";

      return "";
   }

   void
   Application::SubmitPendingEmail()
   {
      LOG_DEBUG("Requesting SMTPDeliveryManager to start message delivery");

      if (m_pSMTPDeliveryManager)
         m_pSMTPDeliveryManager->SetDeliverMessage();
      else
         // We could not notify the SMTP delivery manager, since it does not exit.
         ErrorManager::Instance()->ReportError(ErrorManager::High, 4219, "Application::SubmitPendingEmail", "Could not notify SMTP deliverer about new message, since SMTP deliverer does not exist. The operation requires the SMTP server to be on.");
   }

   shared_ptr<WorkQueue>
   Application::GetRandomWorkQueue()
   {
      shared_ptr<WorkQueue> pWorkQueue;
      
      for (int i = 0; i < 10; i++)
      {
         pWorkQueue = WorkQueueManager::Instance()->GetQueue(m_sRandomWorkQueue);

         if (pWorkQueue)
            break;

         Sleep(2000);
      }
      
      if (!pWorkQueue)
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5118, "Application::GetRandomWorkQueue()", "Random work queue not available.");

      return pWorkQueue;

      
   }

   shared_ptr<WorkQueue>
   Application::GetAsyncWorkQueue()
   {
      shared_ptr<WorkQueue> pAsynchQueue = WorkQueueManager::Instance()->GetQueue(m_sAsynchronousTasksQueue);

      if (!pAsynchQueue)
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5118, "Application::GetAsyncWorkQueue()", "Async work queue not available.");

      return pAsynchQueue;
   }

   int 
   Application::GetUniqueID()
   {
      int iResult = InterlockedIncrement(&m_iUniqueID);

      return iResult;
   }

   void 
   Application::SetServerStartedEvent()
   {
      _serverStartEvent.Set();
   }

   void
   Application::OnPropertyChanged(shared_ptr<Property> pProperty)
   {
      if (ServerStatus::Instance()->GetState() == ServerStatus::StateStopped)
         return;

      String sPropertyName = pProperty->GetName();

      if (sPropertyName == PROPERTY_MAX_NUMBER_OF_ASYNC_TASKS)
      {
         shared_ptr<WorkQueue> pWorkQueue = GetAsyncWorkQueue();

         if (!pWorkQueue)
            return;

         pWorkQueue->SetMaxSimultaneous(pProperty->GetLongValue());
      }
   }

   shared_ptr<NotificationServer> 
   Application::GetNotificationServer()
   {
      return m_pNotificationServer;
   }

   shared_ptr<FolderManager> 
   Application::GetFolderManager()
   {
      return _folderManager;
   }
}
