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

   class Logger : public Singleton<Logger>
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

      void EnableLiveLogging(bool bEnable);   
      String GetLiveLog();

      int GetLogMask() 
      {
         return log_mask_;
      }

      String GetCurrentLogFileName(LogType lt) ;

   private:

      String CleanLogMessage_(const String &message);
      File* GetCurrentLogFile_(LogType lt);

      void LogLive_(String &sMessage);
      void WriteData_(const String &sData, LogType = Normal);
   
      String log_dir_;
      String GetCurrentTime();

      int GetProcessID_();
      int GetThreadID_();


      bool enable_live_log_;
      bool sep_svc_logs_;      
      int  log_level_;      
      int  max_log_line_len_;      

      String live_log_;
      int log_mask_;

      File normal_log_file_;
      File error_log_file_;
      File awstats_log_file_;
      File backup_log_file_;
      File events_log_file_;
      File imaplog_file_;
      File pop3log_file_;
      File smtplog_file_;

      boost::recursive_mutex mtx_;
      boost::recursive_mutex mtx_LiveLog;
   };

}

