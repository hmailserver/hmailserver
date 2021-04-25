// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
// START: Enable tracking of memory leaks.

#ifdef _DEBUG
   #define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
   #define new DEBUG_NEW
#endif

#include "resource.h"
#include "hMailServer.h"

#include "../Common/Util/ServiceManager.h"
#include "../Common/Util/ClassTester.h"
#include "../Common/Util/SystemInformation.h"

// #define VLD_START_DISABLED
// #include "C:\Temp\vld-10\vldapi.h"
// #include "C:\Temp\vld-10\vld.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

using namespace HM;

// Declarations of some extremely global 
// functions and variables.
void InitializeApplication();
static void WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
static void WINAPI ServiceController (DWORD Opcode);
void TerminateServer();
#define SERVICE_SUBMIT_MAIL 200
#define SERVICE_RELOAD_CONFIGURATION 201

SERVICE_STATUS          ServiceStatus; 
SERVICE_STATUS_HANDLE   ServiceStatusHandle; 

DWORD WINAPI StartServiceInitialization(LPVOID vd);

bool DEBUG_MODE = false;

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   class ChMailServerModule : public CAtlExeModuleT< ChMailServerModule >
   {
   public :
	   DECLARE_LIBID(LIBID_hMailServer)
	   DECLARE_REGISTRY_APPID_RESOURCEID(IDR_HMAILSERVER, "{5EDEC473-39E0-43F6-A234-1947071721C8}")
      
      HRESULT PreMessageLoop(int nShowCmd) throw()
      //---------------------------------------------------------------------------//
      // This function is called by __super. We are only supposed to do the
      // initialization in it. But since we're really free to do whatever we
      // want, we simply start the service and wait for that to terminate.
      // When the service has terminated, we return S_FALSE, which will lead to
      // __super to quit.
      //---------------------------------------------------------------------------//
      {
         if (run_as_service_)
         {
            // Start the server threads now.
            StartServiceInitialization(0);
            return S_FALSE;
         }

         return S_OK;
      }

      void
      RegisterObjects()
      {
         HRESULT hr = CoInitializeSecurity(0, -1, 0, 0, RPC_C_AUTHN_LEVEL_CONNECT , RPC_C_IMP_LEVEL_IMPERSONATE, 0,0,0);

         if (FAILED(hr))
         {
            String sErrorMessage;
            sErrorMessage.Format(_T("CoInitializeSecurity returned %d"), hr);

            ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5078, "hMailServer::RegisterObjects", sErrorMessage);

            return;
         }

         hr = RegisterClassObjects(CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE | REGCLS_SUSPENDED);

         if (FAILED(hr))
         {
            String sErrorMessage;
            sErrorMessage.Format(_T("RegisterClassObjects returned %d"), hr);
            
            ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5079, "hMailServer::RegisterObjects", sErrorMessage);

            return;
         }
      }

      void
      ResumeObjects()
      {
         HRESULT hr = CoResumeClassObjects();

         if (FAILED(hr))
         {
            String sErrorMessage;
            sErrorMessage.Format(_T("CoResumeClassObjects returned %d"), hr);

            ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5080, "hMailServer::ResumeObjects", sErrorMessage);

            return;
         }

      }

      LONG Unlock() throw()
      //---------------------------------------------------------------------------//
      // This function is called when an client releases it's reference to
      // hMailServer. If this function returns 0, we are deleted. When running
      // as service, we should never die using this mechanism, hence the return 1.
      //---------------------------------------------------------------------------//
      {
         if (run_as_service_)
            return 1;
         else
            return __super::Unlock();
      }

      HRESULT RegisterAppID()
      //------------------------------------------------------------------------//
      // This function registers an APP-ID in the registry. If this isn't registered
      // it wont be possible to use CreateObject("hMailServer.Application") to get
      // a reference to the running application. 
      //------------------------------------------------------------------------//
      {
         CRegKey keyAppID;
         LONG lRes = keyAppID.Open(HKEY_CLASSES_ROOT, _T("AppID"), KEY_WRITE);
         if (lRes != ERROR_SUCCESS)
            return AtlHresultFromWin32(lRes);

         CRegKey key;

         lRes = key.Create(keyAppID, GetAppIdT());
         if (lRes != ERROR_SUCCESS)
            return AtlHresultFromWin32(lRes);

         key.DeleteValue(_T("LocalService"));
         if (lRes != ERROR_SUCCESS)
            return AtlHresultFromWin32(lRes);

         key.SetStringValue(_T("LocalService"), _T("hMailServer"));
         if (lRes != ERROR_SUCCESS)
            return AtlHresultFromWin32(lRes);

         return S_OK;
      }

      bool run_as_service_;
   };

   
}

HM::ChMailServerModule _AtlModule;


extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
                                LPTSTR lpCmdLine, int nShowCmd)
{
   // Initailize some service variables.
   ServiceStatus.dwServiceType = SERVICE_WIN32; 
   ServiceStatus.dwWin32ExitCode = 0;
   ServiceStatus.dwServiceSpecificExitCode = 0; 
   ServiceStatus.dwCheckPoint = 0; 
   ServiceStatus.dwWaitHint = 0; 

   // Parse the command line.
   HM::String sCommandLine = GetCommandLine();
   std::vector<String> vecParams = StringParser::SplitString(sCommandLine, " ");

   String sLastParam;
   if (!vecParams.empty())
      sLastParam = vecParams[vecParams.size() - 1];

   _AtlModule.InitializeCom();

   // Register app id so that client can create instances
   // of us even when running as a service under the local
   // system account.
   _AtlModule.RegisterAppID();

   // Run registration / unregistration of the server and service.
   bool bRegisterService = sLastParam.CompareNoCase(_T("/Register")) == 0;
   bool bRegisterTypelib = bRegisterService || (sLastParam.CompareNoCase(_T("/RegisterTypeLib")) == 0);
   if (bRegisterService)
   {
      // --- Register the hMailServer service.
      HM::ServiceManager hSCM;
      if (!hSCM.RegisterService("hMailServer", "hMailServer"))
      {
         return -1;         
      }
   }

   if (bRegisterTypelib)
   {
      if (FAILED(_AtlModule.RegisterAppId()))
      {
         return -1;
      }
      if (FAILED(_AtlModule.RegisterServer(TRUE)))
      {
         return -1;
      }

      return 0;
   }

   if (sLastParam.CompareNoCase(_T("/Unregister")) == 0)
   {
      // ---  Unregister the hMailServer service.
      HM::ServiceManager hSCM;
      hSCM.UnregisterService("hMailServer");

      HRESULT res = _AtlModule.UnregisterServer(TRUE);

      if (SUCCEEDED(res))
         _AtlModule.UnregisterAppId();

      return 0;
   }


   // In Windows 2000/XP, we need to register objects directly
   // after service start (that is now). Otherwise it won't 
   // work. In later versions, we must register them after the 
   // service has notified the SCM about the status. 
   if (SystemInformation::GetOperatingSystem() == SystemInformation::Windows2000 ||
      SystemInformation::GetOperatingSystem() == SystemInformation::WindowsXP ||
      SystemInformation::GetOperatingSystem() == SystemInformation::WindowsNT)
   {
      _AtlModule.RegisterObjects();
   }
   
   int iRet = 0;

   if (sLastParam.CompareNoCase(_T("/Test")) == 0)
   {
      // --- We are running in debug mode.
      //     This is a bit ugly but should work fine
      //     for debugging purposes.
      DEBUG_MODE = true;
	
	   String sErrorMessage;
      if (HM::Application::Instance()->InitInstance(sErrorMessage))
      {
         // We were able to connect to the database server
         // and the database is of the correct version. So
         // now we should start the TCP/IP-servers.
         HM::Application::Instance()->StartServers();
      }

      ClassTester oTester;
      oTester.DoTests();

      HM::Application::Instance()->StopServers();

      HM::Application::Instance()->ExitInstance();
   }
   else if (sLastParam.CompareNoCase(_T("/Debug")) == 0)
   {
      // --- We are running in debug mode.
      //     This is a bit ugly but should work fine
      //     for debugging purposes.
      DEBUG_MODE = true;

      // Connect to the database and create configuration objects.
      InitializeApplication();

      while(true)
      {
         // This thread just sits here and waits
         // until the debugger is stopped.
         Sleep(5000);
      }
   }
   else if (sLastParam.CompareNoCase(_T("RunAsService")) == 0)
   {
      _AtlModule.run_as_service_ = true;

	  iRet = _AtlModule.WinMain(nShowCmd);
     
     if (iRet != 0)
     {
        ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5601, "_tWinMain", Formatter::Format("_AtlModule.WinMain returned {0}", iRet));
     }
   }

	//_CrtDumpMemoryLeaks();
   
   return iRet;
}


DWORD WINAPI StartServiceInitialization(LPVOID vd)
{

   SERVICE_TABLE_ENTRY   DispatchTable[] = 
   { 
      { _T("hMailServer"), ServiceMain }, 
      { NULL,          NULL        } 
   }; 

   StartServiceCtrlDispatcher( DispatchTable );

   return 0;
}

void
ReportServiceStatus(DWORD dwState, DWORD dwControlsAccepted)
{
   ServiceStatus.dwCurrentState  = dwState;
   ServiceStatus.dwControlsAccepted   = dwControlsAccepted;

   // Send the server status to SCM so that 
   // it knows that we have started.
   SetServiceStatus (ServiceStatusHandle,  &ServiceStatus);
}

static void WINAPI 
ServiceMain(DWORD /*dwArgc*/, LPTSTR* /*lpszArgv*/)
//------------------------------------------------------------------------//
// The main entry point for SCM.
//------------------------------------------------------------------------//
{
   // MSDN: 
   // The ServiceMain function of a new service should immediately call the RegisterServiceCtrlHandler 
   // function to register a control handler function with the control dispatcher.
   ServiceStatusHandle = RegisterServiceCtrlHandler(_T("hMailServer"), ServiceController); 

   // Then it immediately calls the SetServiceStatus function to notify the 
   // service control manager that its status is SERVICE_START_PENDING. 
   // Tell SCM that we have started.
   ReportServiceStatus(SERVICE_RUNNING, SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN);

   // In Windows 2000/XP, we need to register objects directly
   // after service start. Otherwise it won't work. In later
   // versions, we must register them after the service has
   // notified the SCM about the status (which is right about now). 
   if (SystemInformation::GetOperatingSystem() == SystemInformation::Windows2003)
      _AtlModule.RegisterObjects();

   // Connect to the database and create configuration objects.
   // We need to do this AFTER we have registered the COM objects
   // Otherwise it won't work properly "on all" Win2k3 machines.
   InitializeApplication();

   
   // Inform the OLE SCM that it can now create objects.
   // based on this application. This needs to done after
   // we have connected to the database (Otherwise clients
   // may create instances of for example hMailServer.Database
   // with no open connection.
   _AtlModule.ResumeObjects();
}

void
TerminateServer()
{
   ReportServiceStatus(SERVICE_STOP_PENDING, 0);

   HM::Application::Instance()->StopServers();

   HM::Application::Instance()->ExitInstance();

   if (ServiceStatusHandle != 0)
   {
      DWORD MainThread = GetCurrentThreadId();
      PostThreadMessage(MainThread, WM_QUIT, 0, 0);

      ReportServiceStatus(SERVICE_STOPPED, 0);
   }
   

}


static void WINAPI 
ServiceController (DWORD Opcode) 
//---------------------------------------------------------------------------()
// DESCRIPTION:
// Entrypoint for SCM to change status of this service.
//---------------------------------------------------------------------------()
{ 

   switch(Opcode) 
   { 
   case SERVICE_CONTROL_PAUSE: 
      ServiceStatus.dwCurrentState = SERVICE_PAUSED; 
      break; 
   case SERVICE_CONTROL_CONTINUE: 
      ServiceStatus.dwCurrentState = SERVICE_RUNNING; 
      break; 
   case SERVICE_CONTROL_SHUTDOWN:
      LOG_DEBUG("Received shutdown-request from Windows.");
      TerminateServer();
      break;
   case SERVICE_CONTROL_STOP: 
      LOG_DEBUG("Received stop-request from Windows.");
      TerminateServer();
      // --- Send message to the main thread to quit.
      return; 
   case SERVICE_SUBMIT_MAIL:
      // Submit pending e-mails.
      HM::Application::Instance()->SubmitPendingEmail();
      break;

   } 

   // Send current status. 
   SetServiceStatus (ServiceStatusHandle,  &ServiceStatus);

   return; 
} 

void InitializeApplication()
{
   // Initialize the application.
   // This will cause the application object to be created,
   // and the settings to be loaded. If the settings are
   // loaded properly, StartServers() will start the TCP/IP
   // servers.

   // First allocate all singletons.
   String sErrorMessage;
   if (HM::Application::Instance()->InitInstance(sErrorMessage))
   {
      // We were able to connect to the database server
      // and the database is of the correct version. So
      // now we should start the TCP/IP-servers.
      HM::Application::Instance()->StartServers();
   }

}


