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
      static bool Scan(std::shared_ptr<Message> pMessage, String &virusName);
      static void BlockAttachments(std::shared_ptr<Message> message);

      static void ResetCounter();
      static void DecreaseCounter();
      
   private:

      static long running_scanners_;

      static void WaitForFreeScanner_();
      static void ReportVirusFound(std::shared_ptr<Message> pMessage);
      static VirusScanningResult ScanFile_(const String &fileName);

      static void ReportScanningError_(const VirusScanningResult &scanningResult);
   };

}
