// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "../Util/Time.h"
#include "../Util/File.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   
   Logger::Logger()
   {
      log_mask_ = 0;
      enable_live_log_ = false;

      auto ini_file_settings = IniFileSettings::Instance();

      log_dir_ = ini_file_settings->GetLogDirectory();
      sep_svc_logs_ = ini_file_settings->GetSepSvcLogs();
   }

   Logger::~Logger(void)
   {
   }

   void 
   Logger::SetLogMask(int iMask)
   {
      if (iMask & LSEnabled)
         log_mask_ = iMask;
      else
         log_mask_ = 0;
   }

   bool 
   Logger::GetLoggingEnabled() const
   {
      if (log_mask_ & LSEnabled)
         return true;
      else
         return false;
   }

   void 
   Logger::LogSMTPConversation(int iSessionID, const String &sRemoteHost, const String &sMessage, bool bClient)
   {
      if (!(log_mask_ & LSSMTP))
         return; // not intressted in this...   

      long lThread = GetThreadID_();
      String sTime = GetCurrentTime();

      String sData;

      if (bClient)
         sData.Format(_T("\"SMTPC\"\t%d\t%d\t\"%s\"\t\"%s\"\t\"%s\"\r\n"), lThread, iSessionID, sTime.c_str(), sRemoteHost.c_str(), CleanLogMessage_(sMessage).c_str());
      else
         sData.Format(_T("\"SMTPD\"\t%d\t%d\t\"%s\"\t\"%s\"\t\"%s\"\r\n"), lThread, iSessionID, sTime.c_str(), sRemoteHost.c_str(), CleanLogMessage_(sMessage).c_str());

      if (enable_live_log_)
         LogLive_(sData);

      WriteData_(sData, SMTP);
   
   }

   void 
   Logger::LogPOP3Conversation(int iSessionID, const String &sRemoteHost, const String &sMessage, bool bClient)
   {
      if (!(log_mask_ & LSPOP3))
         return; // not intressted in this...   

      long lThread = GetThreadID_();
      String sTime = GetCurrentTime();

      String sData;
      // Seems this was never done so now external account activity logs as client
      if (bClient)
         sData.Format(_T("\"POP3C\"\t%d\t%d\t\"%s\"\t\"%s\"\t\"%s\"\r\n"), lThread, iSessionID, sTime.c_str(), sRemoteHost, CleanLogMessage_(sMessage).c_str());
      else
         sData.Format(_T("\"POP3D\"\t%d\t%d\t\"%s\"\t\"%s\"\t\"%s\"\r\n"), lThread, iSessionID, sTime.c_str(), sRemoteHost, CleanLogMessage_(sMessage).c_str());

      if (enable_live_log_)
         LogLive_(sData);

      WriteData_(sData, POP3);
   
   }

   void 
   Logger::LogIMAPConversation(int iSessionID, const String &sRemoteHost, const String &sMessage)
   {
      if (!(log_mask_ & LSIMAP))
         return; // not intressted in this...   

      long lThread = GetThreadID_();
      String sTime = GetCurrentTime();

      String sData;
      sData.Format(_T("\"IMAPD\"\t%d\t%d\t\"%s\"\t\"%s\"\t\"%s\"\r\n"), lThread, iSessionID, sTime.c_str(), sRemoteHost.c_str(), CleanLogMessage_(sMessage).c_str());

      if (enable_live_log_)
         LogLive_(sData);

      WriteData_(sData, IMAP);
   
   }

   void 
   Logger::LogApplication(const String &sMessage)
   {
      LogApplication(sMessage, false);
   }

   void 
   Logger::LogApplication(const String &sMessage, bool isError)
   {
#ifndef _DEBUG
      if (!(log_mask_ & LSApplication))
         return; // not intressted in this...   
#endif

      long lThread = GetThreadID_();
      String sTime = GetCurrentTime();

      String sData;
      sData.Format(_T("\"APPLICATION\"\t%d\t\"%s\"\t\"%s\"\r\n"), lThread, sTime.c_str(), CleanLogMessage_(sMessage).c_str());

#ifdef _DEBUG
      OutputDebugString(sData);
#endif

      if (enable_live_log_)
         LogLive_(sData);

      if (log_mask_ & LSApplication)
         WriteData_(sData);
   }

   void 
   Logger::LogDebug(const String &sMessage)
   {
      if (!(log_mask_ & LSDebug))
         return; // not intressted in this...   

      long lThread = GetThreadID_();
      String sTime = GetCurrentTime();

      String sData;
      sData.Format(_T("\"DEBUG\"\t%d\t\"%s\"\t\"%s\"\r\n"), lThread, sTime.c_str(), CleanLogMessage_(sMessage).c_str());

      if (enable_live_log_)
         LogLive_(sData);


      WriteData_(sData);
   
   }

   
   void 
   Logger::LogError(const String &sMessage)
   {
      long lThread = GetThreadID_();
      String sTime = GetCurrentTime();

      String sData;
      sData.Format(_T("\"ERROR\"\t%d\t\"%s\"\t\"%s\"\r\n"), lThread, sTime.c_str(), CleanLogMessage_(sMessage).c_str());

      if (enable_live_log_)
         LogLive_(sData);

      WriteData_(sData, Error);

      // Also log this in the application log if some other logging is enabled.
      if (GetLoggingEnabled())
         WriteData_(sData, Normal);
   }


   void 
   Logger::LogTCPIP(const String &sMessage)
   {
      if (!(log_mask_ & LSTCPIP))
         return; // not intressted in this...   

      long lThread = GetThreadID_();
      String sTime = GetCurrentTime();

      String sData;
      sData.Format(_T("\"TCPIP\"\t%d\t\"%s\"\t\"%s\"\r\n"), lThread, sTime.c_str(), CleanLogMessage_(sMessage).c_str());

      if (enable_live_log_)
         LogLive_(sData);

      WriteData_(sData);
   }

   void 
   Logger::LogEvent(const String &sMessage)
   {
      long lThread = GetThreadID_();
      String sTime = GetCurrentTime();

      String sData;
      sData.Format(_T("%d\t\"%s\"\t\"%s\"\r\n"), lThread, sTime.c_str(), CleanLogMessage_(sMessage).c_str());

      WriteData_(sData, Events);
   }

   String 
   Logger::CleanLogMessage_(const String &message)
   {
      String result = message;
      result.Replace(_T("\r\n"), _T("[nl]"));
      return result;
   }

   String 
   Logger::GetCurrentLogFileName(LogType lt) 
   {
      String sFilename;
      String theTime = Time::GetCurrentDate();

      switch (lt)
      {
      case Normal:
         sFilename.Format(_T("%s\\hmailserver_%s.log"), log_dir_.c_str(), theTime.c_str());
         break;
      case Error:
         sFilename.Format(_T("%s\\ERROR_hmailserver_%s.log"), log_dir_.c_str(), theTime.c_str());
         break;
      case AWStats:
         sFilename.Format(_T("%s\\hmailserver_awstats.log"), log_dir_.c_str());
         break;
      case Backup:
         sFilename.Format(_T("%s\\hmailserver_backup.log"), log_dir_.c_str());
         break;
      case Events:
         sFilename.Format(_T("%s\\hmailserver_events.log"), log_dir_.c_str());
         break;
      case IMAP:
         if (sep_svc_logs_) 
            sFilename.Format(_T("%s\\hmailserver_IMAP_%s.log"), log_dir_.c_str(), theTime.c_str());
         else
            sFilename.Format(_T("%s\\hmailserver_%s.log"), log_dir_.c_str(), theTime.c_str());
         break;
      case POP3:
         if (sep_svc_logs_) 
            sFilename.Format(_T("%s\\hmailserver_POP3_%s.log"), log_dir_.c_str(), theTime.c_str());
         else
            sFilename.Format(_T("%s\\hmailserver_%s.log"), log_dir_.c_str(), theTime.c_str());
         break;
      case SMTP:
         if (sep_svc_logs_) 
            sFilename.Format(_T("%s\\hmailserver_SMTP_%s.log"), log_dir_.c_str(), theTime.c_str());
         else
            sFilename.Format(_T("%s\\hmailserver_%s.log"), log_dir_.c_str(), theTime.c_str());
         break;
      }

      return sFilename;
   }

   File*
   Logger::GetCurrentLogFile_(LogType lt)
   {
      String fileName = GetCurrentLogFileName(lt);
      sep_svc_logs_ = IniFileSettings::Instance()->GetSepSvcLogs();

      bool writeUnicode = false;

      File *file = 0;
      switch (lt)
      {
      case Normal:
         file = &normal_log_file_;
         writeUnicode = false;
         break;
      case Error:
         file = &error_log_file_;
         writeUnicode = false;
         break;
      case AWStats:
         file = &awstats_log_file_;
         writeUnicode = false;
         break;
      case Backup:
         file = &backup_log_file_;
         writeUnicode = true;
         break;
      case Events:
         file = &events_log_file_;
         writeUnicode = true;
         break;
      case IMAP:
         if (sep_svc_logs_) 
            file = &imaplog_file_;
         else
            file = &normal_log_file_;
         writeUnicode = false;
         break;
      case POP3:
         if (sep_svc_logs_) 
            file = &pop3log_file_;
         else
            file = &normal_log_file_;
         writeUnicode = false;
         break;
      case SMTP:
         if (sep_svc_logs_) 
            file = &smtplog_file_;
         else
            file = &normal_log_file_;
         writeUnicode = false;
         break;
      }

      bool fileExists = false;

      try
      {
         bool fileExists = FileUtilities::Exists(fileName);
      }
      catch (boost::system::system_error&)
      {
         // If the log is not accessible for some reason, such as error code 5 - access is denied, 
         // then we can't open the log file. This will result in nothing being logged.
         return nullptr;
      }

      if (file->IsOpen())
      {
         if (file->GetName() != fileName)
            file->Close();
         else if (!fileExists)
            file->Close();
      }
      
      if (!file->IsOpen())
      {
         if (!file->Open(fileName, File::OTAppend))
            return nullptr;

         if (!fileExists && writeUnicode)
         {
            file->WriteBOF();
         }
      }

      return file;
   }

   void
   Logger::WriteData_(const String &sData, LogType lt)
   {
      boost::lock_guard<boost::recursive_mutex> guard(mtx_);

      File *file = GetCurrentLogFile_(lt);
      if (file == nullptr)
         return;

      bool writeUnicode = false;
      bool keepFileOpen = (log_mask_ & LSKeepFilesOpen) && (lt == Normal || lt == SMTP || lt == POP3 || lt == IMAP);

      switch (lt)
      {
      case Normal:
      case Error:
      case AWStats:
      case IMAP:
      case POP3:
      case SMTP:
         writeUnicode = false;
         break;
      case Backup:
      case Events:
         writeUnicode = true;
         break;
      }

      if (writeUnicode)
      {
         file->Write(sData);
      }
      else
      {
         AnsiString sAnsiString;
         // Let's truncate some of those crazy long log lines
         // only when debug is not enabled or loglevel <= 2
         // Only do it if long enough default is 500 by set by MaxLogLineLen
         
         int iDataLenTmp = sData.GetLength();
         log_level_ = IniFileSettings::Instance()->GetLogLevel();
         max_log_line_len_ = IniFileSettings::Instance()->GetMaxLogLineLen();

         if ((Logger::Instance()->GetLogDebug()) || (log_level_ > 2) || (iDataLenTmp < max_log_line_len_ ))
            sAnsiString = sData;
         else
            sAnsiString = sData.Mid(0, max_log_line_len_ - 30) + " ... " + sData.Mid(iDataLenTmp - 25);
            // We keep 25 of end which includes crlf but need to account for middle ... too

         file->Write(sAnsiString);
      }

      if (!keepFileOpen)
         file->Close();
   }


   String
   Logger::GetCurrentTime()
   {
      return Time::GetCurrentDateTimeWithMilliseconds();

   }

   int 
   Logger::GetProcessID_()
   {
      DWORD dwProcessID = GetCurrentProcessId();
      return dwProcessID;
   }

   int 
   Logger::GetThreadID_()
   {
      DWORD dwThreadID = GetCurrentThreadId();
      return dwThreadID;

   }

   void
   Logger::LogLive_(String &sMessage)
   {
      boost::lock_guard<boost::recursive_mutex> guard(mtx_LiveLog);

      // Check if we are still enabled. It could be that 
      // we have just disabled ourselves.
      if (!enable_live_log_)
         return;

      live_log_ += sMessage;

      // Check if the live log listeners has stopped listening without telling us.
      if (live_log_.GetLength() > LiveLogMaxSize)
      {
         live_log_.Empty();
         enable_live_log_ = false;
      }
   }

   void 
   Logger::EnableLiveLogging(bool bEnable)
   {
      boost::lock_guard<boost::recursive_mutex> guard(mtx_LiveLog);
      live_log_.Empty();

      enable_live_log_ = bEnable;
   }

   bool 
   Logger::GetLogDebug() const
   {
      return (log_mask_  & LSDebug) > 0; 
   }

   bool 
   Logger::GetLogApplication() const
   {
      return (log_mask_  & LSApplication) > 0; 
   }

   bool 
   Logger::GetLogTCPIP() const
   {
      return (log_mask_  & LSTCPIP) > 0; 
   }

   bool 
   Logger::GetLiveLogEnabled() const
   {
      return enable_live_log_; 
   }
   
   String 
   Logger::GetLiveLog()
   {
      boost::lock_guard<boost::recursive_mutex> guard(mtx_LiveLog);

      String sResult;
      sResult = live_log_;
      
      live_log_.Empty();

      return sResult;
   }

   void
   Logger::LogAWStats(const String &sData)
   {
      WriteData_(sData, AWStats);
   }

   void
   Logger::LogBackup(const String &sData)
   {
      String sTime = GetCurrentTime();
      String sLogMessage;

      sLogMessage.Format(_T("%s\t%s\r\n"), sTime.c_str(), sData.c_str());
      WriteData_(sLogMessage, Backup);
   }
}
