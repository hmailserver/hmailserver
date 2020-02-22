// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "VirusScannerTester.h"
#include "CustomVirusScanner.h"
#include "ClamWinVirusScanner.h"
#include "ClamAVVirusScanner.h"
#include "VirusScanningResult.h"

#include "../Util/GUIDCreator.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   bool 
   VirusScannerTester::TestClamAVConnect(const String &hostName, int port, String &message)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   //---------------------------------------------------------------------------()
   {
      ClamAVVirusScanner scanner;
      
      String testFile = GeneratePlainTestFile_();
      VirusScanningResult result = scanner.Scan(hostName, port, testFile);
      FileUtilities::DeleteFile(testFile);

      if (result.GetVirusFound())
      {
         message = "False positive: " + result.GetDetails();
         return false;
      }
      
      testFile = GenerateVirusTestFile_();
      result = scanner.Scan(hostName, port, testFile);
      FileUtilities::DeleteFile(testFile);
      message = result.GetDetails();

      return result.GetVirusFound();
   }

   bool 
   VirusScannerTester::TestCustomVirusScanner(const String &executable, int returnValue, String &message)
   {
      CustomVirusScanner scanner;

      String testFile = GeneratePlainTestFile_();
      VirusScanningResult result = scanner.Scan(executable, returnValue, testFile);
      FileUtilities::DeleteFile(testFile);

      if (result.GetVirusFound())
      {
         message = "False positive: " + result.GetDetails();
         return false;
      }

      testFile = GenerateVirusTestFile_();
      result = scanner.Scan(executable, returnValue, testFile);
      FileUtilities::DeleteFile(testFile);
      message = result.GetDetails();
      return result.GetVirusFound();
   }

   bool 
   VirusScannerTester::TestClamWinVirusScanner(const String &executable, const String &databasePath, String &message)
   {
      ClamWinVirusScanner scanner;

      String testFile = GeneratePlainTestFile_();
      VirusScanningResult result = scanner.Scan(executable, databasePath, testFile);
      FileUtilities::DeleteFile(testFile);

      if (result.GetVirusFound())
      {
         message = "False positive: " + result.GetDetails();
         return false;
      }


      testFile = GenerateVirusTestFile_();
      result = scanner.Scan(executable, databasePath, testFile);
      FileUtilities::DeleteFile(testFile);
      message = result.GetDetails();
      return result.GetVirusFound();
   }

   String
   VirusScannerTester::GenerateVirusTestFile_()
   {
      // Store the test virus in reversed form so we don't trigger any virus scanner...
      String eicarTestString = " *H+H$!ELIF-TSET-SURIVITNA-DRADNATS-RACIE$}7)CC7)^P(45XZP\\4[PA@%P!O5X";      

      // Write the test virus to the data directory to simulate email.
      String dataDir = IniFileSettings::Instance()->GetDataDirectory();

      String messageFileName = GUIDCreator::GetGUID() + ".eml";
      String fullPathToMessage = FileUtilities::Combine(dataDir, messageFileName);

      eicarTestString.MakeReverse();
      FileUtilities::WriteToFile(fullPathToMessage, eicarTestString, false);

      return fullPathToMessage;
   }

   String
   VirusScannerTester::GeneratePlainTestFile_()
   {
      // Store the test virus in reversed form so we don't trigger any virus scanner...
      String testString = "Test";

      // Write the test virus to the data directory to simulate email.
      String dataDir = IniFileSettings::Instance()->GetDataDirectory();

      String messageFileName = GUIDCreator::GetGUID() + ".eml";
      String fullPathToMessage = FileUtilities::Combine(dataDir, messageFileName);

      FileUtilities::WriteToFile(fullPathToMessage, testString, false);

      return fullPathToMessage;
   }
}
