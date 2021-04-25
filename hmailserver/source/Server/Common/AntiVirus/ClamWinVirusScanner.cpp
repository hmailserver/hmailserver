// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "ClamWinVirusScanner.h"

#include "../Util/ProcessLauncher.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   ClamWinVirusScanner::ClamWinVirusScanner()
   {

   }

   ClamWinVirusScanner::~ClamWinVirusScanner()
   {

   }

   VirusScanningResult 
   ClamWinVirusScanner::Scan(const String &sFilename)
   {
      String scannerExecutable = Configuration::Instance()->GetAntiVirusConfiguration().ClamWinExecutable();
      String databasePath = Configuration::Instance()->GetAntiVirusConfiguration().ClamWinDatabase();

      return Scan(scannerExecutable, databasePath, sFilename);
   }

   VirusScanningResult 
   ClamWinVirusScanner::Scan(const String &scannerExecutable, const String &databasePath, const String &sFilename)
   {
      LOG_DEBUG("Running ClamWin");

      String sPath = FileUtilities::GetFilePath(sFilename);
      String sFileToScan = FileUtilities::GetFileNameFromFullPath(sFilename);
      String sTempDir = IniFileSettings::Instance()->GetTempDirectory();

      String sCommandLine;
      sCommandLine.Format(_T("%s --database=\"%s\" \"%s\" --tempdir=\"%s\""), scannerExecutable.c_str(), databasePath.c_str(), sFileToScan.c_str(), sTempDir.c_str());

      unsigned int exitCode = 0;
      ProcessLauncher launcher(sCommandLine, sPath);
      launcher.SetErrorLogTimeout(20000);
      if (!launcher.Launch(exitCode))
      {
         return VirusScanningResult("ClamWinVirusScanner::Scan", "Unable to launch executable.");
      }

      String sDebugMessage = Formatter::Format("ClamWin: {0}. Return code: {1}", sCommandLine, exitCode);
      LOG_DEBUG(sDebugMessage);

      if (exitCode == 1)
         return VirusScanningResult(VirusScanningResult::VirusFound, "Unknown");
      else
         return VirusScanningResult(VirusScanningResult::NoVirusFound, Formatter::Format("Return code: {0}", exitCode));

   }
}