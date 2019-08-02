// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{

   class ErrorManager : public Singleton<ErrorManager>
   {
   public:
	   ErrorManager();
      virtual ~ErrorManager();

      enum eSeverity
      {
         Critical = 1,
         High = 2,
         Medium = 3,
         Low = 4
      };

      void ReportError(eSeverity iSeverity, int iErrorID, const String &sSource, const String &sDescription);
      void ReportError(eSeverity iSeverity, int iErrorID, const String &sSource, const String &sDescription, const boost::system::system_error &error);
      void ReportError(eSeverity iSeverity, int iErrorID, const String &sSource, const String &sDescription, const boost::system::error_code &error);
      void ReportError(eSeverity iSeverity, int iErrorID, const String &sSource, const String &sDescription, const std::exception &error);

      String GetWindowsErrorText(int windows_error_code);
      
      static int GetNativeErrorCode(IErrorInfo *pIErrorInfo);

   private:

   };
}
