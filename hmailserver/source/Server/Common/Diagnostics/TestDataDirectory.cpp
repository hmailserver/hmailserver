// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "TestDataDirectory.h"
#include "../Persistence/PersistentMessage.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   TestDataDirectory::TestDataDirectory()
   {

   }

   TestDataDirectory::~TestDataDirectory()
   {

   }

   DiagnosticResult
   TestDataDirectory::PerformTest()
   {
      DiagnosticResult diagResult;
      diagResult.SetName("Test message file locations");
      diagResult.SetDescription("Confirms that all email messages are located in the data directory.");

      bool allMessagesPartialNames = PersistentMessage::GetAllMessageFilesArePartialNames();
      bool allMessagesInDataDir = PersistentMessage::GetAllMessageFilesAreInDataFolder();

      String result;
      String formattedString;

      diagResult.SetSuccess(true);

      if (!allMessagesInDataDir)
      {
         result.append(Formatter::Format(_T("ERROR: Messages exists which are located outside of the data directory {0}.\r\n"), 
            IniFileSettings::Instance()->GetDataDirectory()));

         diagResult.SetSuccess(false);
      }

      if (!allMessagesPartialNames)
      {
         result.append(_T("ERROR: Full paths are stored in the database.\r\n"));
         diagResult.SetSuccess(false);
      }
      else
      {
         result.append(_T("Relative message paths are stored in the database for all messages.\r\n"));
      }

      diagResult.SetDetails(result);

      return diagResult;
   }


   
      
}
