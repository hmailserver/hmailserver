// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "ExceptionLogger.h"

#include "FileInfo.h"
#include "Time.h"
#include "GUIDCreator.h"
#include "FileUtilities.h"
#include "MiniDumpInput.h"
#include "ProcessLauncher.h"

#include <DbgHelp.h>

#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

using namespace boost::interprocess;

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

      MiniDumpInput view;
      view.ProcessId = ::GetCurrentProcessId();
      view.ThreadId = ::GetCurrentThreadId();
      view.ContextRecord = *pExp->ContextRecord;
      view.ExceptionRecord = *pExp->ExceptionRecord;

      _tcscpy_s(view.DumpFile, 2048, full_path_to_minidump_file.c_str());

      windows_shared_memory shm (create_only, MiniDumpInput::SharedMemoryName.c_str(), read_write, 20000);
      
      mapped_region region(shm, read_write);

      //Write all the memory to 1
      std::memcpy(region.get_address(), &view, sizeof(MiniDumpInput));
      
      String command_line = "hMailServer.minidump.exe";

      unsigned int exit_code;
      ProcessLauncher launcher(command_line);
      if (!launcher.Launch(exit_code))
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5521, "StackLogger::Log", "An error has been detected. hMailServer was unable to launch minidump generator.");
         return;
      }

      if (exit_code == 0)
      {
         String message = Formatter::Format("An error has been detected. A mini dump has been written to {0}", full_path_to_minidump_file);
         ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5519, "StackLogger::Log", message);
      }
      else
      {
         String message = Formatter::Format("An error has been detected. hMailServer attempted to generate minidump, but hMailServer.minidump.exe returned {0}.", exit_code);
         ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5521, "StackLogger::Log", message);
         return;
      }

   }

   bool 
   ExceptionLogger::TryToMakeRoom()
   {
      // error logs are saved for 4 hours, rolling.

      String log_directory = IniFileSettings::Instance()->GetLogDirectory();

      std::vector<FileInfo> existing_files = FileUtilities::GetFilesInDirectory(log_directory, "^minidump_.*$");

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
      
   

} 