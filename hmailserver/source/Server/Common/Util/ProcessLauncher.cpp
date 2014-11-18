// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "ProcessLauncher.h"
#include "Utilities.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   ProcessLauncher::ProcessLauncher(const String &commandLine, const String &workingDirectory) :
      command_line_(commandLine),
      working_directory_(workingDirectory),
      error_log_timeout_(0)
   {
   }

   ProcessLauncher::ProcessLauncher(const String &commandLine) :
      command_line_(commandLine),
      error_log_timeout_(0)
   {
      working_directory_ = IniFileSettings::Instance()->GetTempDirectory();
   }

   ProcessLauncher::~ProcessLauncher(void)
   {
   }

   void
   ProcessLauncher::SetErrorLogTimeout(unsigned int milliseconds)
   {
      error_log_timeout_ = milliseconds;
   }

   bool
   ProcessLauncher::Launch(unsigned int &exitCode)
   {
      exitCode = 0;

      STARTUPINFO si;
      PROCESS_INFORMATION pi;

      ZeroMemory( &si, sizeof(si) );
      si.cb = sizeof(si);

      si.dwFlags = STARTF_USESHOWWINDOW;
      si.wShowWindow = SW_HIDE;

      ZeroMemory( &pi, sizeof(pi) );

      DWORD creationFlags = 0;

      // Start the child process. 
      if ( !CreateProcess( NULL,    // No module name (use command line). 
         command_line_.GetBuffer(0), // Command line. 
         NULL,                      // Process handle not inheritable. 
         NULL,                      // Thread handle not inheritable. 
         FALSE,                     // Set handle inheritance to FALSE. 
         creationFlags,                         // No creation flags. 
         NULL,                      // Use parent's environment block. 
         working_directory_.GetBuffer(0),        // Use parent's starting directory. 
         &si,                       // Pointer to STARTUPINFO structure.
         &pi )                      // Pointer to PROCESS_INFORMATION structure.
         ) 
      {
         String errorMessage = Formatter::Format("There was an error launching external process {0}. Process start failed. Windows error code: {1}", command_line_, (int) GetLastError());
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5401, "ProcessLauncher::Launch", errorMessage); 
         
         return false;
      }

      command_line_.ReleaseBuffer();
      working_directory_.ReleaseBuffer();

      DWORD waitResult = 0;

      if (error_log_timeout_ > 0)
      {
         // If it takes too long time, we should report an error. After that, we
         // should continue to wait.
         waitResult = WaitForSingleObject( pi.hProcess, error_log_timeout_ );
         switch (waitResult)
         {
         case WAIT_ABANDONED:
            ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5402, "ProcessLauncher::Launch", "Wait abandoned."); 
            break;
         case WAIT_TIMEOUT:
            {
               String errorMessage = Formatter::Format("A launched process did not exit within an expected time. The command line is {0}. The timeout occurred after {1} milliseconds. hMailServer will continue to wait for process to finish.", command_line_, error_log_timeout_);
               ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5400, "ProcessLauncher::Launch", errorMessage); 
               break;
            }
         case WAIT_FAILED:
            {
               String errorMessage = Formatter::Format("Failed to wait. Windows error code: {0}.", (int) GetLastError());
               ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5403, "ProcessLauncher::Launch", errorMessage); 
               break;
            }
         }
      }

      // Wait until child process exits.
      waitResult = WaitForSingleObject( pi.hProcess, INFINITE);

      switch (waitResult)
      {
      case WAIT_ABANDONED:
         {
            ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5404, "ProcessLauncher::Launch", "Wait abandoned (infinite wait)."); 
            break;
         }
      case WAIT_FAILED:
         {
            String errorMessage = Formatter::Format("Failed to wait. Windows error code: {0}.", (int) GetLastError());
            ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5405, "ProcessLauncher::Launch", errorMessage); 
            break;
         }
      }

      int result = 0;

      ULONG rc;
      if (!GetExitCodeProcess(pi.hProcess, &rc))
      {
         String errorMessage = Formatter::Format("There was an error determining the exit code of {0}. Windows error: {1}", command_line_, (int) GetLastError());
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5400, "ProcessLauncher::Launch", errorMessage); 

         rc = 0;
      }

      exitCode = rc;

      // Close process and thread handles. 
      CloseHandle( pi.hProcess );
      CloseHandle( pi.hThread );

      return true;
   }

}
