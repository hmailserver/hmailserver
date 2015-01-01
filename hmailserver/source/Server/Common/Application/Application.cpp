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
#include "../../ExternalFetcher/ExternalFetchManager.h"

#include "../Threading/WorkQueueManager.h"
#include "../Tracking/NotificationServer.h"
#include "SessionManager.h"

#include "../../SMTP/GreyListCleanerTask.h"
#include "../TCPIP/IOService.h"

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
      server_work_queue_("Server queue"),
      maintenance_queue_("Maintenance queue"),
      asynchronous_tasks_queue_("Asynchronous task queue"),
      unique_id_(0)
   {
      version_ = Formatter::Format("{0}-B{1}", HMAILSERVER_VERSION, HMAILSERVER_BUILD);
      start_time_ = Time::GetCurrentDateTime();
   }

   Application::~Application()
   {

   }

   String
   Application::GetVersionNumber() const
   {
      return version_;
   }

   String
   Application::GetVersionArchitecture() const
   {
#if _WIN64
      return "x64";
#else
      return "x86";
#endif 
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
      WorkQueueManager::Instance()->CreateWorkQueue(5, maintenance_queue_);

      // Language needed for COM API
      Languages::Instance()->Load();

      // Create the backup manager that manages backup tasks...
      backup_manager_ = std::shared_ptr<BackupManager>(new BackupManager);

      LOG_DEBUG("Application::InitInstance - Connecting to database...");
      if (!OpenDatabase(sErrorMessage))
      {
         // We could not open the database. We can just
         // as well stop now since there's nothing to do.
         return false;
      }
      
      if (!OnDatabaseConnected(sErrorMessage))
      {
         return false;
      }

      LOG_DEBUG("Application::InitInstance - Loading configuration...");
      if (!Configuration::Instance()->Load())
         return false;

      LOG_DEBUG("Application::InitInstance - Configuration loaded.");

      // Start an asynch workqueue which processes asynchronous tasks from clients.
      WorkQueueManager::Instance()->CreateWorkQueue(Configuration::Instance()->GetAsynchronousThreads(), 
                                                    asynchronous_tasks_queue_);

      return true;
   }

   bool
   Application::OpenDatabase(String &sErrorMessage)
   {
      String sIniFileCheckError;
      if (!IniFileSettings::Instance()->CheckSettings(sIniFileCheckError))
      {
         String sError;
         sError.Format(_T("Loading of ini file settings failed. Error: %s"), sIniFileCheckError.c_str());

         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5005, "Application::OpenDatabase", sError);
         return false;
      }

      db_manager_ = std::shared_ptr<DatabaseConnectionManager>(new DatabaseConnectionManager);
      bool bConnectedSuccessfully = db_manager_->CreateConnections(sErrorMessage);

      last_connect_error_message_ = sErrorMessage;

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
         last_connect_error_message_ = sErrorMessage;
         ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5010, "Application::OnDatabaseConnected", sErrorMessage);
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
         
         last_connect_error_message_ = sErrorMessage;

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
      WorkQueueManager::Instance()->RemoveQueue(maintenance_queue_);

      WorkQueueManager::Instance()->RemoveQueue(asynchronous_tasks_queue_);

      // Backup manager is created by initinstance so should be destroyed here.
      if (backup_manager_) 
         backup_manager_.reset();

      LOG_DEBUG("Application::ExitInstance - Closing database connection...");
      CloseDatabase();

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
      if (db_manager_)
      {
         try
         {
            db_manager_->Disconnect();
         }
         catch (...)
         {

         }

         db_manager_.reset();
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

      io_service_ = std::shared_ptr<IOService>(new IOService);
      io_service_->Initialize();
      RegisterSessionTypes_();

      // Create the main work queue.
      size_t iMainServerQueue = WorkQueueManager::Instance()->CreateWorkQueue(4, server_work_queue_);

      notification_server_ = std::shared_ptr<NotificationServer>(new NotificationServer());
      folder_manager_ = std::shared_ptr<FolderManager>(new FolderManager());
      
      // Create the scheduler. This is always in use.
      scheduler_ = std::shared_ptr<Scheduler>(new Scheduler());
      WorkQueueManager::Instance()->AddTask(iMainServerQueue, scheduler_);

      WorkQueueManager::Instance()->AddTask(iMainServerQueue, io_service_);

      smtp_delivery_manager_ = std::shared_ptr<SMTPDeliveryManager>(new SMTPDeliveryManager);
      WorkQueueManager::Instance()->AddTask(iMainServerQueue, smtp_delivery_manager_);

      external_fetch_manager_ = std::shared_ptr<ExternalFetchManager> (new ExternalFetchManager);
      WorkQueueManager::Instance()->AddTask(iMainServerQueue, external_fetch_manager_);

      CreateScheduledTasks_();

      if (Configuration::Instance()->GetMessageIndexing())
      {
         MessageIndexer::Instance()->Start();
      }

      start_time_ = Time::GetCurrentDateTime();

      scheduler_->GetIsStartedEvent().Wait();
      smtp_delivery_manager_->GetIsStartedEvent().Wait();
      external_fetch_manager_->GetIsStartedEvent().Wait();
      io_service_->GetIsStartedEvent().Wait();

      ServerStatus::Instance()->SetState(ServerStatus::StateRunning);
      LOG_APPLICATION("Servers started.")

      return true;
   }

   void 
   Application::RegisterSessionTypes_()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Registers the different session types...
   //---------------------------------------------------------------------------()
   {
      // Start SMTP server and delivery threads.
      if (Configuration::Instance()->GetUseSMTP())
      {
         io_service_->RegisterSessionType(STSMTP);
      }

      if (Configuration::Instance()->GetUsePOP3())
      {
         io_service_->RegisterSessionType(STPOP3);
      }

      if (Configuration::Instance()->GetUseIMAP())
      {
         io_service_->RegisterSessionType(STIMAP);
      }
   }

   void 
   Application::CreateScheduledTasks_()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Create scheduled tasks.
   //---------------------------------------------------------------------------()
   {
      if (Configuration::Instance()->GetUseSMTP())
      {
         std::shared_ptr<GreyListCleanerTask> pCleanerTask = std::shared_ptr<GreyListCleanerTask>(new GreyListCleanerTask);
         pCleanerTask->SetReoccurance(ScheduledTask::RunInfinitely);
         pCleanerTask->SetMinutesBetweenRun(IniFileSettings::Instance()->GetGreylistingExpirationInterval());
         scheduler_->ScheduleTask(pCleanerTask);
      }

      // cleaning of expired IP ranges.
      std::shared_ptr<RemoveExpiredRecords> removeExpiredRecordsTask = std::shared_ptr<RemoveExpiredRecords>(new RemoveExpiredRecords);
      removeExpiredRecordsTask->SetReoccurance(ScheduledTask::RunInfinitely);
      removeExpiredRecordsTask->SetMinutesBetweenRun(1);
      scheduler_->ScheduleTask(removeExpiredRecordsTask);

   }

   void 
   Application::StopServers()
   {
      LOG_APPLICATION("Stopping servers...")

      start_time_ = "";

      if (ServerStatus::Instance()->GetState() != ServerStatus::StateRunning)
      {
         LOG_DEBUG("Application::~StopServers() - E1");
         return;
      }

      ServerStatus::Instance()->SetState(ServerStatus::StateStopping);

      LOG_DEBUG("Application::StopServers() - Removing server work queue");
      // Then remove the main server.
      WorkQueueManager::Instance()->RemoveQueue(server_work_queue_);

      OnServerStopped();

      // Deinitialize servers
      LOG_DEBUG("Application::StopServers() - Destructing IOCP");
      if (io_service_) io_service_.reset();
      LOG_DEBUG("Application::StopServers() - Destructing DeliveryManager");
      if (smtp_delivery_manager_) smtp_delivery_manager_.reset();
      LOG_DEBUG("Application::StopServers() - Destructing FetchManager");
      if (external_fetch_manager_) external_fetch_manager_.reset();
      LOG_DEBUG("Application::StopServers() - Destructing Scheduler");
      if (scheduler_) scheduler_.reset();
      
      LOG_DEBUG("Application::StopServers() - Destructing Rest");
      if (notification_server_) notification_server_.reset();
      if (folder_manager_) folder_manager_.reset();

      MessageIndexer::Instance()->Stop();
      
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

      if (smtp_delivery_manager_)
         smtp_delivery_manager_->SetDeliverMessage();
      else
         // We could not notify the SMTP delivery manager, since it does not exit.
         ErrorManager::Instance()->ReportError(ErrorManager::High, 4219, "Application::SubmitPendingEmail", "Could not notify SMTP deliverer about new message, since SMTP deliverer does not exist. The operation requires the SMTP server to be on.");
   }

   std::shared_ptr<WorkQueue>
   Application::GetMaintenanceWorkQueue()
   {
      std::shared_ptr<WorkQueue> pWorkQueue =
         WorkQueueManager::Instance()->GetQueue(maintenance_queue_);
      
      if (!pWorkQueue)
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5118, "Application::GetMaintenanceWorkQueue()", "Maintenance work queue not available.");

      return pWorkQueue;

      
   }

   std::shared_ptr<WorkQueue>
   Application::GetAsyncWorkQueue()
   {
      std::shared_ptr<WorkQueue> pAsynchQueue = WorkQueueManager::Instance()->GetQueue(asynchronous_tasks_queue_);

      if (!pAsynchQueue)
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5118, "Application::GetAsyncWorkQueue()", "Async work queue not available.");

      return pAsynchQueue;
   }

   int 
   Application::GetUniqueID()
   {
      int iResult = InterlockedIncrement(&unique_id_);

      return iResult;
   }

   void
   Application::OnPropertyChanged(std::shared_ptr<Property> pProperty)
   {
      if (ServerStatus::Instance()->GetState() == ServerStatus::StateStopped)
         return;

      String sPropertyName = pProperty->GetName();

      if (sPropertyName == PROPERTY_MAX_NUMBER_OF_ASYNC_TASKS)
      {
         std::shared_ptr<WorkQueue> pWorkQueue = GetAsyncWorkQueue();

         if (!pWorkQueue)
            return;

         pWorkQueue->SetMaxSimultaneous(pProperty->GetLongValue());
      }
   }

   std::shared_ptr<NotificationServer> 
   Application::GetNotificationServer()
   {
      return notification_server_;
   }

   std::shared_ptr<FolderManager> 
   Application::GetFolderManager()
   {
      return folder_manager_;
   }
}
