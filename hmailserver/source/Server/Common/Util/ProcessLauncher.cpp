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
      _commandLine(commandLine),
      _workingDirectory(workingDirectory),
      _errorLogTimeout(0)
   {
   }

   ProcessLauncher::ProcessLauncher(const String &commandLine) :
      _commandLine(commandLine),
      _errorLogTimeout(0)
   {
      _workingDirectory = Utilities::GetWin32TempDirectory();
   }

   ProcessLauncher::~ProcessLauncher(void)
   {
   }

   void
   ProcessLauncher::SetErrorLogTimeout(unsigned int milliseconds)
   {
      _errorLogTimeout = milliseconds;
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

      // Start the child process. 
      if ( !CreateProcess( NULL,    // No module name (use command line). 
         _commandLine.GetBuffer(0), // Command line. 
         NULL,                      // Process handle not inheritable. 
         NULL,                      // Thread handle not inheritable. 
         FALSE,                     // Set handle inheritance to FALSE. 
         0,                         // No creation flags. 
         NULL,                      // Use parent's environment block. 
         _workingDirectory.GetBuffer(0),        // Use parent's starting directory. 
         &si,                       // Pointer to STARTUPINFO structure.
         &pi )                      // Pointer to PROCESS_INFORMATION structure.
         ) 
      {
         String errorMessage = Formatter::Format("There was an error launching external process {0}. Process start failed. Windows error code: {1}", _commandLine, (int) GetLastError());
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5401, "ProcessLauncher::Launch", errorMessage); 
         
         return false;
      }

      _commandLine.ReleaseBuffer();
      _workingDirectory.ReleaseBuffer();

      DWORD waitResult = 0;

      if (_errorLogTimeout > 0)
      {
         // If it takes too long time, we should report an error. After that, we
         // should continue to wait.
         waitResult = WaitForSingleObject( pi.hProcess, _errorLogTimeout );
         switch (waitResult)
         {
         case WAIT_ABANDONED:
            ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5402, "ProcessLauncher::Launch", "Wait abandoned."); 
            break;
         case WAIT_TIMEOUT:
            {
               String errorMessage = Formatter::Format("A launched process did not exit within an expected time. The command line is {0}. The timeout occurred after {1} milliseconds. hMailServer will continue to wait for process to finish.", _commandLine, _errorLogTimeout);
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
         String errorMessage = Formatter::Format("There was an error determining the exit code of {0}. Windows error: {1}", _commandLine, (int) GetLastError());
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
