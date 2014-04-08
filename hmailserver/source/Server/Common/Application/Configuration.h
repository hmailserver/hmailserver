// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "Backup.h"

#include "../AntiSpam/AntiSpamConfiguration.h"
#include "../AntiVirus/AntiVirusConfiguration.h"

namespace HM
{

   class SMTPConfiguration;
   class POP3Configuration;
   class IMAPConfiguration;
   class AntiSpamConfiguration;
   class Logger;
   class PropertySet;
   class ServerMessages;
   class TCPIPPorts;
   class CacheConfiguration;
   class Settings;
   class ServerMessages;
   class TCPIPPorts;
   class BlockedAttachments;
   class SSLCertificates;

   class Configuration : public Singleton<Configuration>
   {
   public:

	   Configuration();
	   virtual ~Configuration();

      shared_ptr<POP3Configuration> GetPOP3Configuration() const { return m_pPOP3Configuration; }
      shared_ptr<SMTPConfiguration> GetSMTPConfiguration() const { return m_pSMTPConfiguration; }
      shared_ptr<IMAPConfiguration> GetIMAPConfiguration() const { return m_pIMAPConfiguration; }
      AntiSpamConfiguration &GetAntiSpamConfiguration() { return _antiSpamConfiguration; }
      AntiVirusConfiguration &GetAntiVirusConfiguration() { return _antiVirusConfiguration; }
      shared_ptr<CacheConfiguration> GetCacheConfiguration() const { return m_pCacheConfiguration; }

      bool Load();

      void OnPropertyChanged(shared_ptr<Property> pProperty);

      bool GetUseSMTP() const;
      void SetUseSMTP(bool UseSMTP);
      bool GetUsePOP3() const;
      void SetUsePOP3(bool UsePOP3);
      bool GetUseIMAP() const;
      void SetUseIMAP(bool UseIMAP);

      bool GetAWStatsEnabled() const;
      void SetAWStatsEnabled(bool newVal);

      long GetLogFormat() const;
      void SetLogFormat(long lNewVal);

      bool GetUseLogging() const;
      void SetUseLogging(bool bLogging);

      void SetLogSMTPConversations(bool newVal);
      bool GetLogSMTPConversations() const;

      void SetLogPOP3Conversations(bool newVal);
      bool GetLogPOP3Conversations() const;

      void SetLogIMAPConversations(bool newVal);
      bool GetLogIMAPConversations() const;

      void SetLogTCPIP(bool newVal);
      bool GetLogTCPIP() const;

      void SetLogApplication(bool newVal);
      bool GetLogApplication() const;

      void SetLogDebug(bool newVal);
      bool GetLogDebug() const;

      long GetLogDevice() const;
      void SetLogDevice(long newVal);

      void SetUseScriptServer(bool newVal);
      bool GetUseScriptServer() const;

      void SetKeepFilesOpen(bool newVal);
      bool GetKeepFilesOpen() const;


      int GetWorkerThreadPriority() const;
      void SetWorkerThreadPriority(int iNewVal);

      int GetTCPIPThreads() const;
      void SetTCPIPThreads(int iNewVal);

      void SetScriptLanguage(const String &newVal);
      String GetScriptLanguage() const;

      void SetDefaultDomain(const String &newVal);
      String GetDefaultDomain() const;

      String GetMirrorAddress() const;
      void SetMirrorAddress(const String &sAddress);


      // BACKUP OPTIONS
      String GetBackupDestination() const;
      void SetBackupDestination(const String &sNewPath);

      bool GetBackupOption(Backup::BackupOptions iMode) const;
      int GetBackupOptions() const;
      void SetBackupOption(Backup::BackupOptions iMode, bool bSet);

      int GetRequiredDBVersion() { return REQUIRED_DB_VERSION; }

      bool GetSendStatistics() const;
      void SetSendStatistics(bool bNewVal);

      String GetHostName() const;
      void SetHostName(String &NewVal);

      bool XMLStore(XNode *pBackupNode);
      bool XMLLoad(XNode *pBackupNode, int iRestoreOptions);

      bool GetDisconnectInvalidClients();
      void SetDisconnectInvalidClients(bool bNewVal);

      int GetMaximumIncorrectCommands();
      void SetMaximumIncorrectCommands(int iNewVal);

      bool GetAutoBanLogonEnabled();
      void SetAutoBanLogonEnabled(bool newValue);

      int GetMaxInvalidLogonAttempts();
      void SetMaxInvalidLogonAttempts(int iNewVal);

      int GetMaxLogonAttemptsWithin();
      void SetMaxLogonAttemptsWithin(int iNewVal);

      int GetAutoBanMinutes();
      void SetAutoBanMinutes(int iNewVal);

      int GetAsynchronousThreads() const;
      void SetAsynchronousThreads(int iNewVal);

      bool ClearOldLogonFailures();

      bool GetMessageIndexing();
      void SetMessageIndexing(bool enable);

      shared_ptr<PropertySet> GetSettings() const;

      shared_ptr<ServerMessages> GetServerMessages() {return _serverMessages;}
      shared_ptr<TCPIPPorts> GetTCPIPPorts() const;
      shared_ptr<BlockedAttachments> GetBlockedAttachments() {return _blockedAttachments;}
      shared_ptr<SSLCertificates> GetSSLCertificates() {return _sslCertificates;}
      shared_ptr<PropertySet> GetPropertySet() {return _propertySet;}

   private:

      shared_ptr<POP3Configuration> m_pPOP3Configuration;
      shared_ptr<SMTPConfiguration> m_pSMTPConfiguration;
      shared_ptr<IMAPConfiguration> m_pIMAPConfiguration;
      
      shared_ptr<CacheConfiguration> m_pCacheConfiguration;

      AntiSpamConfiguration _antiSpamConfiguration;
      AntiVirusConfiguration _antiVirusConfiguration;

      shared_ptr<ServerMessages> _serverMessages;
      shared_ptr<TCPIPPorts> _tcpipPorts;
      shared_ptr<BlockedAttachments> _blockedAttachments;
      shared_ptr<SSLCertificates> _sslCertificates;

      shared_ptr<PropertySet> _propertySet;
   };
}
