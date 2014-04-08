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

      bool GetVirusFound() const {return m_scanningResult == VirusFound;}
      bool GetErrorOccured() const {return m_scanningResult == ErrorOccurred;}
      String GetErrorMessageSource() const {return m_errorMessageSource;}
      String GetDetails() const {return m_details;}

   private:

      ScanningResult m_scanningResult;
      
      String m_errorMessageSource;
      String m_details;
   };

}