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
   Logger* Logger::pInstanceApp = NULL;

   void
   Logger::CreateInstance()
   {
      pInstanceApp = new Logger();
   }

   void
   Logger::DeleteInstance()
   {
      delete pInstanceApp;
      pInstanceApp = 0;
   }

   Logger* 
   Logger::Instance()
   {
      return pInstanceApp;
   }

   Logger::Logger()
   {
      m_iLogMask = 0;
      m_bEnableLiveLog = false;

      m_sLogDir = IniFileSettings::GetLogDirectory();
   }

   Logger::~Logger(void)
   {
   }

   void 
   Logger::SetLogMask(int iMask)
   {
      if (iMask & LSEnabled)
         m_iLogMask = iMask;
      else
         m_iLogMask = 0;
   }

   bool 
   Logger::GetLoggingEnabled() const
   {
      if (m_iLogMask & LSEnabled)
         return true;
      else
         return false;
   }

   void 
   Logger::LogSMTPConversation(int iSessionID, const String &sRemoteHost, const String &sMessage, bool bClient)
   {
      if (!(m_iLogMask & LSSMTP))
         return; // not intressted in this...   

      long lThread = _GetThreadID();
      String sTime = GetCurrentTime();

      String sData;
      if (bClient)
         sData.Format(_T("\"SMTPC\"\t%d\t%d\t\"%s\"\t\"%s\"\t\"%s\"\r\n"), lThread, iSessionID, sTime, sRemoteHost, sMessage);
      else
         sData.Format(_T("\"SMTPD\"\t%d\t%d\t\"%s\"\t\"%s\"\t\"%s\"\r\n"), lThread, iSessionID,sTime, sRemoteHost, sMessage);

      if (m_bEnableLiveLog)
         _LogLive(sData);

      _WriteData(sData, SMTP);
   
   }

   void 
   Logger::LogPOP3Conversation(int iSessionID, const String &sRemoteHost, const String &sMessage, bool bClient)
   {
      if (!(m_iLogMask & LSPOP3))
         return; // not intressted in this...   

      long lThread = _GetThreadID();
      String sTime = GetCurrentTime();

      String sData;
      // Seems this was never done so now external account activity logs as client
      if (bClient)
         sData.Format(_T("\"POP3C\"\t%d\t%d\t\"%s\"\t\"%s\"\t\"%s\"\r\n"), lThread, iSessionID, sTime, sRemoteHost, sMessage);
      else
         sData.Format(_T("\"POP3D\"\t%d\t%d\t\"%s\"\t\"%s\"\t\"%s\"\r\n"), lThread, iSessionID, sTime, sRemoteHost, sMessage);

      if (m_bEnableLiveLog)
         _LogLive(sData);

      _WriteData(sData, POP3);
   
   }

   void 
   Logger::LogIMAPConversation(int iSessionID, const String &sRemoteHost, const String &sMessage)
   {
      if (!(m_iLogMask & LSIMAP))
         return; // not intressted in this...   

      long lThread = _GetThreadID();
      String sTime = GetCurrentTime();

      String sData;
      sData.Format(_T("\"IMAPD\"\t%d\t%d\t\"%s\"\t\"%s\"\t\"%s\"\r\n"), lThread, iSessionID,sTime, sRemoteHost, sMessage);

      if (m_bEnableLiveLog)
         _LogLive(sData);

      _WriteData(sData, IMAP);
   
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
      if (!(m_iLogMask & LSApplication))
         return; // not intressted in this...   
#endif

      long lThread = _GetThreadID();
      String sTime = GetCurrentTime();

      String sData;
      sData.Format(_T("\"APPLICATION\"\t%d\t\"%s\"\t\"%s\"\r\n"), lThread, sTime, sMessage);

#ifdef _DEBUG
      OutputDebugString(sData);
#endif

      if (m_bEnableLiveLog)
         _LogLive(sData);

      if (m_iLogMask & LSApplication)
         _WriteData(sData);
   }

   void 
   Logger::LogDebug(const String &sMessage)
   {
      if (!(m_iLogMask & LSDebug))
         return; // not intressted in this...   

      long lThread = _GetThreadID();
      String sTime = GetCurrentTime();

      String sData;
      sData.Format(_T("\"DEBUG\"\t%d\t\"%s\"\t\"%s\"\r\n"), lThread, sTime, sMessage);

      if (m_bEnableLiveLog)
         _LogLive(sData);


      _WriteData(sData);
   
   }

   
   void 
   Logger::LogError(const String &sMessage)
   {
      long lThread = _GetThreadID();
      String sTime = GetCurrentTime();

      String sData;
      sData.Format(_T("\"ERROR\"\t%d\t\"%s\"\t\"%s\"\r\n"), lThread, sTime, sMessage);

      if (m_bEnableLiveLog)
         _LogLive(sData);

      _WriteData(sData, Error);

      // Also log this in the application log if some other logging is enabled.
      if (GetLoggingEnabled())
         _WriteData(sData, Normal);
   }


   void 
   Logger::LogTCPIP(const String &sMessage)
   {
      if (!(m_iLogMask & LSTCPIP))
         return; // not intressted in this...   

      long lThread = _GetThreadID();
      String sTime = GetCurrentTime();


      String sData;
      sData.Format(_T("\"TCPIP\"\t%d\t\"%s\"\t\"%s\"\r\n"), lThread, sTime, sMessage);

      if (m_bEnableLiveLog)
         _LogLive(sData);

      _WriteData(sData);
   }

   void 
   Logger::LogEvent(const String &sMessage)
   {
      long lThread = _GetThreadID();
      String sTime = GetCurrentTime();

      String sData;
      sData.Format(_T("%d\t\"%s\"\t\"%s\"\r\n"), lThread, sTime, sMessage);

      _WriteData(sData, Events);
   }

   String 
   Logger::GetCurrentLogFileName(LogType lt) 
   {
      String sFilename;

      String theTime = Time::GetCurrentDate();

      m_bSepSvcLogs = IniFileSettings::Instance()->GetSepSvcLogs();

      switch (lt)
      {
      case Normal:
         sFilename.Format(_T("%s\\hmailserver_%s.log"), m_sLogDir, theTime );
         break;
      case Error:
         sFilename.Format(_T("%s\\ERROR_hmailserver_%s.log"), m_sLogDir, theTime );
         break;
      case AWStats:
         sFilename.Format(_T("%s\\hmailserver_awstats.log"), m_sLogDir );
         break;
      case Backup:
         sFilename.Format(_T("%s\\hmailserver_backup.log"), m_sLogDir );
         break;
      case Events:
         sFilename.Format(_T("%s\\hmailserver_events.log"), m_sLogDir );
         break;
      case IMAP:
         if (m_bSepSvcLogs) 
            sFilename.Format(_T("%s\\hmailserver_IMAP_%s.log"), m_sLogDir, theTime );
         else
            sFilename.Format(_T("%s\\hmailserver_%s.log"), m_sLogDir, theTime );
         break;
      case POP3:
         if (m_bSepSvcLogs) 
            sFilename.Format(_T("%s\\hmailserver_POP3_%s.log"), m_sLogDir, theTime );
         else
            sFilename.Format(_T("%s\\hmailserver_%s.log"), m_sLogDir, theTime );
         break;
      case SMTP:
         if (m_bSepSvcLogs) 
            sFilename.Format(_T("%s\\hmailserver_SMTP_%s.log"), m_sLogDir, theTime );
         else
            sFilename.Format(_T("%s\\hmailserver_%s.log"), m_sLogDir, theTime );
         break;
      }

      return sFilename;
   }

   File*
   Logger::_GetCurrentLogFile(LogType lt)
   {
      String fileName = GetCurrentLogFileName(lt);
      m_bSepSvcLogs = IniFileSettings::Instance()->GetSepSvcLogs();

      bool writeUnicode = false;

      File *file = 0;
      switch (lt)
      {
      case Normal:
         file = &_normalLogFile;
         writeUnicode = false;
         break;
      case Error:
         file = &_errorLogFile;
         writeUnicode = false;
         break;
      case AWStats:
         file = &_awstatsLogFile;
         writeUnicode = false;
         break;
      case Backup:
         file = &_backupLogFile;
         writeUnicode = true;
         break;
      case Events:
         file = &_eventsLogFile;
         writeUnicode = true;
         break;
      case IMAP:
         if (m_bSepSvcLogs) 
            file = &_IMAPLogFile;
         else
            file = &_normalLogFile;
         writeUnicode = false;
         break;
      case POP3:
         if (m_bSepSvcLogs) 
            file = &_POP3LogFile;
         else
            file = &_normalLogFile;
         writeUnicode = false;
         break;
      case SMTP:
         if (m_bSepSvcLogs) 
            file = &_SMTPLogFile;
         else
            file = &_normalLogFile;
         writeUnicode = false;
         break;
      }

      bool fileExists = FileUtilities::Exists(fileName);

      if (file->IsOpen())
      {
         if (file->GetName() != fileName)
            file->Close();
         else if (!fileExists)
            file->Close();
      }
      
      if (!file->IsOpen())
      {
         file->Open(fileName, File::OTAppend);

         if (!fileExists && writeUnicode)
         {
            file->WriteBOF();
         }
      }

      return file;
   }

   bool
   Logger::_WriteData(const String &sData, LogType lt)
   {
      CriticalSectionScope scope(m_oCritSec);

      File *file = _GetCurrentLogFile(lt);

      bool writeUnicode = false;
      bool keepFileOpen = (m_iLogMask & LSKeepFilesOpen) && (lt == Normal || lt == SMTP || lt == POP3 || lt == IMAP);

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
         m_iLogLevel = IniFileSettings::Instance()->GetLogLevel();
         m_iMaxLogLineLen = IniFileSettings::Instance()->GetMaxLogLineLen();

         if ((Logger::Instance()->GetLogDebug()) || (m_iLogLevel > 2) || (iDataLenTmp < m_iMaxLogLineLen ))
            sAnsiString = sData;
         else
            sAnsiString = sData.Mid(0, m_iMaxLogLineLen - 30) + " ... " + sData.Mid(iDataLenTmp - 25);
            // We keep 25 of end which includes crlf but need to account for middle ... too

         file->Write(sAnsiString);
      }

      if (!keepFileOpen)
         file->Close();

      return true;
   }


   String
   Logger::GetCurrentTime()
   {
      return Time::GetCurrentDateTimeWithMilliseconds();

   }

   int 
   Logger::_GetProcessID()
   {
      DWORD dwProcessID = GetCurrentProcessId();
      return dwProcessID;
   }

   int 
   Logger::_GetThreadID()
   {
      DWORD dwThreadID = GetCurrentThreadId();
      return dwThreadID;

   }

   void
   Logger::_LogLive(String &sMessage)
   {
      CriticalSectionScope scope(m_oCritSecLiveLog);

      // Check if we are still enabled. It could be that 
      // we have just disabled ourselves.
      if (!m_bEnableLiveLog)
         return;

      m_sLiveLog += sMessage;

      // Check if the live log listeners has stopped listening without telling us.
      if (m_sLiveLog.GetLength() > LiveLogMaxSize)
      {
         m_sLiveLog.Empty();
         m_bEnableLiveLog = false;
      }
   }

   void 
   Logger::EnableLiveLogging(bool bEnable)
   {
      CriticalSectionScope scope(m_oCritSecLiveLog);
      m_sLiveLog.Empty();

      m_bEnableLiveLog = bEnable;
   }

   bool 
   Logger::GetLogDebug() const
   {
      return (m_iLogMask  & LSDebug) > 0; 
   }

   bool 
   Logger::GetLogApplication() const
   {
      return (m_iLogMask  & LSApplication) > 0; 
   }

   bool 
   Logger::GetLogTCPIP() const
   {
      return (m_iLogMask  & LSTCPIP) > 0; 
   }

   bool 
   Logger::GetLiveLogEnabled() const
   {
      return m_bEnableLiveLog; 
   }
   
   String 
   Logger::GetLiveLog()
   {
      CriticalSectionScope scope(m_oCritSecLiveLog);

      String sResult;
      sResult = m_sLiveLog;
      
      m_sLiveLog.Empty();

      return sResult;
   }

   void
   Logger::LogAWStats(const String &sData)
   {
      _WriteData(sData, AWStats);
   }

   void
   Logger::LogBackup(const String &sData)
   {
      String sTime = GetCurrentTime();
      String sLogMessage;

      sLogMessage.Format(_T("%s\t%s\r\n"), sTime, sData);
      _WriteData(sLogMessage, Backup);
   }
}
