// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once


namespace HM
{
   class VirusScanningResult
   {
   public:

      enum ScanningResult
      {
         ErrorOccurred = 1,
         VirusFound = 2,
         NoVirusFound = 3
      };
   
      VirusScanningResult(ScanningResult result, const String &details);
      VirusScanningResult(const String &errorMessageSource, const String &errorMessage);

      bool GetVirusFound() const {return result_ == VirusFound;}
      bool GetErrorOccured() const {return result_ == ErrorOccurred;}
      String GetErrorMessageSource() const {return message_source_;}
      String GetDetails() const {return details_;}

   private:

      ScanningResult result_;
      
      String message_source_;
      String details_;
   };

}