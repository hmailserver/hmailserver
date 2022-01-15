// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "TestInformationGatherer.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   TestInformationGatherer::TestInformationGatherer()
   {

   }

   TestInformationGatherer::~TestInformationGatherer()
   {

   }

   DiagnosticResult
   TestInformationGatherer::PerformTest()
   {
      DiagnosticResult diagResult;
      diagResult.SetName("Server details");
      diagResult.SetDescription("Collects basic server details");

      String result;

      String formattedLine;
      formattedLine.Format(_T("hMailServer version: %s\r\n"), Application::Instance()->GetVersionNumber().c_str());
      result.append(formattedLine);

      SYSTEM_INFO system_info;
      GetSystemInfo(&system_info);

      formattedLine = Formatter::Format("Logical processors: {0}\r\n", (int) system_info.dwNumberOfProcessors);
      result.append(formattedLine);

      MEMORYSTATUSEX memory_status;
      memory_status.dwLength = sizeof(memory_status);
      GlobalMemoryStatusEx(&memory_status);
      memory_status.ullTotalPhys;

      formattedLine = Formatter::Format("System memory: {0} MB\r\n", memory_status.ullTotalPhys/1024/1024);
      result.append(formattedLine);

      String databaseType = DatabaseSettings::GetDatabaseTypeName(IniFileSettings::Instance()->GetDatabaseType());
      formattedLine.Format(_T("Database type: %s\r\n"), databaseType.c_str());
      result.append(formattedLine);

      diagResult.SetSuccess(true);
      diagResult.SetDetails(result);

      return diagResult;
   }


   
      
}
