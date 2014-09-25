// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "StackLogger.h"
#include "CustomStackWalker.h"

#include "../Util/Time.h"
#include "../Util/GUIDCreator.h"
#include "../Util/FileUtilities.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   CustomStackWalker StackLogger::stack_walker_;
   boost::mutex StackLogger::stack_walker_mutex_;

   void
   StackLogger::Log(int exception_code, const CONTEXT *context)
   {
      boost::mutex::scoped_lock lock(stack_walker_mutex_);

      CustomStackWalker sw;
      sw.ShowCallstack(GetCurrentThread(), context);

      String stack_trace = sw.GetStackData();

      String log_directory = IniFileSettings::Instance()->GetLogDirectory();
      String current_date_time = Time::GetCurrentDateTime();
      current_date_time.Replace(_T(":"), _T(""));
      String stack_file_name = "stacktrace_" + current_date_time + "_" + GUIDCreator::GetGUID() + ".log";

      String full_path_to_stack_file = FileUtilities::Combine(log_directory, stack_file_name);

      String stack_dump_data;      
      stack_dump_data.append(Formatter::Format(_T("Exception code: {0}\r\n"), exception_code));
      stack_dump_data.append(Formatter::Format(_T("Stack trace follows: {0}\r\n"), String(stack_trace)));

      FileUtilities::WriteToFile(full_path_to_stack_file, stack_dump_data);

      String message = Formatter::Format("An error has been detected and an error stack will be generated and saved in {0}.", full_path_to_stack_file);

      ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5519, "StackLogger::Log", message);
   }
} 