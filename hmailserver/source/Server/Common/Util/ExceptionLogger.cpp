// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "ExceptionLogger.h"
#include "CustomStackWalker.h"

#include "../Util/Time.h"
#include "../Util/GUIDCreator.h"
#include "../Util/FileUtilities.h"

#include <DbgHelp.h>

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   CustomStackWalker ExceptionLogger::stack_walker_;
   boost::mutex ExceptionLogger::stack_walker_mutex_;

   void
   ExceptionLogger::Log(int exception_code, EXCEPTION_POINTERS* pExp)
   {
      boost::mutex::scoped_lock lock(stack_walker_mutex_);

      String log_directory = IniFileSettings::Instance()->GetLogDirectory();
      String current_date_time = Time::GetCurrentDateTime();
      current_date_time.Replace(_T(":"), _T(""));

      String log_identifier = GUIDCreator::GetGUID();

      String stack_file_name = "errordetails_" + current_date_time + "_" + log_identifier + "_stacktrace.log";
      String minidump_file_name = "errordetails_" + current_date_time + "_" + log_identifier + "_minidump.dmp";
      
      String full_path_to_minidump_file = FileUtilities::Combine(log_directory, minidump_file_name);
      String full_path_to_stack_file = FileUtilities::Combine(log_directory, stack_file_name);

      LOG_DEBUG("Creating minidump...");
      CreateMiniDump_(pExp, full_path_to_minidump_file);
      LOG_DEBUG("Minidump created...");

      LOG_DEBUG("Creating stacktrace...");
      CustomStackWalker sw;
      sw.ShowCallstack(GetCurrentThread(), pExp->ContextRecord);
      String stack_trace = sw.GetStackData();
      LOG_DEBUG("Stacktrace created...");

      String stack_dump_data;      
      stack_dump_data.append(Formatter::Format(_T("Exception code: {0}\r\n"), exception_code));
      stack_dump_data.append(Formatter::Format(_T("Stack trace follows: {0}\r\n"), String(stack_trace)));

      FileUtilities::WriteToFile(full_path_to_stack_file, stack_dump_data);

      String message = Formatter::Format("An error has been detected. A stack trace will be logged in {0}. A mini dump will be created in {1}", full_path_to_stack_file, full_path_to_minidump_file);

      ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5519, "StackLogger::Log", message);
   }

      
   void 
   ExceptionLogger::CreateMiniDump_(EXCEPTION_POINTERS* pExp, const String &file_name)
   {   

      HMODULE debug_help_dll = LoadLibrary(_T("dbghelp.dll"));
      if(debug_help_dll==NULL)
      {
         return;
      }

      HANDLE dump_file = CreateFile(
         file_name,
         GENERIC_WRITE,
         0,
         NULL,
         CREATE_ALWAYS,
         FILE_ATTRIBUTE_NORMAL,
         NULL);

      if(dump_file==INVALID_HANDLE_VALUE)
      {
         return;
      }

      MINIDUMP_EXCEPTION_INFORMATION minidump_exception_info;
      minidump_exception_info.ThreadId = GetCurrentThreadId();
      minidump_exception_info.ExceptionPointers = pExp;
      minidump_exception_info.ClientPointers = FALSE;

      MINIDUMP_CALLBACK_INFORMATION minidump_callback_info;
      minidump_callback_info.CallbackRoutine = NULL;
      minidump_callback_info.CallbackParam = NULL;

      typedef BOOL (WINAPI *LPMINIDUMPWRITEDUMP)(
         HANDLE hProcess, 
         DWORD ProcessId, 
         HANDLE hFile, 
         MINIDUMP_TYPE DumpType, 
         CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, 
         CONST PMINIDUMP_USER_STREAM_INFORMATION UserEncoderParam, 
         CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

      LPMINIDUMPWRITEDUMP pfnMiniDumpWriteDump = 
         (LPMINIDUMPWRITEDUMP)GetProcAddress(debug_help_dll, "MiniDumpWriteDump");
      if(!pfnMiniDumpWriteDump)
      {    
         // Bad MiniDumpWriteDump function
         return;
      }

      HANDLE current_process = GetCurrentProcess();
      DWORD process_id = GetCurrentProcessId();

      BOOL bWriteDump = pfnMiniDumpWriteDump(
         current_process,
         process_id,
         dump_file,
         MiniDumpNormal,
         &minidump_exception_info,
         NULL,
         &minidump_callback_info);

      if(!bWriteDump)
      {    
         // Error writing dump.
         return;
      }

      // Close file
      CloseHandle(dump_file);

      // Unload dbghelp.dll
      FreeLibrary(debug_help_dll);
   }


} 