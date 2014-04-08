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
   class IOCPServer;
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
      shared_ptr<DatabaseConnectionManager> GetDBManager() { return m_pDBManager; }

      // --- overridables
      virtual bool InitInstance(String &sErrorMessage);
      virtual bool ExitInstance();

      String GetVersion() const;
      String GetVersionNumber() const {return m_sVersion;}

      String GetStartTime() const {return m_sStartTime; }

      bool OpenDatabase(String &sErrorMessage);
      void CloseDatabase();

      
      String GetLastErrorMessage() {return m_sLastConnectErrorMessage; }; 

      bool StartServers();
      void StopServers();
      void SubmitPendingEmail();

      shared_ptr<SMTPDeliveryManager> GetSMTPDeliveryManager() {return m_pSMTPDeliveryManager;} 
      shared_ptr<ExternalFetchManager> GetExternalFetchManager() {return m_pExternalFetchManager;} 

      shared_ptr<BackupManager> GetBackupManager() {return m_pBackupManager; }

      shared_ptr<WorkQueue> GetRandomWorkQueue();
      shared_ptr<WorkQueue> GetAsyncWorkQueue();
      shared_ptr<IOCPServer> GetIOCPServer() {return m_pIOCPServer; }
      // The random work queue can run any task.

      shared_ptr<NotificationServer> GetNotificationServer();
      shared_ptr<FolderManager> GetFolderManager();

      String Reinitialize();

      int GetUniqueID();

      void SetServerStartedEvent();

      void OnPropertyChanged(shared_ptr<Property> pProperty);

   protected:

      virtual bool OnDatabaseConnected(String &sErrorMessage);

   private:

      void _RegisterSessionTypes();
      void _CreateScheduledTasks();

      String m_sProdName;
      String m_sVersion;
      String m_sStartTime;
      
      String m_sLastConnectErrorMessage;

      shared_ptr<DatabaseConnectionManager> m_pDBManager;
   
      // the servers
      shared_ptr<SMTPDeliveryManager> m_pSMTPDeliveryManager;

      shared_ptr<ExternalFetchManager> m_pExternalFetchManager;
      shared_ptr<BackupManager> m_pBackupManager;
      shared_ptr<Scheduler> m_pScheduler;
      shared_ptr<NotificationServer> m_pNotificationServer;
      shared_ptr<IOCPServer> m_pIOCPServer;
      shared_ptr<FolderManager> _folderManager;

      const String m_sRandomWorkQueue;
      // The random work queue can run any type of task.

      const String m_sServerWorkQueue;
      // The main server queue, that contains one task per server.

      const String m_sAsynchronousTasksQueue;

      long m_iUniqueID;

      Event _serverStartEvent;
   };
}
