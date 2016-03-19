// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "ServiceManager.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   ServiceManager::ServiceManager()
   {

   }

   ServiceManager::~ServiceManager()
   {

   }

   bool
   ServiceManager::RegisterService(const String &ServiceName, const String &ServiceCaption)
   //---------------------------------------------------------------------------//
   // DESCRIPTION:
   // Registres a service with the SCM. (adds the application to the list of services)
   //---------------------------------------------------------------------------//
   {
      // Retrieve the handle for the local service manager.
      SC_HANDLE hSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_CREATE_SERVICE | SC_MANAGER_LOCK);

      if (hSCManager == NULL)
      {
         return false;
      }

      // Check wether we already exists.
      if (DoesServiceExist(ServiceName))
      {
         if (!ReconfigureService_(hSCManager, ServiceName))
            return false;
      }
      else
      {
         // Get the path to the currently running executable
         String sPath = "\"" + Application::GetExecutableName() + "\" RunAsService";

         // Check wether we should set the service dependent on MySQL.
         LPCTSTR szServiceDependencies = _T("RPCSS\0\0");

         SC_HANDLE hService = CreateService( hSCManager,
                                   ServiceName,
                                   ServiceCaption,
                                   GENERIC_EXECUTE | GENERIC_WRITE | GENERIC_READ,
                                   SERVICE_WIN32_OWN_PROCESS,
                                   SERVICE_AUTO_START,
                                   SERVICE_ERROR_NORMAL,
                                   sPath,
                                   NULL,
                                   NULL,
                                   szServiceDependencies,
                                   NULL,
                                   NULL
                                   );  


         if (hService == NULL)
         {
            String sErrorMessage;
            sErrorMessage.Format(_T("Description: CreateService failed. (%d)"), GetLastError());
            ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5055, "ServiceManager::RegisterService", sErrorMessage);

            return false;
         }
         
         CloseServiceHandle (hService);

      }

      
      CloseServiceHandle (hSCManager);

      return true;
   }

   bool
   ServiceManager::ReconfigureService_(SC_HANDLE hSCManager, const String &ServiceName)
   //---------------------------------------------------------------------------//
   // DESCRIPTION:
   // Updates an existing hMailServer service.
   //---------------------------------------------------------------------------//
   {
      // Retrieve the handle for the service.
      SC_HANDLE hService = OpenService( hSCManager, ServiceName,  SERVICE_QUERY_CONFIG | SERVICE_CHANGE_CONFIG);

      
      SC_LOCK sclLock = LockServiceDatabase(hSCManager); 

      if (sclLock == NULL)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5056, "ServiceManager::ReconfigureService_", "Failed to obtain lock on service database.");

         CloseServiceHandle(hService);
         return false;
      }

      // Update the path to the executable
      String sPath = "\"" + Application::GetExecutableName() + "\" RunAsService";

      if (ChangeServiceConfig(
               hService,        // handle of service 
               SERVICE_NO_CHANGE , // service type: no change 
               SERVICE_NO_CHANGE, // service start type no change
               SERVICE_NO_CHANGE, // error control: no change 
               sPath,            // binary path changed
               NULL,              // load order group: no change 
               NULL,              // tag ID: no change 
               NULL,              // dependencies: no change 
               NULL,              // account name: no change 
               NULL,              // password: no change 
               NULL) == 0)             // display name: no change
      {
         String sErrorMessage;
         sErrorMessage.Format(_T("ChangeServiceConfig failed. (%d)"), GetLastError());

         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5057, "ServiceManager::ReconfigureService_", sErrorMessage);
         return false;
      }

      // Unlock and release.
      CloseServiceHandle(hService); 

      UnlockServiceDatabase(sclLock); 

      CloseServiceHandle (hSCManager);

      return true;
   }

   void
   ServiceManager::MakeDependentOn(const String &ServiceName)
   //---------------------------------------------------------------------------//
   // DESCRIPTION:
   // Makes hMailServer dependent on RPCSS and ServiceName
   //---------------------------------------------------------------------------//
   {
      // Retrieve the handle for the local service manager.
      SC_HANDLE hSCManager;
      hSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_CREATE_SERVICE | SC_MANAGER_LOCK);

      // Retrieve the handle for the service.
      SC_HANDLE hService = OpenService( hSCManager, _T("hMailServer"), SERVICE_CHANGE_CONFIG );

      SC_LOCK sclLock = LockServiceDatabase(hSCManager); 

      if (sclLock == NULL)
      {
         CloseServiceHandle(hService);
         return;
      }

      // Create null-terminated list of services hMailServer depends on.
      int iLength = ServiceName.GetLength() + 8;

      TCHAR * lpDependent = new TCHAR[iLength];
      int memSize = iLength * sizeof(TCHAR);

      memset(lpDependent, 0, memSize);

      _tcscpy_s(lpDependent, memSize, _T("RPCSS"));
      _tcscpy_s(lpDependent + 6, memSize-6, ServiceName);
      
      int iRet = ChangeServiceConfig( 
         hService,        // handle of service 
         SERVICE_NO_CHANGE, // service type: no change 
         SERVICE_NO_CHANGE, // service start type no change
         SERVICE_NO_CHANGE, // error control: no change 
         NULL,            // binary path changed
         NULL,              // load order group: no change 
         NULL,              // tag ID: no change 
         lpDependent,      // dependencies: no change 
         NULL,              // account name: no change 
         NULL,              // password: no change 
         NULL);             // display name: no change


      CloseServiceHandle(hService); 
      UnlockServiceDatabase(sclLock); 

      delete [] lpDependent;
   }

   bool
   ServiceManager::UnregisterService(const String &ServiceName)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Deregisters a service. (removes the service from the list of services)
   //---------------------------------------------------------------------------()
   {
      // Retrieve the handle for the local service manager.
      SC_HANDLE hSCManager;
      hSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_CREATE_SERVICE );

      // Retrieve the handle for the service.
      SC_HANDLE hService = OpenService( hSCManager, ServiceName, SERVICE_ALL_ACCESS );

      if (!hService)
      {
         CloseServiceHandle (hSCManager);
         return false;
      }

      DeleteService(hService);

      CloseServiceHandle (hSCManager);
      CloseServiceHandle (hService);

      return 0;
   }

   bool
   ServiceManager::StartServiceOnLocalComputer(const String &ServiceName)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Starts the service with name ServiceName on the local computer.
   //---------------------------------------------------------------------------()
   {
      // Retrieve the handle for the local service manager.
      SC_HANDLE hSCManager;
      hSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_CONNECT );
 
      // Retrieve the handle for the service.
      SC_HANDLE hService = OpenService( hSCManager, ServiceName, SERVICE_START );
   
      BOOL bResult = StartService(hService, 0, NULL);

      CloseServiceHandle (hSCManager);
      CloseServiceHandle (hService);

      if (bResult)
         return true;
      else
         return false;

   }

   bool
   ServiceManager::StopServiceOnLocalComputer(const String &ServiceName)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Stops the service with name ServiceName on the local computer.
   //---------------------------------------------------------------------------()
   {
      // Retrieve the handle for the local service manager.
      SC_HANDLE hSCManager;
      hSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_CONNECT );
 
      // Retrieve the handle for the service.
      SC_HANDLE hService = OpenService( hSCManager, ServiceName, SERVICE_STOP );
   
      SERVICE_STATUS ssStatus;
      BOOL bResult = ::ControlService(hService, SERVICE_CONTROL_STOP, &ssStatus);

      CloseServiceHandle (hSCManager);
      CloseServiceHandle (hService);

      if (bResult)
         return true;
      else
         return false;

   }

   bool
   ServiceManager::UserControlService(const String &ServiceName, DWORD OpCode)
   {
      // Retrieve the handle for the local service manager.
      SC_HANDLE hSCManager;
      hSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_CONNECT );

      if (hSCManager == NULL)
      {
         int err = GetLastError();
         switch (err)
         {
            case ERROR_ACCESS_DENIED:
               ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5059, "ServiceManager::UserControlService", "OpenSCManager failed. The requested access was denied.");
               return false;
            case ERROR_DATABASE_DOES_NOT_EXIST:
               ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5096, "ServiceManager::UserControlService", "OpenSCManager failed. The specified database does not exist.");
               return false;
            case ERROR_INVALID_PARAMETER:
               ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5061, "ServiceManager::UserControlService", "OpenSCManager failed. A specified parameter is invalid.");
               return false;
         }

      }
 


      // Retrieve the handle for the service.
      SC_HANDLE hService = OpenService( hSCManager, ServiceName, SERVICE_USER_DEFINED_CONTROL );

      if (hService == NULL)
      {
         int err = GetLastError();

         switch (err)
         {
            case ERROR_ACCESS_DENIED:
               ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5062, "ServiceManager::UserControlService", "OpenService failed. The specified service control manager database handle does not have access to the service.");
               return false;
            case ERROR_INVALID_HANDLE:
               ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5063, "ServiceManager::UserControlService", "OpenService failed. The specified handle is invalid.");
               return false;
            case ERROR_INVALID_NAME:
               ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5064, "ServiceManager::UserControlService", "OpenService failed. The specified service name is invalid.");
               return false;
            case ERROR_SERVICE_DOES_NOT_EXIST:
               ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5065, "ServiceManager::UserControlService", "OpenService failed. The specified service does not exist.");
               return false;
         }
      }

   

      // Thell the thserver what to to
      SERVICE_STATUS ssStatus;
      BOOL bResult = ControlService(hService, OpCode, &ssStatus);


      if (bResult != 0)
      {
         int err = GetLastError();

         switch (err)
         {
            case ERROR_ACCESS_DENIED:
               ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5066, "ServiceManager::ControlService", "ControlService failed. The specified handle was not opened with the necessary access.");
               return false;
            case ERROR_DEPENDENT_SERVICES_RUNNING:
               ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5067, "ServiceManager::ControlService", "ControlService failed. The service cannot be stopped because other running services are dependent on it.");
               return false;
            case ERROR_INVALID_HANDLE:
               ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5068, "ServiceManager::ControlService", "ControlService failed. The specified handle was not obtained using CreateService or OpenService, or the handle is no longer valid.");
               return false;
            case ERROR_INVALID_PARAMETER:
               ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5069, "ServiceManager::ControlService", "ControlService failed. The requested control code is undefined.");
               return false;
            case ERROR_INVALID_SERVICE_CONTROL:
               ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5070, "ServiceManager::ControlService", "ControlService failed. The requested control code is not valid, or it is unacceptable to the service.");
               return false;
            case ERROR_SERVICE_CANNOT_ACCEPT_CTRL:
               ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5071, "ServiceManager::ControlService", "ControlService failed. The requested control code cannot be sent to the service because the state of the service is SERVICE_STOPPED, SERVICE_START_PENDING, or SERVICE_STOP_PENDING.");
               return false;
            case ERROR_SERVICE_NOT_ACTIVE:
               ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5072, "ServiceManager::ControlService", "ControlService failed. The service has not been started.");
               return false;
            case ERROR_SERVICE_REQUEST_TIMEOUT:
               ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5073, "ServiceManager::ControlService", "ControlService failed. The process for the service was started, but it did not call StartServiceCtrlDispatcher, or the thread that called StartServiceCtrlDispatcher may be blocked in a control handler function.");
               return false;
            case ERROR_SHUTDOWN_IN_PROGRESS:
               ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5074, "ServiceManager::ControlService", "ControlService failed. The system is shutting down.");
               return false;
         }
      }


      CloseServiceHandle (hSCManager);
      CloseServiceHandle (hService);

      if (bResult)
         return true;
      else
         return false;
   }

   SERVICE_STATUS
   ServiceManager::GetServiceStatus(const String &ServiceName)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Stops the service with name ServiceName on the local computer.
   //---------------------------------------------------------------------------()
   {
      // Retrieve the handle for the local service manager.
      SC_HANDLE hSCManager;
      hSCManager = OpenSCManager( NULL, NULL, GENERIC_READ );
 
      // Retrieve the handle for the service.
      SC_HANDLE hService = OpenService( hSCManager, ServiceName, SERVICE_QUERY_STATUS );
   
      SERVICE_STATUS Status;
      BOOL bResult = QueryServiceStatus(hService, &Status);

      int e = GetLastError();

      CloseServiceHandle (hSCManager);
      CloseServiceHandle (hService);

      return Status;

   }

   bool
   ServiceManager::DoesServiceExist(const String &ServiceName)
   {
      // Retrieve the handle for the local service manager.
      SC_HANDLE hSCManager;
      hSCManager = OpenSCManager( NULL, NULL, GENERIC_READ );
 
      // Retrieve the handle for the service.
      SC_HANDLE hService = OpenService( hSCManager, ServiceName, SERVICE_QUERY_STATUS );
      
      CloseServiceHandle (hSCManager);
      CloseServiceHandle (hService);

      if (hService == NULL)
         return false;
      else
         return true;
   }

}
