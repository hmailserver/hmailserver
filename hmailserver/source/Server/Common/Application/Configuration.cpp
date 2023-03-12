// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com


#include "stdafx.h"
#include "Configuration.h"

#include "../BO/BlockedAttachments.h"
#include "../BO/SecurityRanges.h"
#include "../BO/ServerMessages.h"
#include "../BO/SSLCertificates.h"
#include "../Application/Property.h"
#include "../Cache/CacheContainer.h"
#include "../Application/ObjectCache.h"
#include "..\Scripting\ScriptServer.h"
#include "..\Util\TLD.h"
#include "..\TCPIP\LocalIPAddresses.h"

#include "../../POP3/POP3Configuration.h"
#include "../../SMTP/SMTPConfiguration.h"

#include "../../IMAP/IMAPConfiguration.h"
#include "../Cache/CacheConfiguration.h"
#include "../Util/AWStats.h"

// BACKUP
#include "..\BO\Rules.h"
#include "..\BO\TCPIPPorts.h"
#include "..\BO\SSLCertificates.h"

#include "..\Persistence\PersistentServerMessage.h"
#include "..\Persistence\PersistentLogonFailure.h"

#include "../Application/MessageIndexer.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{

   Configuration::Configuration() :
      crash_simulation_mode_(0),
      ipv6_available_(false)
   {
   }

   Configuration::~Configuration()
   {  
   
   }

   bool 
   Configuration::Load()
   {
      IPAddress address;
      ipv6_available_ = address.TryParse("::F", false);

      property_set_ = std::shared_ptr<PropertySet>(new PropertySet());
      property_set_->Refresh();
      
      pop3_configuration_ = std::shared_ptr<POP3Configuration>(new POP3Configuration);
      smtp_configuration_ = std::shared_ptr<SMTPConfiguration>(new SMTPConfiguration);
      imap_configuration_ = std::shared_ptr<IMAPConfiguration>(new IMAPConfiguration);
      cache_configuration_ = std::shared_ptr<CacheConfiguration>(new CacheConfiguration);
      
      if (!smtp_configuration_->Load())
         return false;

      if (!anti_spam_configuration_.Load())
         return false;

      server_messages_ = std::shared_ptr<ServerMessages> (new ServerMessages);
      server_messages_->Refresh();

      blocked_attachments_ = std::shared_ptr<BlockedAttachments>(new BlockedAttachments);
      blocked_attachments_->Refresh();

      ssl_certificates_ = std::shared_ptr<SSLCertificates>(new SSLCertificates);
      ssl_certificates_->Refresh();

      ScriptServer::Instance()->LoadScripts();
      TLD::Instance()->Initialize();
      LocalIPAddresses::Instance()->LoadIPAddresses();

      return imap_configuration_->Load();
   }

   void 
   Configuration::OnPropertyChanged(std::shared_ptr<Property> pProperty)
   {
      String sPropertyName = pProperty->GetName();
      if (sPropertyName == PROPERTY_LOGGING)
      {
         // Activate the logger.
         Logger::Instance()->SetLogMask(pProperty->GetLongValue());
      }
      else if (sPropertyName == PROPERTY_AWSTATSENABLED)
      {
         AWStats::SetEnabled(GetAWStatsEnabled());
      }
      else
      {
         CacheContainer::Instance()->OnPropertyChanged(pProperty);
         Application::Instance()->OnPropertyChanged(pProperty);
      } 

      if (smtp_configuration_)
         smtp_configuration_->OnPropertyChanged(pProperty);
   }


   std::shared_ptr<PropertySet>
   Configuration::GetSettings() const
   {
      return property_set_;
   }

   std::shared_ptr<TCPIPPorts>
   Configuration::GetTCPIPPorts() const
   {
      std::shared_ptr<TCPIPPorts> tcpipPorts = std::shared_ptr<TCPIPPorts>(new TCPIPPorts);
      tcpipPorts->Refresh();
      return tcpipPorts;
   }

   int
   Configuration::GetWorkerThreadPriority() const
   {
      return (GetSettings()->GetLong(PROPERTY_WORKERTHREADPRIORITY));
   }

   void
   Configuration::SetWorkerThreadPriority(int iNewVal)
   {
      GetSettings()->SetLong(PROPERTY_WORKERTHREADPRIORITY, iNewVal);
   }

   int
   Configuration::GetTCPIPThreads() const
   {
      return (GetSettings()->GetLong(PROPERTY_TCPIPTHREADS));
   }

   void
   Configuration::SetTCPIPThreads(int iNewVal)
   {
      GetSettings()->SetLong(PROPERTY_TCPIPTHREADS, iNewVal);
   }

   bool
   Configuration::GetUseLogging() const
   {
      return (GetSettings()->GetLong(PROPERTY_LOGGING)& Logger::LSEnabled) ? true : false;
   }

   void
   Configuration::SetUseLogging(bool bLogging)
   {
      int iLogging = GetSettings()->GetLong(PROPERTY_LOGGING);

      if (bLogging)
         iLogging = iLogging | Logger::LSEnabled;
      else
         iLogging = iLogging & ~Logger::LSEnabled;

      // Save setting
      GetSettings()->SetLong(PROPERTY_LOGGING, iLogging);
   }


   bool
   Configuration::GetUseSMTP() const
   {
      return GetSettings()->GetBool(PROPERTY_PROTOCOLSMTP);
   }

   void
   Configuration::SetUseSMTP(bool UseSMTP)
   {
      GetSettings()->SetBool(PROPERTY_PROTOCOLSMTP, UseSMTP);
   }

   bool
   Configuration::GetUsePOP3() const
   {
      return GetSettings()->GetBool(PROPERTY_PROTOCOLPOP3);
   }

   void
   Configuration::SetUsePOP3(bool UsePOP3)
   {
      GetSettings()->SetBool(PROPERTY_PROTOCOLPOP3, UsePOP3);
   }


   bool
   Configuration::GetUseIMAP() const
   {
      return GetSettings()->GetBool(PROPERTY_PROTOCOLIMAP);
   }

   void
   Configuration::SetUseIMAP(bool UseIMAP)
   {
      GetSettings()->SetBool(PROPERTY_PROTOCOLIMAP, UseIMAP);
   }


   bool
   Configuration::GetAWStatsEnabled() const
   {
      return GetSettings()->GetBool(PROPERTY_AWSTATSENABLED);
   }

   void
   Configuration::SetAWStatsEnabled(bool bNewVal)
   {
      GetSettings()->SetBool(PROPERTY_AWSTATSENABLED, bNewVal);

      // Update the logger
      AWStats::SetEnabled(bNewVal);
   }

   long
   Configuration::GetLogFormat() const
   {
      return GetSettings()->GetLong(PROPERTY_LOGFORMAT);
   }

   void
   Configuration::SetLogFormat(long lNewVal)
   {
      GetSettings()->SetLong(PROPERTY_LOGFORMAT, lNewVal);
   }

   void 
   Configuration::SetMirrorAddress(const String &sAddress)
   {
      GetSettings()->SetString(PROPERTY_MIRROREMAILADDRESS, sAddress);
   }

   String
   Configuration::GetMirrorAddress() const
   {
      return GetSettings()->GetString(PROPERTY_MIRROREMAILADDRESS);
   }

   bool
   Configuration::GetLogSMTPConversations() const
   {
      return (GetSettings()->GetLong(PROPERTY_LOGGING)& Logger::LSSMTP) ? true : false;
   }

   void
   Configuration::SetLogSMTPConversations(bool newVal) 
   {
      int iLogging = GetSettings()->GetLong(PROPERTY_LOGGING);

      if (newVal)
         iLogging = iLogging | Logger::LSSMTP;
      else
         iLogging = iLogging & ~Logger::LSSMTP;

      // Save setting
      GetSettings()->SetLong(PROPERTY_LOGGING, iLogging);
   }

   bool
   Configuration::GetLogPOP3Conversations() const
   {
      return (GetSettings()->GetLong(PROPERTY_LOGGING)& Logger::LSPOP3) ? true : false;
   }

   void
   Configuration::SetLogPOP3Conversations(bool newVal)
   {
      int iLogging = GetSettings()->GetLong(PROPERTY_LOGGING);

      if (newVal)
         iLogging = iLogging | Logger::LSPOP3;
      else
         iLogging = iLogging & ~Logger::LSPOP3;

      // Save setting
      GetSettings()->SetLong(PROPERTY_LOGGING, iLogging);
   }

   bool
   Configuration::GetLogIMAPConversations() const
   {
      return (GetSettings()->GetLong(PROPERTY_LOGGING)& Logger::LSIMAP) ? true : false;
   }

   void
   Configuration::SetLogIMAPConversations(bool newVal)
   {
      int iLogging = GetSettings()->GetLong(PROPERTY_LOGGING);

      if (newVal)
         iLogging = iLogging | Logger::LSIMAP;
      else
         iLogging = iLogging & ~Logger::LSIMAP;

      // Save setting
      GetSettings()->SetLong(PROPERTY_LOGGING, iLogging);
   }

   bool
   Configuration::GetLogTCPIP() const
   {
      return (GetSettings()->GetLong(PROPERTY_LOGGING)& Logger::LSTCPIP) ? true : false;
   }

   void
   Configuration::SetLogTCPIP(bool newVal)
   {
      int iLogging = GetSettings()->GetLong(PROPERTY_LOGGING);

      if (newVal)
         iLogging = iLogging | Logger::LSTCPIP;
      else
         iLogging = iLogging & ~Logger::LSTCPIP;

      // Save setting
      GetSettings()->SetLong(PROPERTY_LOGGING, iLogging);
   }

   bool
   Configuration::GetLogApplication() const
   {
      return (GetSettings()->GetLong(PROPERTY_LOGGING)& Logger::LSApplication) ? true : false;
   }

   void
   Configuration::SetLogApplication(bool newVal)
   {
      int iLogging = GetSettings()->GetLong(PROPERTY_LOGGING);

      if (newVal)
         iLogging = iLogging | Logger::LSApplication;
      else
         iLogging = iLogging &~ Logger::LSApplication;

      // Save setting
      GetSettings()->SetLong(PROPERTY_LOGGING, iLogging);
   }

   bool
   Configuration::GetLogDebug() const
   {
      return (GetSettings()->GetLong(PROPERTY_LOGGING) & Logger::LSDebug) ? true : false;
   }

   void
   Configuration::SetLogDebug(bool newVal)
   {
      int iLogging = GetSettings()->GetLong(PROPERTY_LOGGING);

      if (newVal)
         iLogging = iLogging | Logger::LSDebug;
      else
         iLogging = iLogging &~ Logger::LSDebug;

      // Save setting
      GetSettings()->SetLong(PROPERTY_LOGGING, iLogging);
   }

   bool
   Configuration::GetUseScriptServer() const
   {
      return GetSettings()->GetBool(PROPERTY_USESCRIPTSERVER);
   }

   void
   Configuration::SetUseScriptServer(bool newVal)
   {
      GetSettings()->SetBool(PROPERTY_USESCRIPTSERVER, newVal);
   }

   bool
   Configuration::GetKeepFilesOpen() const
   {
      return (GetSettings()->GetLong(PROPERTY_LOGGING)& Logger::LSKeepFilesOpen) ? true : false;
   }

   void
   Configuration::SetKeepFilesOpen(bool bLogging)
   {
      int iLogging = GetSettings()->GetLong(PROPERTY_LOGGING);

      if (bLogging)
         iLogging = iLogging | Logger::LSKeepFilesOpen;
      else
         iLogging = iLogging & ~Logger::LSKeepFilesOpen;

      GetSettings()->SetLong(PROPERTY_LOGGING, iLogging);
   }

   String
   Configuration::GetScriptLanguage() const
   {
      return GetSettings()->GetString(PROPERTY_SCRIPTLANGUAGE);
   }

   void
   Configuration::SetScriptLanguage(const String &newVal)
   {
      GetSettings()->SetString(PROPERTY_SCRIPTLANGUAGE, newVal);
   }

   String
   Configuration::GetDefaultDomain() const
   {
      return GetSettings()->GetString(PROPERTY_DEFAULTDOMAIN);
   }

   void
   Configuration::SetDefaultDomain(const String &newVal)
   {
      GetSettings()->SetString(PROPERTY_DEFAULTDOMAIN, newVal);
   }

   long
   Configuration::GetLogDevice() const
   {
      return GetSettings()->GetLong(PROPERTY_LOGDEVICE);
   }

   void
   Configuration::SetLogDevice(long newVal)
   {
      GetSettings()->SetLong(PROPERTY_LOGDEVICE, newVal);
   }

   String 
   Configuration::GetBackupDestination() const
   {
      return GetSettings()->GetString(PROPERTY_BACKUPDESTINATION);
   }
   
   void 
   Configuration::SetBackupDestination(const String &sNewPath)
   {
      GetSettings()->SetString(PROPERTY_BACKUPDESTINATION, sNewPath);
   }

   bool
   Configuration::GetBackupOption(Backup::BackupOptions iMode) const
   {
      return (GetSettings()->GetLong(PROPERTY_BACKUPOPTIONS)& iMode) ? true : false;      
   }

   int 
   Configuration::GetBackupOptions() const
   {
      return (GetSettings()->GetLong(PROPERTY_BACKUPOPTIONS));
   }

   void
   Configuration::SetBackupOption(Backup::BackupOptions iMode, bool bSet) 
   {
      int iOption = GetSettings()->GetLong(PROPERTY_BACKUPOPTIONS);

      if (bSet)
         iOption = iOption | iMode;
      else
         iOption = iOption &~ iMode;

      GetSettings()->SetLong(PROPERTY_BACKUPOPTIONS, iOption);

   }

   void 
   Configuration::SetHostName(String &NewVal)
   {
      GetSettings()->SetString(PROPERTY_HOSTNAME, NewVal);
   }

   String
   Configuration::GetHostName() const
   {
      return GetSettings()->GetString(PROPERTY_HOSTNAME);
   }

   void
   Configuration::SetDisconnectInvalidClients(bool newVal)
   {
      GetSettings()->SetBool(PROPERTY_SMTPDISCONNECTINVALIDCLIENTS, newVal);
   }

   bool 
   Configuration::GetDisconnectInvalidClients() 
   {
      return GetSettings()->GetBool(PROPERTY_SMTPDISCONNECTINVALIDCLIENTS);
   }

   void
   Configuration::SetMaximumIncorrectCommands(int newVal)
   {
      GetSettings()->SetLong(PROPERTY_SMTPMAXINCORRECTCOMMANDS, newVal);
   }

   int
   Configuration::GetMaximumIncorrectCommands() 
   {
      return GetSettings()->GetLong(PROPERTY_SMTPMAXINCORRECTCOMMANDS);
   }


   void 
   Configuration::SetAutoBanLogonEnabled(bool newVal)
   {
      GetSettings()->SetBool(PROPERTY_AUTOBAN_ENABLED, newVal);
   }

   bool
   Configuration::GetAutoBanLogonEnabled() 
   {
      return GetSettings()->GetBool(PROPERTY_AUTOBAN_ENABLED);
   }


   void Configuration::SetMaxInvalidLogonAttempts(int newVal)
   {
      GetSettings()->SetLong(PROPERTY_MAX_INVALID_LOGON_ATTEMPTS, newVal);
   }

   int
   Configuration::GetMaxInvalidLogonAttempts() 
   {
      return GetSettings()->GetLong(PROPERTY_MAX_INVALID_LOGON_ATTEMPTS);
   }

   void
   Configuration::SetMaxLogonAttemptsWithin(int newVal)
   {
      GetSettings()->SetLong(PROPERTY_LOGON_ATTEMPTS_WITHIN, newVal);
   }

   int
   Configuration::GetMaxLogonAttemptsWithin() 
   {
      return GetSettings()->GetLong(PROPERTY_LOGON_ATTEMPTS_WITHIN);
   }

   void
   Configuration::SetAutoBanMinutes(int newVal)
   {
      GetSettings()->SetLong(PROPERTY_AUTOBAN_MINUTES, newVal);
   }

   int
   Configuration::GetAutoBanMinutes() 
   {
      return GetSettings()->GetLong(PROPERTY_AUTOBAN_MINUTES);
   }

   bool 
   Configuration::ClearOldLogonFailures()
   {
      HM::PersistentLogonFailure failure;
      return failure.ClearOldFailures(-1);
   }

   int
   Configuration::GetAsynchronousThreads() const
   {
      return (GetSettings()->GetLong(PROPERTY_MAX_NUMBER_OF_ASYNC_TASKS));
   }

   void
   Configuration::SetAsynchronousThreads(int iNewVal)
   {
      GetSettings()->SetLong(PROPERTY_MAX_NUMBER_OF_ASYNC_TASKS, iNewVal);
   }

   bool 
   Configuration::GetMessageIndexing()
   {
      return property_set_->GetBool(PROPERTY_MESSAGE_INDEXING);
   }

   void 
   Configuration::SetMessageIndexing(bool enable)
   {
      if (enable)
         MessageIndexer::Instance()->Start();
      else
         MessageIndexer::Instance()->Stop();

      property_set_->SetBool(PROPERTY_MESSAGE_INDEXING, enable);
   }

   bool
   Configuration::GetVerifyRemoteSslCertificate() const
   {
      return GetSettings()->GetBool(PROPERTY_VERIFYREMOTESSLCERTIFICATE);
   }

   void
   Configuration::SetVerifyRemoteSslCertificate(bool UseSMTP)
   {
      GetSettings()->SetBool(PROPERTY_VERIFYREMOTESSLCERTIFICATE, UseSMTP);
   }

   String
   Configuration::GetSslCipherList() const
   {
      return GetSettings()->GetString(PROPERTY_SSLCIPHERLIST);
   }

   void
   Configuration::SetSslCipherList(String newValue)
   {
      GetSettings()->SetString(PROPERTY_SSLCIPHERLIST, newValue);
   }

   bool
   Configuration::GetSslVersionEnabled(SslTlsVersion version) const
   {
      return (GetSettings()->GetLong(PROPERTY_SSLVERSIONS) & version) ? true : false;
   }

   
   void
   Configuration::SetSslVersionEnabled(SslTlsVersion version, bool enabled)
   {
      int versions = GetSettings()->GetLong(PROPERTY_SSLVERSIONS);

      if (enabled)
         versions = versions | version;
      else
         versions = versions &~version;

      GetSettings()->SetLong(PROPERTY_SSLVERSIONS, versions);
   }

   void
   Configuration::SetIPv6Preferred(bool newVal)
   {
      GetSettings()->SetBool(PROPERTY_IPV6_PREFERRED, newVal);
   }

   bool
   Configuration::GetIPv6Preferred()
   {
      return GetSettings()->GetBool(PROPERTY_IPV6_PREFERRED);
   }

   int
   Configuration::GetCrashSimulationMode() const
   {
      return crash_simulation_mode_;
   }


   void
   Configuration::SetCrashSimulationMode(int mode)
   {
      crash_simulation_mode_ = mode;
   }


   bool 
   Configuration::XMLStore(XNode *pBackupNode)
   {
      // PROPERTIES
      property_set_->XMLStore(pBackupNode);

      // SECURITY RANGES
      SecurityRanges securityRanges;
      securityRanges.Refresh();
      securityRanges.XMLStore(pBackupNode, 0);

      // RULES
      std::shared_ptr<Rules> pRules = std::shared_ptr<Rules>(new Rules(0));
      pRules->Refresh();
      pRules->XMLStore(pBackupNode, 0);

      // TCP/IP ports
      GetTCPIPPorts()->XMLStore(pBackupNode, 0);
      ssl_certificates_->XMLStore(pBackupNode, 0);
      blocked_attachments_->XMLStore(pBackupNode, 0);

      smtp_configuration_->XMLStore(pBackupNode, 0);
      imap_configuration_->XMLStore(pBackupNode, 0);
      anti_spam_configuration_.XMLStore(pBackupNode, 0);
      cache_configuration_->XMLStore(pBackupNode, 0);

      return true;
   }

   bool
   Configuration::XMLLoad(XNode *pBackupNode, int iRestoreOptions)
   {
      // PROPERTIES
      if (!property_set_->XMLLoad(pBackupNode))
         return false;

      // SECURITY RANGES
      SecurityRanges securityRanges;
      securityRanges.Refresh();
      if (!securityRanges.XMLLoad(pBackupNode, iRestoreOptions))
         return false;

      // RULES
      std::shared_ptr<Rules> pRules = std::shared_ptr<Rules>(new Rules(0));
      pRules->Refresh();
      if (!pRules->XMLLoad(pBackupNode, iRestoreOptions))
         return false;
      ObjectCache::Instance()->GetGlobalRules()->Refresh();

      // SSL certs must be restored before TCP/IP ports, since the
      // TCP/IP ports refers to the SSL certs.
      ssl_certificates_->Refresh();
      if (!ssl_certificates_->XMLLoad(pBackupNode, iRestoreOptions))
         return false;

      // TCP/IP ports
      if (!GetTCPIPPorts()->XMLLoad(pBackupNode, iRestoreOptions))
         return false;

      blocked_attachments_->Refresh();
      if (!blocked_attachments_->XMLLoad(pBackupNode, iRestoreOptions))
         return false;

      if (!smtp_configuration_->XMLLoad(pBackupNode, iRestoreOptions))
         return false;

      if (!imap_configuration_->XMLLoad(pBackupNode, iRestoreOptions))
         return false;

      if (!anti_spam_configuration_.XMLLoad(pBackupNode, iRestoreOptions))
         return false;

      if (!cache_configuration_->XMLLoad(pBackupNode, iRestoreOptions))
         return false;

      return true;
   }

   bool
   Configuration::IsIPv6Available()
   {
      return ipv6_available_;
   }
} 