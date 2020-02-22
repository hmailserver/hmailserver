// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../Util/Event.h"

namespace HM
{
 
   class SMTPDeliveryManager;
   class ExternalFetchManager;
   class BackupManager;
   class DatabaseConnectionManager;
   class Scheduler;
   class WorkQueue;
   class IOService;
   class ServerMessages;
   class NotificationServer;
   class FolderManager;

   class Application : public Singleton<Application>
   {
   public:

	   Application();
	   virtual ~Application();

      static String GetExecutableName();

      // --- global 
      std::shared_ptr<DatabaseConnectionManager> GetDBManager() { return db_manager_; }

      // --- overridables
      virtual bool InitInstance(String &sErrorMessage);
      virtual bool ExitInstance();

      String GetVersionNumber() const;
      String GetVersionArchitecture() const;
      

      String GetStartTime() const {return start_time_; }

      bool OpenDatabase(String &sErrorMessage);
      void CloseDatabase();

      
      String GetLastErrorMessage() {return last_connect_error_message_; }; 

      bool StartServers();
      void StopServers();
      void SubmitPendingEmail();

      std::shared_ptr<SMTPDeliveryManager> GetSMTPDeliveryManager() {return smtp_delivery_manager_;} 
      std::shared_ptr<ExternalFetchManager> GetExternalFetchManager() {return external_fetch_manager_;} 

      std::shared_ptr<BackupManager> GetBackupManager() {return backup_manager_; }

      std::shared_ptr<WorkQueue> GetMaintenanceWorkQueue();
      std::shared_ptr<WorkQueue> GetAsyncWorkQueue();
      std::shared_ptr<IOService> GetIOService() {return io_service_; }
      // The random work queue can run any task.

      std::shared_ptr<NotificationServer> GetNotificationServer();
      std::shared_ptr<FolderManager> GetFolderManager();

      String Reinitialize();

      int GetUniqueID();

      void OnPropertyChanged(std::shared_ptr<Property> pProperty);

      bool OnDatabaseConnected(String &sErrorMessage);

      boost::signals2::signal<void()> OnServerStopped;

   private:

      void RegisterSessionTypes_();
      void CreateScheduledTasks_();

      String version_;
      String start_time_;
      
      String last_connect_error_message_;

      std::shared_ptr<DatabaseConnectionManager> db_manager_;
   
      // the servers
      std::shared_ptr<SMTPDeliveryManager> smtp_delivery_manager_;

      std::shared_ptr<ExternalFetchManager> external_fetch_manager_;
      std::shared_ptr<BackupManager> backup_manager_;
      std::shared_ptr<Scheduler> scheduler_;
      std::shared_ptr<NotificationServer> notification_server_;
      std::shared_ptr<IOService> io_service_;
      std::shared_ptr<FolderManager> folder_manager_;

      const String maintenance_queue_;
      // The random work queue can run any type of task.

      const String server_work_queue_;
      // The main server queue, that contains one task per server.

      const String asynchronous_tasks_queue_;

      long unique_id_;
   };
}
