// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "ErrorManager.h"

#include <oledb.h>


#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif


#ifndef _ERROR_LOGGING_IN_MESSAGE_BOXES
   #include "ScriptingHost/ScriptServer.h"
   #include "ScriptingHost/ScriptObjectContainer.h"
#endif

namespace HM
{
   ErrorManager::ErrorManager()
   {

   }

   ErrorManager::~ErrorManager()
   {

   }

   String
   GetSeverity(ErrorManager::eSeverity iSev)
   {
      switch (iSev)
      {
      case ErrorManager::Critical:
         return "Critical";
      case ErrorManager::High:
         return "High";
      case ErrorManager::Medium:
         return "Medium";
      case ErrorManager::Low:
         return "Low";
      default:
         return "Unknown";
      }
   }

   int 
   ErrorManager::GetNativeErrorCode(IErrorInfo *pIErrorInfo)
   {
      int iRetValue = 0;
      HRESULT hr                          = S_OK;
      IErrorRecords    *pIErrorRecords    = NULL;
      ERRORINFO        errorInfo          = { 0 };
      IErrorInfo       *pIErrorInfoRecord = NULL;

      try
      {
         hr = pIErrorInfo->QueryInterface(IID_IErrorRecords, (void **) &pIErrorRecords);
         if ( FAILED(hr) || NULL == pIErrorRecords )
            return -1;

         pIErrorInfo->Release();
         pIErrorInfo = NULL;

         ULONG ulNumErrorRecs = 0;

         hr = pIErrorRecords->GetRecordCount(&ulNumErrorRecs);
         if ( FAILED(hr) )
            return -1;


         for (DWORD dwErrorIndex = 0; dwErrorIndex < ulNumErrorRecs; dwErrorIndex++)
         {
            hr = pIErrorRecords->GetBasicErrorInfo(dwErrorIndex, &errorInfo);

            if ( FAILED(hr) )
               return -1;

            iRetValue = errorInfo.dwMinor;
         }
      }
      catch( ... )
      {
         iRetValue = errorInfo.dwMinor;
      }

      if( pIErrorInfoRecord )
         pIErrorInfoRecord->Release();

      if ( pIErrorInfo )
         pIErrorInfo->Release();

      if ( pIErrorRecords )
         pIErrorRecords->Release();

      return iRetValue;
   }

   void 
   ErrorManager::ReportError(eSeverity iSeverity, int iErrorID, const String &sSource, const String &sDescription)
   {
#ifdef _ERROR_LOGGING_IN_MESSAGE_BOXES
      
      String sErrorToLog;
      sErrorToLog.Format(_T("%s"), 
                             sDescription);

      MessageBoxW(0, sErrorToLog, _T("hMailServer"), 0);

#else
      String sSeverityStr = GetSeverity(iSeverity);

      String sTempDesc = sDescription;
      sTempDesc.Replace(_T("\r\n"), _T("[nl]"));

      String sErrorToLog;
      sErrorToLog.Format(_T("Severity: %d (%s), Code: HM%d, Source: %s, Description: %s"), 
                            iSeverity, sSeverityStr, iErrorID, sSource, sTempDesc);

      Logger::Instance()->LogError(sErrorToLog); 

      // Send an event if we've been able to load our settings. During database
      // creation, we don't have any PropertySet in the cache but we should still
      // be able to report errors. During server start up, we have a property set (?)
      // but it's not initialized. So we need to be a bit careful here.
      if (Configuration::Instance()&& 
          Configuration::Instance()->GetPropertySet() && 
          Configuration::Instance()->GetUseScriptServer())
      {
         String sEventCaller;
         
         String sScriptLanguage = Configuration::Instance()->GetScriptLanguage();

         if (sScriptLanguage == _T("VBScript"))
         {
            String tempSource = sSource;
            String tempDescription = sDescription;
            
            tempSource.Replace(_T("\""), _T("\"\""));
            tempDescription.Replace(_T("\""), _T("\"\""));

            sEventCaller.Format(_T("OnError(%d, %d, \"%s\", \"%s\")"), 
                                    iSeverity, iErrorID, tempSource, tempDescription);
         }
         else if (sScriptLanguage == _T("JScript"))
         {
            String tempSource = sSource;
            String tempDescription = sDescription;

            tempSource.Replace(_T("'"), _T("\\'"));
            tempDescription.Replace(_T("'"), _T("\\'"));

            sEventCaller.Format(_T("OnError(%d, %d, '%s', '%s')"), 
                                    iSeverity, iErrorID, tempSource, tempDescription);
         }

         shared_ptr<ScriptObjectContainer> pContainer  = shared_ptr<ScriptObjectContainer>(new ScriptObjectContainer);
         
         ScriptServer::Instance()->FireEvent(ScriptServer::EventOnError, sEventCaller, pContainer);

      }
#endif
   }


}
