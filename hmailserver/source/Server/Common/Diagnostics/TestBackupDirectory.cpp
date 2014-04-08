// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "TestBackupDirectory.h"
#include "../Util/Time.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   TestBackupDirectory::TestBackupDirectory()
   {

   }

   TestBackupDirectory::~TestBackupDirectory()
   {

   }

   DiagnosticResult
   TestBackupDirectory::PerformTest()
   {
      DiagnosticResult diagResult;
      diagResult.SetName("Test backup directory");
      diagResult.SetDescription("Confirms that the current backup directory is writable.");

      String backupDirectory = Configuration::Instance()->GetBackupDestination();
      if (backupDirectory.IsEmpty())
      {
         diagResult.SetSuccess(false);
         diagResult.SetDetails(_T("ERROR: Backup directory has not been specified."));
      }
      else
      {
         String timeStamp = Time::GetCurrentDateTime() + ".tmp";
         timeStamp.Replace(_T(":"), _T("-"));

         String tempFileName = FileUtilities::Combine(backupDirectory, timeStamp);

         if (FileUtilities::WriteToFile(tempFileName, String("This is a test string."), false))
         {
            diagResult.SetSuccess(true);
            diagResult.SetDetails(Formatter::Format("Backup directory {0} is writable.", backupDirectory));
         }
         else
         {
            diagResult.SetSuccess(false);
            diagResult.SetDetails(Formatter::Format("ERROR: Backup directory {0} is not writable.", backupDirectory));
         }

         // Delete the temp file.
         FileUtilities::DeleteFile(tempFileName);
         
      }

      return diagResult;
   }


   
      
}
