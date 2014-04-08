// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "VirusScanningResult.h"

namespace HM
{
   class Message;

   class VirusScanner  
   {
   public:
   
      class VirusScannerAutoCount
      {
      public:
         ~VirusScannerAutoCount()
         {
            VirusScanner::DecreaseCounter();
         }
      };
      
      enum Constants
      {
         MaxRunningScanners = 10
      };


      static bool GetVirusScanningEnabled();
      static bool Scan(shared_ptr<Message> pMessage, String &virusName);
      static void BlockAttachments(shared_ptr<Message> pMessage);

      static void ResetCounter();
      static void DecreaseCounter();
      
   private:

      static long _runningScanners;

      static void _WaitForFreeScanner();
      static void ReportVirusFound(shared_ptr<Message> pMessage);
      static VirusScanningResult _ScanFile(const String &fileName);

      static void _ReportScanningError(const VirusScanningResult &scanningResult);
   };

}
