// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IniFileSettings.h"

#include "../Util/Crypt.h"
#include "../Util/Utilities.h"
#include "../Util/Registry.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   String IniFileSettings::m_sLogDirectory;
   String IniFileSettings::m_sIniFile;
  
   IniFileSettings::IniFileSettings() :
      m_bIsInternalDatabase(false),
      m_lDBPort(0),
      m_iNoOfDBConnections(0),
      m_bAddXAuthUserHeader(false),
      m_iNoOfDBConnectionAttempts(6),
      m_iNoOfDBConnectionAttemptsDelay(5),
      m_iMaxNoOfExternalFetchThreads(15),
      m_bGreylistingEnabledDuringRecordExpiration(true),
      m_iGreylistingExpirationInterval(240),
      _preferredHashAlgorithm(3),
      m_bDNSBlChecksAfterMailFrom(false),
      _useSSLVerifyPeer(false),
      m_iLogLevel(0),
      m_iMaxLogLineLen(500),
      m_iQuickRetries(0),
      m_iQuickRetriesMinutes(0),
      m_iQueueRandomnessMinutes(0),
      m_iMXTriesFactor(0),
      m_eSQLDBType(HM::DatabaseSettings::TypeUnknown)
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
      m_AdministratorPassword = _ReadIniSettingString("Security", "AdministratorPassword", "");

      m_DatabaseServer = _ReadIniSettingString("Database", "Server", "");
      m_DatabaseName = _ReadIniSettingString("Database", "Database", "");
      m_Username = _ReadIniSettingString("Database", "Username", "");
      m_Password = _ReadIniSettingString("Database", "Password", "");
      m_bIsInternalDatabase = _ReadIniSettingInteger("Database", "Internal", 0) == 1;
      m_DatabaseServerFailoverPartner = _ReadIniSettingString("Database", "ServerFailoverPartner", "");

      String sDatabaseType = _ReadIniSettingString("Database", "Type", "");
      
      Crypt::EncryptionType iPWDEncryptionType = (Crypt::EncryptionType) _ReadIniSettingInteger("Database", "Passwordencryption", 0);

      // Decrypt password read from hmailserver.ini
      m_Password = Crypt::Instance()->DeCrypt(m_Password, iPWDEncryptionType);

      if (sDatabaseType.CompareNoCase(_T("MSSQL")) == 0)
         m_eSQLDBType = HM::DatabaseSettings::TypeMSSQLServer;
      else if (sDatabaseType.CompareNoCase(_T("MYSQL")) == 0)
         m_eSQLDBType = HM::DatabaseSettings::TypeMYSQLServer;
      else if (sDatabaseType.CompareNoCase(_T("PostgreSQL")) == 0)
         m_eSQLDBType = HM::DatabaseSettings::TypePGServer;
      else if (sDatabaseType.CompareNoCase(_T("MSSQLCE")) == 0)
         m_eSQLDBType = HM::DatabaseSettings::TypeMSSQLCompactEdition;

      m_lDBPort = _ReadIniSettingInteger( "Database", "Port", 0);

      m_AppDirectory = _ReadIniSettingString("Directories", "ProgramFolder", "");
      if (m_AppDirectory.Right(1) != _T("\\"))
         m_AppDirectory += "\\";

      m_DataDirectory = _ReadIniSettingString("Directories", "DataFolder", "");
      if (m_DataDirectory.Right(1) == _T("\\"))
         m_DataDirectory = m_DataDirectory.Left(m_DataDirectory.GetLength() -1);

      m_sTempDirectory = _ReadIniSettingString("Directories", "TempFolder", "");
      if (m_sTempDirectory.Right(1) == _T("\\"))
         m_sTempDirectory = m_sTempDirectory.Left(m_sTempDirectory.GetLength() -1);

      m_sEventDirectory = _ReadIniSettingString("Directories", "EventFolder", "");

      m_sDBScriptDirectory = _ReadIniSettingString("Directories", "ProgramFolder", "");
      if (m_sDBScriptDirectory.Right(1) != _T("\\"))
         m_sDBScriptDirectory += "\\";
      m_sDBScriptDirectory += "DBScripts";

      m_iNoOfDBConnections = _ReadIniSettingInteger("Database", "NumberOfConnections", 5);            
      m_iNoOfDBConnectionAttempts = _ReadIniSettingInteger("Database", "ConnectionAttempts", 6);  
      m_iNoOfDBConnectionAttemptsDelay = _ReadIniSettingInteger("Database", "ConnectionAttemptsDelay", 5);  
      
      if (m_eSQLDBType == HM::DatabaseSettings::TypeMSSQLCompactEdition)
      {
         // Always use one database connection when working with SQL CE. SQL CE is supposed
         // to be ACID, robust and so on but isn't really.
         // http://forums.microsoft.com/MSDN/ShowPost.aspx?PostID=4141097&SiteID=1
         m_iNoOfDBConnections = 1;
      }

      m_iMaxNoOfExternalFetchThreads = _ReadIniSettingInteger("Settings", "MaxNumberOfExternalFetchThreads", 15);
      m_bAddXAuthUserHeader = _ReadIniSettingInteger("Settings", "AddXAuthUserHeader", 0) == 1;
      
      m_bGreylistingEnabledDuringRecordExpiration = _ReadIniSettingInteger("Settings", "GreylistingEnabledDuringRecordExpiration", 1) == 1;
      m_iGreylistingExpirationInterval = _ReadIniSettingInteger("Settings", "GreylistingRecordExpirationInterval", 240);

      m_sDatabaseDirectory = _ReadIniSettingString("Directories", "DatabaseFolder", "");
      if (m_sDatabaseDirectory.Right(1) == _T("\\"))
         m_sDatabaseDirectory = m_sDatabaseDirectory.Left(m_sDatabaseDirectory.GetLength() -1);

      String sValidLanguages = _ReadIniSettingString("GUILanguages", "ValidLanguages", "");
      m_vecValidLanguages = StringParser::SplitString(sValidLanguages, ",");

      _preferredHashAlgorithm = _ReadIniSettingInteger("Settings", "PreferredHashAlgorithm", 3);

      m_bDNSBlChecksAfterMailFrom = _ReadIniSettingInteger("Settings", "DNSBLChecksAfterMailFrom", 1) == 1;

      m_bSepSvcLogs = _ReadIniSettingInteger("Settings", "SepSvcLogs", 0) == 1;
      m_iLogLevel = _ReadIniSettingInteger("Settings", "LogLevel", 9);
      m_iMaxLogLineLen = _ReadIniSettingInteger("Settings", "MaxLogLineLen", 500);
      if (m_iMaxLogLineLen < 100) m_iMaxLogLineLen = 100;
      m_iQuickRetries = _ReadIniSettingInteger("Settings", "QuickRetries", 0);
      m_iQuickRetriesMinutes = _ReadIniSettingInteger("Settings", "QuickRetriesMinutes", 6);
      m_iQueueRandomnessMinutes = _ReadIniSettingInteger("Settings", "QueueRandomnessMinutes", 0);
      // If m_iQueueRandomnessMinutes out of range use 0 
      if (m_iQueueRandomnessMinutes <= 0) m_iQueueRandomnessMinutes = 0;
      m_iMXTriesFactor = _ReadIniSettingInteger("Settings", "MXTriesFactor", 0);
      if (m_iMXTriesFactor <= 0) m_iMXTriesFactor = 0;
      m_sArchiveDir = _ReadIniSettingString("Settings", "ArchiveDir", "");
      if (m_sArchiveDir.Right(1) == _T("\\"))
         m_sArchiveDir = m_sArchiveDir.Left(m_sArchiveDir.GetLength() -1);
      m_bArchiveHardlinks =  _ReadIniSettingInteger("Settings", "ArchiveHardLinks", 0) == 1;
      m_iPOP3DMinTimeout =  _ReadIniSettingInteger("Settings", "POP3DMinTimeout", 10);
      m_iPOP3DMaxTimeout =  _ReadIniSettingInteger("Settings", "POP3DMaxTimeout",600);
      m_iPOP3CMinTimeout =  _ReadIniSettingInteger("Settings", "POP3CMinTimeout", 30);
      m_iPOP3CMaxTimeout =  _ReadIniSettingInteger("Settings", "POP3CMaxTimeout",900);
      m_iSMTPDMinTimeout =  _ReadIniSettingInteger("Settings", "SMTPDMinTimeout", 10);
      m_iSMTPDMaxTimeout =  _ReadIniSettingInteger("Settings", "SMTPDMaxTimeout",1800);
      m_iSMTPCMinTimeout =  _ReadIniSettingInteger("Settings", "SMTPCMinTimeout", 30);
      m_iSMTPCMaxTimeout =  _ReadIniSettingInteger("Settings", "SMTPCMaxTimeout",600);
      m_iSAMinTimeout =  _ReadIniSettingInteger("Settings", "SAMinTimeout", 30);
      m_iSAMaxTimeout =  _ReadIniSettingInteger("Settings", "SAMaxTimeout",90);
      m_iClamMinTimeout =  _ReadIniSettingInteger("Settings", "ClamMinTimeout", 15);
      m_iClamMaxTimeout =  _ReadIniSettingInteger("Settings", "ClamMaxTimeout",90);
      m_bSAMoveVsCopy = _ReadIniSettingInteger("Settings", "SAMoveVsCopy", 0) == 1;
      m_sAuthUserReplacementIP = _ReadIniSettingString("Settings", "AuthUserReplacementIP", "");
      m_iIndexerFullMinutes =  _ReadIniSettingInteger("Settings", "IndexerFullMinutes",720);
      m_iIndexerFullLimit =  _ReadIniSettingInteger("Settings", "IndexerFullLimit",25000);
      m_iIndexerQuickLimit =  _ReadIniSettingInteger("Settings", "IndexerQuickLimit",1000);
      m_iLoadHeaderReadSize =  _ReadIniSettingInteger("Settings", "LoadHeaderReadSize",4000);
      m_iLoadBodyReadSize =  _ReadIniSettingInteger("Settings", "LoadBodyReadSize",4000);
      m_iBlockedIPHoldSeconds =  _ReadIniSettingInteger("Settings", "BlockedIPHoldSeconds",0);
      m_iSMTPDMaxSizeDrop =  _ReadIniSettingInteger("Settings", "SMTPDMaxSizeDrop",0);
      m_bBackupMessagesDBOnly =  _ReadIniSettingInteger("Settings", "BackupMessagesDBOnly",0) == 1;
      m_bAddXAuthUserIP =  _ReadIniSettingInteger("Settings", "AddXAuthUserIP",1) == 1;

      //Probably need some more sanity checks on these settings but for now we assume user has some sense

      // check if we should validate peer's.
      _useSSLVerifyPeer = FileUtilities::GetFilesInDirectory(GetCertificateAuthorityDirectory()).size() > 0;
   }

   bool 
   IniFileSettings::GetDatabaseSettingsExists()
   {
      if (m_eSQLDBType == HM::DatabaseSettings::TypeUnknown)
         return false;

      return true;
   }

   void
   IniFileSettings::_WriteIniSetting(const String &sSection, const String &sKey, const String &sValue)
   {
      WritePrivateProfileString(sSection, sKey, sValue, GetInitializationFile() );
   }

   void
   IniFileSettings::_WriteIniSetting(const String &sSection, const String &sKey, int Value)
   {
      String sValue = StringParser::IntToString(Value);
      WritePrivateProfileString(sSection, sKey, sValue, GetInitializationFile() );
   }

   String 
   IniFileSettings::_ReadIniSettingString(const String &sSection, const String &sKey, const String &sDefault)
   {
      TCHAR Value[255];
      GetPrivateProfileString( sSection, sKey, sDefault, Value, 255, GetInitializationFile() );
      return Value;
   }

   int 
   IniFileSettings::_ReadIniSettingInteger(const String &sSection, const String &sKey, int iDefault)
   {
      int iValue = GetPrivateProfileInt( sSection, sKey, iDefault, GetInitializationFile() );
      return iValue;
   }

   String
   IniFileSettings::GetInitializationFile() 
   {
      if (m_sIniFile.IsEmpty())
      {

         String AppPath = Utilities::GetExecutableDirectory();

         m_sIniFile = AppPath;

         if (m_sIniFile.Right(1) != _T("\\"))
            m_sIniFile += "\\";

         m_sIniFile += "hMailServer.ini";

#ifdef _DEBUG

         if (!FileUtilities::Exists(m_sIniFile))
         {
            // We're running in debug. Since the hMailServer.ini is not copied to 
            // the Source\hMailServer\Debug directory, we need to locate it.
            //
            // We assume that the install location specified in the registry is
            // the current one.
            //
            String installPath;

            Registry registry;
            if (registry.GetStringValue(HKEY_LOCAL_MACHINE, "SOFTWARE\\hMailServer", "InstallLocation", installPath))
            {
               m_sIniFile = 
                  FileUtilities::Combine(installPath, "Bin\\hMailServer.ini");
            }


         }

#endif
      }

      return m_sIniFile;
   }
   
   String
   IniFileSettings::GetLogDirectory() 
   { 
      if (m_sLogDirectory.IsEmpty())
      {
         TCHAR Value[255];
         GetPrivateProfileString( _T("Directories"), _T("LogFolder"), _T(""), Value, 255, GetInitializationFile() );
         m_sLogDirectory = Value;
      }

      return m_sLogDirectory; 
   }

   String 
   IniFileSettings::GetLanguageDirectory() const
   {
      return m_AppDirectory + "Languages";
   }

   String 
   IniFileSettings::GetCertificateAuthorityDirectory() const
   {
      return m_AppDirectory + "Externals\\CA";
   }


   bool
   IniFileSettings::CheckSettings(String &sErrorMessage)
   {
      String sIniFile = GetInitializationFile();

      String sLog;
      sLog.Format(_T("Configuration::CheckSettings - %s"), sIniFile);
      LOG_DEBUG(sLog);

      switch (GetDatabaseType())
      {
      case HM::DatabaseSettings::TypeMSSQLCompactEdition:
         {
            if (m_DatabaseName.IsEmpty())
            {
               sErrorMessage.Format(_T("The setting Database in the section Database could not be read from %s"), sIniFile);
               return false;
            }
            break;
         }
      default:
         {
            if (m_DatabaseServer.IsEmpty())
            {
               sErrorMessage.Format(_T("The setting Server in the section Database could not be read from %s"), sIniFile);
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
      return m_iNoOfDBConnections;
   }

   int 
   IniFileSettings::GetNumberOfDatabaseConnectionAttempts() const
   {
      return m_iNoOfDBConnectionAttempts;
   }

   int 
   IniFileSettings::GetDBConnectionAttemptsDelay() const
   {
      return m_iNoOfDBConnectionAttemptsDelay;
   }


   String 
   IniFileSettings::GetAdministratorPassword()
   {
      return m_AdministratorPassword;
   }

   void 
   IniFileSettings::SetAdministratorPassword(const String &sNewPassword)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Updates the main hMailServer administration password found in hMailServer.ini
   //---------------------------------------------------------------------------()
   {
      m_AdministratorPassword = HM::Crypt::Instance()->EnCrypt(sNewPassword, HM::Crypt::ETSHA256);

      _WriteIniSetting("Security", "AdministratorPassword", m_AdministratorPassword);
   }

   void 
   IniFileSettings::SetProgramDirectory(const String &sNewVal)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Updates the main hMailServer administration password found in hMailServer.ini
   //---------------------------------------------------------------------------()
   {
      m_AppDirectory = sNewVal;
      _WriteIniSetting("Directories", "ProgramFolder", m_AppDirectory);
   }

   void 
   IniFileSettings::SetDataDirectory(const String &sNewVal)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Updates a directory in hMailServer.ini.
   //---------------------------------------------------------------------------()
   {
      m_DataDirectory = sNewVal;
      _WriteIniSetting("Directories", "DataFolder", m_DataDirectory);
   }

   void 
   IniFileSettings::SetTempDirectory(const String &sNewVal)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Updates a directory in hMailServer.ini.
   //---------------------------------------------------------------------------()
   {
      m_sTempDirectory = sNewVal;
      _WriteIniSetting("Directories", "TempFolder", m_sTempDirectory);
   }

   void 
   IniFileSettings::SetEventDirectory(const String &sNewVal)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Updates a directory in hMailServer.ini.
   //---------------------------------------------------------------------------()
   {
      m_sEventDirectory = sNewVal;
      _WriteIniSetting("Directories", "EventFolder", m_sEventDirectory);
   }

   void 
   IniFileSettings::SetDatabaseDirectory(const String &sNewVal)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Updates a directory in hMailServer.ini.
   //---------------------------------------------------------------------------()
   {
      m_sDatabaseDirectory = sNewVal;
      _WriteIniSetting("Directories", "DatabaseFolder", m_sDatabaseDirectory);
   }

   void 
   IniFileSettings::SetLogDirectory(const String &sNewVal)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Updates a directory in hMailServer.ini.
   //---------------------------------------------------------------------------()
   {
      m_sLogDirectory = sNewVal;
      _WriteIniSetting("Directories", "LogFolder", m_sLogDirectory);
   }

   void 
   IniFileSettings::SetDatabaseServer(const String &sNewVal)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Updates a directory in hMailServer.ini.
   //---------------------------------------------------------------------------()
   {
      m_DatabaseServer = sNewVal;
      _WriteIniSetting("Database", "Server", m_DatabaseServer);
   }

   void 
   IniFileSettings::SetDatabaseName(const String &sNewVal)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Updates a directory in hMailServer.ini.
   //---------------------------------------------------------------------------()
   {
      m_DatabaseName = sNewVal;
      _WriteIniSetting("Database", "Database", m_DatabaseName);
   }

   void 
   IniFileSettings::SetUsername(const String &sNewVal)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Updates a directory in hMailServer.ini.
   //---------------------------------------------------------------------------()
   {
      m_Username = sNewVal;
      _WriteIniSetting("Database", "Username", m_Username);
   }

   void 
   IniFileSettings::SetPassword(const String &sNewVal)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Updates a directory in hMailServer.ini.
   //---------------------------------------------------------------------------()
   {
      m_Password = sNewVal;

      _WriteIniSetting("Database", "Password", Crypt::Instance()->EnCrypt(m_Password, Crypt::ETBlowFish));
      _WriteIniSetting("Database", "PasswordEncryption", Crypt::ETBlowFish);
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
      m_eSQLDBType = type;

      _WriteIniSetting("Database", "Type", sDatabaseType);
   }

   void 
   IniFileSettings::SetDatabasePort(long lNewValue)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Updates a directory in hMailServer.ini.
   //---------------------------------------------------------------------------()
   {
      m_lDBPort = lNewValue;
      _WriteIniSetting("Database", "Port", m_lDBPort);
   }

   void 
   IniFileSettings::SetIsInternalDatabase(bool newValue)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Updates a directory in hMailServer.ini.
   //---------------------------------------------------------------------------()
   {
      m_bIsInternalDatabase = newValue;

      _WriteIniSetting("Database", "Internal", m_bIsInternalDatabase ? 1 : 0);
   }

   String 
   IniFileSettings::GetBinDirectory()
   {
      return FileUtilities::Combine(m_AppDirectory, "Bin");
   }
}


