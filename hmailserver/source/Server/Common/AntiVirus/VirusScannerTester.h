// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class VirusScannerTester
   {
   public:
      bool TestClamAVConnect(const String &hostName, int port, String &message);
      bool TestCustomVirusScanner(const String &executable, int returnValue, String &message);
      bool TestClamWinVirusScanner(const String &executable, const String &databasePath, String &message);

   private:
      String GenerateVirusTestFile_();
      String GeneratePlainTestFile_();
   };

}