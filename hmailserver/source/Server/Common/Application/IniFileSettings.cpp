// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IniFileSettings.h"

#include "../Util/Crypt.h"
#include "../Util/Utilities.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   String IniFileSettings::ini_file_;
  
   IniFileSettings::IniFileSettings() :
      is_internal_database_(false),
      dbport_(0),
      no_of_dbconnections_(0),
      add_xauth_user_header_(false),
      no_of_dbconnection_attempts_(6),
      no_of_dbconnection_attempts_Delay(5),
      max_no_of_external_fetch_threads_(15),
      greylisting_enabled_during_record_expiration_(true),
      greylisting_expiration_interval_(240),
      preferred_hash_algorithm_(3),
      dnsbl_checks_after_mail_from_(false),
      log_level_(0),
      max_log_line_len_(500),
      quick_retries_(0),
      quick_retries_Minutes(0),
      queue_randomness_minutes_(0),
      mxtries_factor_(0),
      sqldbtype_(HM::DatabaseSettings::TypeUnknown),
      sep_svc_logs_(false),
	  rewrite_envelope_from_when_forwarding_(false),
      archive_hardlinks_(false),
      pop3dmin_timeout_(0),
      pop3dmax_timeout_(0),
      pop3cmin_timeout_(0),
      pop3cmax_timeout_(0),
      smtpdmin_timeout_(0),
      smtpdmax_timeout_(0),
      smtpcmin_timeout_(0),
      smtpcmax_timeout_(0),
      samin_timeout_(0),
      samax_timeout_(0),
      clam_min_timeout_(0),
      clam_max_timeout_(0),
      samove_vs_copy_(false),
      indexer_full_minutes_(0),
      indexer_full_limit_(0),
      indexer_quick_limit_(0),
      load_header_read_size_(0),
      load_body_read_size_(0),
      blocked_iphold_seconds_(0),
      smtpdmax_size_drop_(0),
      backup_messages_dbonly_(false),
      add_xauth_user_ip_(false)
      
   {

   }

   IniFileSettings::~IniFileSettings()
   {  
   

   }

   void
   IniFileSettings::LoadSettings()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Load all settings from hMailServer.ini
   //---------------------------------------------------------------------------()
   {
      administrator_password_ = ReadIniSettingString_("Security", "AdministratorPassword", "");

      database_server_ = ReadIniSettingString_("Database", "Server", "");
      database_name_ = ReadIniSettingString_("Database", "Database", "");
      username_ = ReadIniSettingString_("Database", "Username", "");
      password_ = ReadIniSettingString_("Database", "Password", "");
      is_internal_database_ = ReadIniSettingInteger_("Database", "Internal", 0) == 1;
      database_server_FailoverPartner = ReadIniSettingString_("Database", "ServerFailoverPartner", "");
      database_provider_ = ReadIniSettingString_("Database", "Provider", "");

      String sDatabaseType = ReadIniSettingString_("Database", "Type", "");
      
      Crypt::EncryptionType iPWDEncryptionType = (Crypt::EncryptionType) ReadIniSettingInteger_("Database", "Passwordencryption", 0);

      // Decrypt password read from hmailserver.ini
      password_ = Crypt::Instance()->DeCrypt(password_, iPWDEncryptionType);

      if (sDatabaseType.CompareNoCase(_T("MSSQL")) == 0)
         sqldbtype_ = HM::DatabaseSettings::TypeMSSQLServer;
      else if (sDatabaseType.CompareNoCase(_T("MYSQL")) == 0)
         sqldbtype_ = HM::DatabaseSettings::TypeMYSQLServer;
      else if (sDatabaseType.CompareNoCase(_T("PostgreSQL")) == 0)
         sqldbtype_ = HM::DatabaseSettings::TypePGServer;
      else if (sDatabaseType.CompareNoCase(_T("MSSQLCE")) == 0)
         sqldbtype_ = HM::DatabaseSettings::TypeMSSQLCompactEdition;

      dbport_ = ReadIniSettingInteger_( "Database", "Port", 0);

      app_directory_ = ReadIniSettingString_("Directories", "ProgramFolder", "");
      if (app_directory_.Right(1) != _T("\\"))
         app_directory_ += "\\";

      data_directory_ = ReadIniSettingString_("Directories", "DataFolder", "");
      if (data_directory_.Right(1) == _T("\\"))
         data_directory_ = data_directory_.Left(data_directory_.GetLength() -1);

      temp_directory_ = ReadIniSettingString_("Directories", "TempFolder", "");
      if (temp_directory_.Right(1) == _T("\\"))
         temp_directory_ = temp_directory_.Left(temp_directory_.GetLength() -1);

      event_directory_ = ReadIniSettingString_("Directories", "EventFolder", "");

      dbscript_directory_ = ReadIniSettingString_("Directories", "ProgramFolder", "");
      if (dbscript_directory_.Right(1) != _T("\\"))
         dbscript_directory_ += "\\";
      dbscript_directory_ += "DBScripts";

      no_of_dbconnections_ = ReadIniSettingInteger_("Database", "NumberOfConnections", 5);            
      no_of_dbconnection_attempts_ = ReadIniSettingInteger_("Database", "ConnectionAttempts", 6);  
      no_of_dbconnection_attempts_Delay = ReadIniSettingInteger_("Database", "ConnectionAttemptsDelay", 5);  
      
      if (sqldbtype_ == HM::DatabaseSettings::TypeMSSQLCompactEdition)
      {
         // Always use one database connection when working with SQL CE. SQL CE is supposed
         // to be ACID, robust and so on but isn't really.
         // http://forums.microsoft.com/MSDN/ShowPost.aspx?PostID=4141097&SiteID=1
         no_of_dbconnections_ = 1;
      }

      max_no_of_external_fetch_threads_ = ReadIniSettingInteger_("Settings", "MaxNumberOfExternalFetchThreads", 15);
      add_xauth_user_header_ = ReadIniSettingInteger_("Settings", "AddXAuthUserHeader", 0) == 1;

      daemonaddress_domain_ = ReadIniSettingString_("Settings", "DaemonAddressDomain", "");
      
      greylisting_enabled_during_record_expiration_ = ReadIniSettingInteger_("Settings", "GreylistingEnabledDuringRecordExpiration", 1) == 1;
      greylisting_expiration_interval_ = ReadIniSettingInteger_("Settings", "GreylistingRecordExpirationInterval", 240);

      database_directory_ = ReadIniSettingString_("Directories", "DatabaseFolder", "");
      if (database_directory_.Right(1) == _T("\\"))
         database_directory_ = database_directory_.Left(database_directory_.GetLength() -1);

      String sValidLanguages = ReadIniSettingString_("GUILanguages", "ValidLanguages", "");
      valid_languages_ = StringParser::SplitString(sValidLanguages, ",");

      preferred_hash_algorithm_ = ReadIniSettingInteger_("Settings", "PreferredHashAlgorithm", 3);

      dnsbl_checks_after_mail_from_ = ReadIniSettingInteger_("Settings", "DNSBLChecksAfterMailFrom", 1) == 1;

      sep_svc_logs_ = ReadIniSettingInteger_("Settings", "SepSvcLogs", 0) == 1;
      log_level_ = ReadIniSettingInteger_("Settings", "LogLevel", 9);
      max_log_line_len_ = ReadIniSettingInteger_("Settings", "MaxLogLineLen", 500);
      if (max_log_line_len_ < 100) max_log_line_len_ = 100;
      quick_retries_ = ReadIniSettingInteger_("Settings", "QuickRetries", 0);
      quick_retries_Minutes = ReadIniSettingInteger_("Settings", "QuickRetriesMinutes", 6);
      queue_randomness_minutes_ = ReadIniSettingInteger_("Settings", "QueueRandomnessMinutes", 0);
      // If queue_randomness_minutes_ out of range use 0 
      if (queue_randomness_minutes_ <= 0) queue_randomness_minutes_ = 0;
      mxtries_factor_ = ReadIniSettingInteger_("Settings", "MXTriesFactor", 0);
      if (mxtries_factor_ <= 0) mxtries_factor_ = 0;
      archive_dir_ = ReadIniSettingString_("Settings", "ArchiveDir", "");
      if (archive_dir_.Right(1) == _T("\\"))
         archive_dir_ = archive_dir_.Left(archive_dir_.GetLength() -1);
      archive_hardlinks_ =  ReadIniSettingInteger_("Settings", "ArchiveHardLinks", 0) == 1;
      pop3dmin_timeout_ =  ReadIniSettingInteger_("Settings", "POP3DMinTimeout", 10);
      pop3dmax_timeout_ =  ReadIniSettingInteger_("Settings", "POP3DMaxTimeout",600);
      pop3cmin_timeout_ =  ReadIniSettingInteger_("Settings", "POP3CMinTimeout", 30);
      pop3cmax_timeout_ =  ReadIniSettingInteger_("Settings", "POP3CMaxTimeout",900);
      smtpdmin_timeout_ =  ReadIniSettingInteger_("Settings", "SMTPDMinTimeout", 10);
      smtpdmax_timeout_ =  ReadIniSettingInteger_("Settings", "SMTPDMaxTimeout",1800);
      smtpcmin_timeout_ =  ReadIniSettingInteger_("Settings", "SMTPCMinTimeout", 30);
      smtpcmax_timeout_ =  ReadIniSettingInteger_("Settings", "SMTPCMaxTimeout",600);
      samin_timeout_ =  ReadIniSettingInteger_("Settings", "SAMinTimeout", 30);
      samax_timeout_ =  ReadIniSettingInteger_("Settings", "SAMaxTimeout",90);
      clam_min_timeout_ =  ReadIniSettingInteger_("Settings", "ClamMinTimeout", 15);
      clam_max_timeout_ =  ReadIniSettingInteger_("Settings", "ClamMaxTimeout",90);
      samove_vs_copy_ = ReadIniSettingInteger_("Settings", "SAMoveVsCopy", 0) == 1;
      auth_user_replacement_ip_ = ReadIniSettingString_("Settings", "AuthUserReplacementIP", "");
      indexer_full_minutes_ =  ReadIniSettingInteger_("Settings", "IndexerFullMinutes",720);
      indexer_full_limit_ =  ReadIniSettingInteger_("Settings", "IndexerFullLimit",25000);
      indexer_quick_limit_ =  ReadIniSettingInteger_("Settings", "IndexerQuickLimit",1000);
      load_header_read_size_ =  ReadIniSettingInteger_("Settings", "LoadHeaderReadSize",4000);
      load_body_read_size_ =  ReadIniSettingInteger_("Settings", "LoadBodyReadSize",4000);
      blocked_iphold_seconds_ =  ReadIniSettingInteger_("Settings", "BlockedIPHoldSeconds",0);
      smtpdmax_size_drop_ =  ReadIniSettingInteger_("Settings", "SMTPDMaxSizeDrop",0);
      backup_messages_dbonly_ =  ReadIniSettingInteger_("Settings", "BackupMessagesDBOnly",0) == 1;
      add_xauth_user_ip_ =  ReadIniSettingInteger_("Settings", "AddXAuthUserIP",1) == 1;

      rewrite_envelope_from_when_forwarding_ = ReadIniSettingInteger_("Settings", "RewriteEnvelopeFromWhenForwarding", 0) == 1;
      m_sDisableAUTHList = ReadIniSettingString_("Settings", "DisableAUTHList", "");
   }

   bool 
   IniFileSettings::GetDatabaseSettingsExists()
   {
      if (sqldbtype_ == HM::DatabaseSettings::TypeUnknown)
         return false;

      return true;
   }


   void
   IniFileSettings::WriteIniSetting_(const String &sSection, const String &sKey, const String &sValue)
   {
      WritePrivateProfileString(sSection, sKey, sValue, GetInitializationFile() );
   }

   void
   IniFileSettings::WriteIniSetting_(const String &sSection, const String &sKey, int Value)
   {
      String sValue = StringParser::IntToString(Value);
      WritePrivateProfileString(sSection, sKey, sValue, GetInitializationFile() );
   }

   String 
   IniFileSettings::ReadIniSettingString_(const String &sSection, const String &sKey, const String &sDefault)
   {
      TCHAR Value[255];
      GetPrivateProfileString( sSection, sKey, sDefault, Value, 255, GetInitializationFile() );
      return Value;
   }

   int 
   IniFileSettings::ReadIniSettingInteger_(const String &sSection, const String &sKey, int iDefault)
   {
      int iValue = GetPrivateProfileInt( sSection, sKey, iDefault, GetInitializationFile() );
      return iValue;
   }

   String
   IniFileSettings::GetInitializationFile() 
   {
      if (ini_file_.IsEmpty())
      {
         String AppPath = Utilities::GetBinDirectory();

         ini_file_ = AppPath;

         if (ini_file_.Right(1) != _T("\\"))
            ini_file_ += "\\";

         ini_file_ += "hMailServer.ini";
      }

      return ini_file_;
   }
   
   String
   IniFileSettings::GetLogDirectory() 
   { 
      if (log_directory_.IsEmpty())
      {
         TCHAR Value[255];
         GetPrivateProfileString( _T("Directories"), _T("LogFolder"), _T(""), Value, 255, GetInitializationFile() );
         log_directory_ = Value;
      }

      return log_directory_; 
   }

   String 
   IniFileSettings::GetLanguageDirectory() const
   {
      return app_directory_ + "Languages";
   }

   bool
   IniFileSettings::CheckSettings(String &sErrorMessage)
   {
      String sIniFile = GetInitializationFile();

      String sLog;
      sLog.Format(_T("Configuration::CheckSettings - %s"), sIniFile.c_str());
      LOG_DEBUG(sLog);

      switch (GetDatabaseType())
      {
      case HM::DatabaseSettings::TypeMSSQLCompactEdition:
         {
            if (database_name_.IsEmpty())
            {
               sErrorMessage.Format(_T("The setting Database in the section Database could not be read from %s"), sIniFile.c_str());
               return false;
            }
            break;
         }
      default:
         {
            if (database_server_.IsEmpty())
            {
               sErrorMessage.Format(_T("The setting Server in the section Database could not be read from %s"), sIniFile.c_str());
               return false;
            }
            break;
         }
      }


      return true;
   }

   String 
   IniFileSettings::GetUserInterfaceLanguage()
   {
      TCHAR Value[255];

      GetPrivateProfileString( _T("Settings"), _T("UseLanguage"), _T("English"), Value, 255, GetInitializationFile() );
      return Value;

   }

   void 
   IniFileSettings::SetUserInterfaceLanguage(String sLanguage)
   {
      WritePrivateProfileString(_T("Settings"), _T("UseLanguage"), sLanguage, GetInitializationFile());
   }

   int 
   IniFileSettings::GetNumberOfDatabaseConnections() const
   {
      return no_of_dbconnections_;
   }

   int 
   IniFileSettings::GetNumberOfDatabaseConnectionAttempts() const
   {
      return no_of_dbconnection_attempts_;
   }

   int 
   IniFileSettings::GetDBConnectionAttemptsDelay() const
   {
      return no_of_dbconnection_attempts_Delay;
   }


   String 
   IniFileSettings::GetAdministratorPassword()
   {
      return administrator_password_;
   }

   void 
   IniFileSettings::SetAdministratorPassword(const String &sNewPassword)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Updates the main hMailServer administration password found in hMailServer.ini
   //---------------------------------------------------------------------------()
   {
      administrator_password_ = HM::Crypt::Instance()->EnCrypt(sNewPassword, HM::Crypt::ETSHA256);

      WriteIniSetting_("Security", "AdministratorPassword", administrator_password_);
   }

   void 
   IniFileSettings::SetProgramDirectory(const String &sNewVal)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Updates the main hMailServer administration password found in hMailServer.ini
   //---------------------------------------------------------------------------()
   {
      app_directory_ = sNewVal;
      WriteIniSetting_("Directories", "ProgramFolder", app_directory_);
   }

   void 
   IniFileSettings::SetDataDirectory(const String &sNewVal)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Updates a directory in hMailServer.ini.
   //---------------------------------------------------------------------------()
   {
      data_directory_ = sNewVal;
      WriteIniSetting_("Directories", "DataFolder", data_directory_);
   }

   void 
   IniFileSettings::SetTempDirectory(const String &sNewVal)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Updates a directory in hMailServer.ini.
   //---------------------------------------------------------------------------()
   {
      temp_directory_ = sNewVal;
      WriteIniSetting_("Directories", "TempFolder", temp_directory_);
   }

   void 
   IniFileSettings::SetEventDirectory(const String &sNewVal)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Updates a directory in hMailServer.ini.
   //---------------------------------------------------------------------------()
   {
      event_directory_ = sNewVal;
      WriteIniSetting_("Directories", "EventFolder", event_directory_);
   }

   void 
   IniFileSettings::SetDatabaseDirectory(const String &sNewVal)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Updates a directory in hMailServer.ini.
   //---------------------------------------------------------------------------()
   {
      database_directory_ = sNewVal;
      WriteIniSetting_("Directories", "DatabaseFolder", database_directory_);
   }

   void 
   IniFileSettings::SetLogDirectory(const String &sNewVal)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Updates a directory in hMailServer.ini.
   //---------------------------------------------------------------------------()
   {
      log_directory_ = sNewVal;
      WriteIniSetting_("Directories", "LogFolder", log_directory_);
   }

   void 
   IniFileSettings::SetDatabaseServer(const String &sNewVal)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Updates a directory in hMailServer.ini.
   //---------------------------------------------------------------------------()
   {
      database_server_ = sNewVal;
      WriteIniSetting_("Database", "Server", database_server_);
   }

   void 
   IniFileSettings::SetDatabaseName(const String &sNewVal)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Updates a directory in hMailServer.ini.
   //---------------------------------------------------------------------------()
   {
      database_name_ = sNewVal;
      WriteIniSetting_("Database", "Database", database_name_);
   }

   void 
   IniFileSettings::SetUsername(const String &sNewVal)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Updates a directory in hMailServer.ini.
   //---------------------------------------------------------------------------()
   {
      username_ = sNewVal;
      WriteIniSetting_("Database", "Username", username_);
   }

   void 
   IniFileSettings::SetPassword(const String &sNewVal)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Updates a directory in hMailServer.ini.
   //---------------------------------------------------------------------------()
   {
      password_ = sNewVal;

      WriteIniSetting_("Database", "Password", Crypt::Instance()->EnCrypt(password_, Crypt::ETBlowFish));
      WriteIniSetting_("Database", "PasswordEncryption", Crypt::ETBlowFish);
   }

   void 
   IniFileSettings::SetDatabaseType(HM::DatabaseSettings::SQLDBType type)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Updates a directory in hMailServer.ini.
   //---------------------------------------------------------------------------()
   {
      String sDatabaseType;
      switch (type)
      {
      case HM::DatabaseSettings::TypeMSSQLServer:
          sDatabaseType = _T("MSSQL");
          break;
      case HM::DatabaseSettings::TypeMYSQLServer:
         sDatabaseType = _T("MYSQL");
         break;
      case HM::DatabaseSettings::TypePGServer:
         sDatabaseType = _T("PostgreSQL");
         break;
      case HM::DatabaseSettings::TypeMSSQLCompactEdition:
         sDatabaseType = _T("MSSQLCE");
         break;
      default:
         return;
      }

      LOG_DEBUG("Setting database type to " + sDatabaseType);
      sqldbtype_ = type;

      WriteIniSetting_("Database", "Type", sDatabaseType);
   }

   void 
   IniFileSettings::SetDatabasePort(long lNewValue)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Updates a directory in hMailServer.ini.
   //---------------------------------------------------------------------------()
   {
      dbport_ = lNewValue;
      WriteIniSetting_("Database", "Port", dbport_);
   }

   void 
   IniFileSettings::SetIsInternalDatabase(bool newValue)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Updates a directory in hMailServer.ini.
   //---------------------------------------------------------------------------()
   {
      is_internal_database_ = newValue;

      WriteIniSetting_("Database", "Internal", is_internal_database_ ? 1 : 0);
   }

   String 
   IniFileSettings::GetBinDirectory()
   {
      return FileUtilities::Combine(app_directory_, "Bin");
   }

   std::set<int> 
   IniFileSettings::GetAuthDisabledOnPorts()
   {
      if (m_sDisableAUTHList.IsEmpty())
      {
         std::set<int> empty;
         return empty;
      }

      std::vector<String> authDisabledOnPortsStr = StringParser::SplitString(m_sDisableAUTHList, ",");

      std::set<int> authDisabledOnPorts;

      for (AnsiString port : authDisabledOnPortsStr)
      {
         authDisabledOnPorts.insert(atoi(port));
      }

      return authDisabledOnPorts;
   }
}
