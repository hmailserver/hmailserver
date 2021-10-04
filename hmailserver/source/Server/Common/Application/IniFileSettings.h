// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../SQL/DatabaseSettings.h"

namespace HM
{
   class IniFileSettings : public Singleton<IniFileSettings>
   {
   public:

	   IniFileSettings();
	   virtual ~IniFileSettings();

      void LoadSettings();

      bool CheckSettings(String &sErrorMessage);

      static String GetInitializationFile();

      bool GetDatabaseSettingsExists();

      String GetDatabaseProvider() const { return database_provider_; }
      String GetDatabaseServer() const { return database_server_; }
      String GetDatabaseName() const { return database_name_; }
      String GetUsername() const { return username_; }
      String GetPassword() const { return password_; }
      DatabaseSettings::SQLDBType GetDatabaseType() const { return sqldbtype_; }
      long GetDatabasePort() const { return dbport_;}
      bool GetIsInternalDatabase() const {return is_internal_database_; }

      void SetDatabaseServer(const String &sNewValue);
      void SetDatabaseName(const String &sNewValue);
      void SetUsername(const String &sNewValue);
      void SetPassword(const String &sNewValue);
      void SetDatabaseType(DatabaseSettings::SQLDBType type);
      void SetDatabasePort(long lNewValue);
      void SetIsInternalDatabase(bool newValue);

      void SetAdministratorPassword(const String &sNewPassword);
      String GetAdministratorPassword();
      String GetLogDirectory();

      String GetBinDirectory();
      String GetProgramDirectory() const { return app_directory_; }
      String GetDataDirectory() const { return data_directory_; }
      String GetTempDirectory() const { return temp_directory_; }
      String GetEventDirectory() const { return event_directory_; }
      String GetDBScriptDirectory() const { return dbscript_directory_; }
      String GetDatabaseDirectory() const { return database_directory_; }
      String GetLanguageDirectory() const;
      String GetDatabaseServerFailoverPartner() const { return database_server_FailoverPartner; }

      void SetProgramDirectory(const String &sNewVal);
      void SetDataDirectory(const String &sNewVal);
      void SetTempDirectory(const String &sNewVal);
      void SetEventDirectory(const String &sNewVal);
      void SetDatabaseDirectory(const String &sNewVal);
      void SetLogDirectory(const String &sLogDirectory);

      
      std::vector<String> GetValidLanguages() const {return valid_languages_; }

      String GetUserInterfaceLanguage();
      void SetUserInterfaceLanguage(String sLanguage);

      int GetNumberOfDatabaseConnections() const;
      int GetNumberOfDatabaseConnectionAttempts() const;
      int GetDBConnectionAttemptsDelay() const;
      
      bool GetAddXAuthUserHeader() {return add_xauth_user_header_; }
      String GetDaemonAddressDomain() const { return daemonaddress_domain_; }
      int GetMaxNumberOfExternalFetchThreads() {return max_no_of_external_fetch_threads_ ;}
      bool GetGreylistingEnabledDuringRecordExpiration() {return greylisting_enabled_during_record_expiration_;}
      int GetGreylistingExpirationInterval() {return greylisting_expiration_interval_; }
      int GetPreferredHashAlgorithm() {return preferred_hash_algorithm_;}
      bool GetDNSBLChecksAfterMailFrom() {return dnsbl_checks_after_mail_from_; }
      bool GetSepSvcLogs() {return sep_svc_logs_; }
      int GetLogLevel() {return log_level_; }
      int GetMaxLogLineLen() {return max_log_line_len_; }
      int GetQuickRetries() {return quick_retries_; }
      int GetQuickRetriesMinutes() {return quick_retries_Minutes; }
      int GetQueueRandomnessMinutes () {return queue_randomness_minutes_; }
      int GetMXTriesFactor () {return mxtries_factor_; }
      String GetArchiveDir() const { return archive_dir_; }
      bool GetArchiveHardlinks() const { return archive_hardlinks_; }
      int GetPOP3DMinTimeout () {return pop3dmin_timeout_; }
      int GetPOP3DMaxTimeout () {return pop3dmax_timeout_; }
      int GetPOP3CMinTimeout () {return pop3cmin_timeout_; }
      int GetPOP3CMaxTimeout () {return pop3cmax_timeout_; }
      int GetSMTPDMinTimeout () {return smtpdmin_timeout_; }
      int GetSMTPDMaxTimeout () {return smtpdmax_timeout_; }
      int GetSMTPCMinTimeout () {return smtpcmin_timeout_; }
      int GetSMTPCMaxTimeout () {return smtpcmax_timeout_; }
      int GetSAMinTimeout () {return samin_timeout_; }
      int GetSAMaxTimeout () {return samax_timeout_; }
      int GetClamMinTimeout () {return clam_min_timeout_; }
      int GetClamMaxTimeout () {return clam_max_timeout_; }
      bool GetSAMoveVsCopy() const { return samove_vs_copy_; }
      String GetAuthUserReplacementIP() const { return auth_user_replacement_ip_; }
      int GetIndexerFullMinutes () {return indexer_full_minutes_; }
      int GetIndexerFullLimit () {return indexer_full_limit_; }
      int GetIndexerQuickLimit () {return indexer_quick_limit_; }
      int GetLoadHeaderReadSize () {return load_header_read_size_; }
      int GetLoadBodyReadSize () {return load_body_read_size_; }
      int GetBlockedIPHoldSeconds () {return blocked_iphold_seconds_; }
      int GetSMTPDMaxSizeDrop () {return smtpdmax_size_drop_; }
      bool GetBackupMessagesDBOnly () const { return backup_messages_dbonly_; }
      bool GetAddXAuthUserIP () const { return add_xauth_user_ip_; }
      bool GetRewriteEnvelopeFromWhenForwarding() const { return rewrite_envelope_from_when_forwarding_; }
      bool GetUseDNSCache() const { return use_dns_cache_; }
      String GetDNSServer() const { return dns_server_; }
      std::set<int> GetAuthDisabledOnPorts();

   private:   

      void WriteIniSetting_(const String &sSection, const String &sKey, const String &sValue);
      void WriteIniSetting_(const String &sSection, const String &sKey, int Value);
      String ReadIniSettingString_(const String &sSection, const String &sKey, const String &sDefault);
      int ReadIniSettingInteger_(const String &sSection, const String &sKey, int iDefault);

      String database_server_;
      String database_name_;
      String username_;
      String password_;
      String data_directory_;
      String app_directory_;
      String logs_directory_;
      String temp_directory_;
      String event_directory_;
      String dbscript_directory_;
      String database_directory_;
      String database_server_FailoverPartner;
      String administrator_password_;

      std::vector<String> valid_languages_;

      DatabaseSettings::SQLDBType sqldbtype_;
      long dbport_;

      int no_of_dbconnections_;
      int no_of_dbconnection_attempts_;
      int no_of_dbconnection_attempts_Delay;
      bool add_xauth_user_header_;
      String daemonaddress_domain_;
      int max_no_of_external_fetch_threads_;

      bool greylisting_enabled_during_record_expiration_;
      bool is_internal_database_;
      int greylisting_expiration_interval_;
      
      int preferred_hash_algorithm_;

      String log_directory_;
      static String ini_file_;

      bool dnsbl_checks_after_mail_from_;
      bool sep_svc_logs_;
      int log_level_;
      int max_log_line_len_;
      int quick_retries_;
      int quick_retries_Minutes;
      int queue_randomness_minutes_;
      int mxtries_factor_;
      String archive_dir_;
      bool archive_hardlinks_;
      int pop3dmin_timeout_;
      int pop3dmax_timeout_;
      int pop3cmin_timeout_;
      int pop3cmax_timeout_;
      int smtpdmin_timeout_;
      int smtpdmax_timeout_;
      int smtpcmin_timeout_;
      int smtpcmax_timeout_;
      int samin_timeout_;
      int samax_timeout_;
      int clam_min_timeout_;
      int clam_max_timeout_;
      bool samove_vs_copy_;
      String auth_user_replacement_ip_;
      int indexer_full_minutes_;
      int indexer_full_limit_;
      int indexer_quick_limit_;
      int load_header_read_size_;
      int load_body_read_size_;
      int blocked_iphold_seconds_;
      int smtpdmax_size_drop_;
      bool backup_messages_dbonly_;
      bool add_xauth_user_ip_;
      bool rewrite_envelope_from_when_forwarding_;
      bool use_dns_cache_;
      String dns_server_;
      String database_provider_;

      String m_sDisableAUTHList;
   };
}
