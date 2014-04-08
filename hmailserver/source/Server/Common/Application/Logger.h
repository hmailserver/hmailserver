// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../Util/File.h"
#include "..\Application\IniFileSettings.h"

namespace HM
{

   #define LOG_DEBUG(s)                                       \
   if (Logger::Instance()->GetLogMask() & Logger::LSDebug)    \
      Logger::Instance()->LogDebug(s);                        \

   #define LOG_TCPIP(s)                                       \
   if (Logger::Instance()->GetLogMask() & Logger::LSTCPIP)    \
      Logger::Instance()->LogTCPIP(s);                        \

   #define LOG_APPLICATION(s)                                 \
   if (Logger::Instance()->GetLogMask() & Logger::LSApplication)   \
      Logger::Instance()->LogApplication(s);                   \

   #define LOG_SMTP(iSession, sIP, sMsg)                       \
   if (Logger::Instance()->GetLogMask() & Logger::LSSMTP)      \
      Logger::Instance()->LogSMTPConversation(iSession, sIP, sMsg);      \

   #define LOG_SMTP_CLIENT(iSession, sIP, sMsg)                          \
   if (Logger::Instance()->GetLogMask() & Logger::LSSMTP)      \
      Logger::Instance()->LogSMTPConversation(iSession, sIP, sMsg,true); \

   #define LOG_POP3(iSession,sIP, sMsg)                                 \
   if (Logger::Instance()->GetLogMask() & Logger::LSPOP3)      \
      Logger::Instance()->LogPOP3Conversation(iSession,sIP, sMsg);      \

   #define LOG_POP3_CLIENT(iSession,sIP, sMsg)                          \
   if (Logger::Instance()->GetLogMask() & Logger::LSPOP3)      \
      Logger::Instance()->LogPOP3Conversation(iSession,sIP, sMsg, true);\

   #define LOG_IMAP(iSession,sIP, sMsg)                                 \
   if (Logger::Instance()->GetLogMask() & Logger::LSIMAP)      \
      Logger::Instance()->LogIMAPConversation(iSession,sIP, sMsg);      \

   class Logger
   {
   public:
      Logger();
      ~Logger(void);

      enum LogSource
      {
         LSEnabled = 1,
         LSSMTP = 2,
         LSPOP3 = 4,
         LSTCPIP = 8,
         LSApplication = 16,
         LSDebug = 32,
         LSIMAP = 64,
         LSEvents = 128,
         LSKeepFilesOpen = 256
      };

      enum LogType
      {
         Normal = 1,
         Error = 2,
         AWStats = 3,
         Backup = 4,
         Events = 5,
         IMAP = 6,
         POP3 = 7,
         SMTP = 8
      };
   
      enum Constants
      {
         LiveLogMaxSize = 1000000
      };

         
      void SetLogMask(int iMask);

      void LogApplication(const String &sMessage);
      void LogApplication(const String &sMessage, bool isError);
      void LogSMTPConversation(int iSessionID, const String &sRemoteHost, const String &sMessage, bool bClient = false);
      void LogPOP3Conversation(int iSessionID, const String &sRemoteHost, const String &sMessage, bool bClient = false);
      void LogIMAPConversation(int iSessionID, const String &sRemoteHost, const String &sMessage);
      void LogEvent(const String &sMessage);
      void LogTCPIP(const String &sMessage);
      void LogDebug(const String &sMessage);
      void LogError(const String &sMessage);

      void LogAWStats(const String &sData);
      void LogBackup(const String &sData);

      bool GetLogPOP3() { return (GetLogMask() & Logger::LSPOP3) != 0; }
      bool GetLogIMAP() { return (GetLogMask() & Logger::LSIMAP) != 0; }
      bool GetLogSMTP() { return (GetLogMask() & Logger::LSSMTP) != 0; }
      bool GetLogDebug() const; 
      bool GetLogApplication() const; 
      bool GetLogTCPIP() const;
      bool GetLoggingEnabled() const;
	  bool GetLiveLogEnabled() const;

      static void CreateInstance();
      static void DeleteInstance();
      static Logger* Instance();

      void EnableLiveLogging(bool bEnable);   
      String GetLiveLog();

      int GetLogMask() 
      {
         return m_iLogMask;
      }

      String GetCurrentLogFileName(LogType lt) ;

   private:

      File* _GetCurrentLogFile(LogType lt);

      void _LogLive(String &sMessage);
      bool _WriteData(const String &sData, LogType = Normal);
   
      static Logger *pInstanceApp;   
     
      String m_sLogDir;
      String GetCurrentTime();

      int _GetProcessID();
      int _GetThreadID();


      bool m_bEnableLiveLog;
      bool m_bSepSvcLogs;      
      int  m_iLogLevel;      
      int  m_iMaxLogLineLen;      

      CriticalSection m_oCritSec;
      CriticalSection m_oCritSecLiveLog;

      String m_sLiveLog;
      int m_iLogMask;

      File _normalLogFile;
      File _errorLogFile;
      File _awstatsLogFile;
      File _backupLogFile;
      File _eventsLogFile;
      File _IMAPLogFile;
      File _POP3LogFile;
      File _SMTPLogFile;
      
   };

}

