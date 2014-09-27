// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "ExceptionLogger.h"

#include "../Util/FileInfo.h"
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
   boost::mutex ExceptionLogger::logging_mutex_;

   void
   ExceptionLogger::Log(int exception_code, EXCEPTION_POINTERS* pExp)
   {
      boost::mutex::scoped_lock lock(logging_mutex_);

      // limit the number of logs crated to prevent disk from becoming full.
      if (!TryToMakeRoom())
      {
         return;
      }

      String log_directory = IniFileSettings::Instance()->GetLogDirectory();
      String current_date_time = Time::GetCurrentDateTime();
      current_date_time.Replace(_T(":"), _T(""));

      String log_identifier = GUIDCreator::GetGUID();

      String minidump_file_name = "minidump_" + current_date_time + "_" + log_identifier + ".dmp";
      
      String full_path_to_minidump_file = FileUtilities::Combine(log_directory, minidump_file_name);

      LOG_DEBUG("Creating minidump...");
      CreateMiniDump_(pExp, full_path_to_minidump_file);
      LOG_DEBUG("Minidump created...");

      String message = Formatter::Format("An error has been detected. A mini dump has been written to {0}", full_path_to_minidump_file);

      ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5519, "StackLogger::Log", message);
   }

   bool 
   ExceptionLogger::TryToMakeRoom()
   {
      // error logs are saved for 4 hours, rolling.

      String log_directory = IniFileSettings::Instance()->GetLogDirectory();

      std::vector<FileInfo> existing_files = FileUtilities::GetFilesInDirectory(log_directory, "minidump_*");

      const int max_count = 10;
      if (existing_files.size() < max_count)
      {
         // there's already room.
         return true;
      }


      FileInfo oldest;

      // check if there's files older than 4 hours.
      for(FileInfo file_info : existing_files)
      {
         if (oldest.GetCreateTime().GetStatus() == DateTime::invalid ||
             file_info.GetCreateTime() < oldest.GetCreateTime())
         {
            oldest = file_info;
         }
      }

      DateTime now = DateTime::GetCurrentTime();
      DateTimeSpan age = now - oldest.GetCreateTime();
      double hoursOld = age.GetNumberOfHours();

      if (hoursOld < 4)
      {
         // we keep all error detail logs for 4 hours.
         LOG_DEBUG(Formatter::Format("Minidump creation aborted. The max count ({0}) is reached and no log is older than 4 hours.", max_count));
         return false;
      }


      String full_path = FileUtilities::Combine(log_directory, oldest.GetName());

      FileUtilities::DeleteFile(full_path);
      return true;
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