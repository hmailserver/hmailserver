// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "TestErrorLogs.h"
#include "../Util/FileInfo.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   TestErrorLogs::TestErrorLogs()
   {

   }

   TestErrorLogs::~TestErrorLogs()
   {

   }

   DiagnosticResult
   TestErrorLogs::PerformTest()
   {
      DiagnosticResult diagResult;
      diagResult.SetName("Error logs");
      diagResult.SetDescription("Checks for error logs");

      String result;

      auto log_dir = IniFileSettings::Instance()->GetLogDirectory();

      auto all_error_logs = FileUtilities::GetFilesInDirectory(log_dir, "^ERROR.*$");

      if (all_error_logs.size() > 0)
      {
         diagResult.SetDetails(Formatter::Format(_T("There are {0} error logs in the log directory."), all_error_logs.size()));
         diagResult.SetSuccess(false);
      }
      else
      {
         diagResult.SetDetails("There are no error logs in the log directory.");
         diagResult.SetSuccess(true);
      }

      return diagResult;
   }


   
      
}
