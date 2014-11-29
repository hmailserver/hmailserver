// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "MySQLConnection.h"
#include "MySQLRecordset.h"
#include "DatabaseSettings.h"
#include "Macros/MySQLMacroExpander.h"
#include "..\Util\Unicode.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   MySQLConnection::MySQLConnection(std::shared_ptr<DatabaseSettings> pSettings) :
      DALConnection(pSettings)
   {
      is_connected_ = false;
      dbconn_ = 0;
      supports_transactions_ = false;
   }

   MySQLConnection::~MySQLConnection()
   {
      try
      {
         if (dbconn_)
         {
            MySQLInterface::Instance()->p_mysql_close(dbconn_);
            dbconn_ = 0;
         }
      }
      catch (...)
      {

      }
         
   }

   DALConnection::ConnectionResult
   MySQLConnection::Connect(String &sErrorMessage)
   {
      if (!MySQLInterface::Instance()->IsLoaded())
      {
         // Load the MySQL interface.
         if (!MySQLInterface::Instance()->Load(sErrorMessage))
         {
            // Loading failed
            return FatalError;
         }
      }

      try
      {
         String sUsername = database_settings_->GetUsername();
         String sPassword = database_settings_->GetPassword();
         String sServer = database_settings_->GetServer();
         String sDatabase = database_settings_->GetDatabaseName();
         long lDBPort = database_settings_->GetPort();

         if (lDBPort == 0)
            lDBPort = 3306;
         
         

         dbconn_ = MySQLInterface::Instance()->p_mysql_init(NULL);

         //MYSQL *pResult = mysql_real_connect(
         hm_MYSQL *pResult = MySQLInterface::Instance()->p_mysql_real_connect(
                     dbconn_, 
                     Unicode::ToANSI(sServer), 
                     Unicode::ToANSI(sUsername), 
                     Unicode::ToANSI(sPassword), 
                     Unicode::ToANSI(sDatabase), lDBPort, 0, 0);

         if (pResult == 0)
         {
            // From MySQL manual:
            // 
            // Return Values:
            //
            // A MYSQL* connection handle if the connection was successful, NULL if the connection was 
            // unsuccessful. For a successful connection, the return value is the same as the value 
            // of the first parameter.

            const char *pError = MySQLInterface::Instance()->p_mysql_error(dbconn_);
            sErrorMessage = pError;

            return TemporaryFailure;
         }

         if (CheckError(pResult, "mysql_real_connect()", sErrorMessage) != DALConnection::DALSuccess)
            return TemporaryFailure;

         SetConnectionCharacterSet_();

         if (!sDatabase.IsEmpty())
         {
            String switch_db_command = "use " + sDatabase;

            if (TryExecute(SQLCommand(switch_db_command), sErrorMessage, 0, 0) != DALConnection::DALSuccess)
            {
               return TemporaryFailure;
            }
         }

         LoadSupportsTransactions_(sDatabase);

         is_connected_ = true;
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5008, "MySQLConnection::Connect", "An unhandled error occurred when connecting to the database.");
         return TemporaryFailure;
      }
          
      return Connected;
   }

   bool 
   MySQLConnection::CheckServerVersion(String &errorMessage)
   {
      // check server version.
      int serverVersion = MySQLInterface::Instance()->p_mysql_get_server_version(dbconn_);
      if (serverVersion < RequiredVersion)
      {
         errorMessage = "hMailServer requires MySQL 4.1.18 or newer. If you are using the internal MySQL database, please upgrade to the latest 4.x version prior to upgrading to version 5 or later.";
         return false;
      }

      return true;
   }

   bool
   MySQLConnection::Disconnect()
   {
      if (dbconn_)
      {
         MySQLInterface::Instance()->p_mysql_close(dbconn_);
         dbconn_ = 0;
      }

      return true;
   }

   DALConnection::ExecutionResult
   MySQLConnection::TryExecute(const SQLCommand &command, String &sErrorMessage, __int64 *iInsertID, int iIgnoreErrors) 
   {
      String SQL = command.GetQueryString();
      try
      {
         // mysql_query-doc:
         // Zero if the query was successful. Non-zero if an error occurred.
         // 
         AnsiString sQuery;
         if (!Unicode::WideToMultiByte(SQL, sQuery))
         {
            ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5105, "MySQLConnection::TryExecute", "Could not convert string into multi-byte.");
            return DALConnection::DALUnknown;
         }
   
         if (MySQLInterface::Instance()->p_mysql_query(dbconn_, sQuery))
         {
            bool bIgnoreErrors = SQL.Find(_T("[IGNORE-ERRORS]")) >= 0;
            if (!bIgnoreErrors)
            {
               if (iIgnoreErrors == 0 || !(GetErrorType_(dbconn_) & iIgnoreErrors))
               {
                  DALConnection::ExecutionResult result = CheckError(dbconn_, SQL, sErrorMessage);
                  return result;
               }
            }
         }

         hm_MYSQL_RES *pRes = MySQLInterface::Instance()->p_mysql_store_result(dbconn_); // should always be called after mysql_query

         if (pRes)
            MySQLInterface::Instance()->p_mysql_free_result(pRes);

         // Fetch insert id.
         if (iInsertID > 0)
         {
            *iInsertID = MySQLInterface::Instance()->p_mysql_insert_id(dbconn_);
         }
      }
      catch (...)
      {
         sErrorMessage = "Source: MySQLConnection::TryExecute, Code: HM10048, Description: An unhandled error occurred while executing: " + SQL;
         return DALConnection::DALUnknown;
      }

      return DALConnection::DALSuccess;
   }

   bool
   MySQLConnection::IsConnected() const
   {
      return is_connected_;
   }

   hm_MYSQL*
   MySQLConnection::GetConnection() const
   {
      return dbconn_;
   }

   DALConnection::ExecutionResult
   MySQLConnection::GetErrorType_(hm_MYSQL *pSQL)
   {
      try
      {
         if (pSQL==NULL) 
            return DALSuccess;

         int iErrNo = MySQLInterface::Instance()->p_mysql_errno(pSQL);

         switch (iErrNo)
         {
         case 0:
            return DALSuccess;
         case 1062: // ER_DUP_ENTRY - Message: Duplicate entry '%s' for key %d
            return DALErrorInSQL;
         default:
            return DALUnknown;
         }

         assert(0); // Should never get here
         return DALSuccess;
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 4373, "MySQLConnection::_GetErrorNumber", "An error occurred while trying to retrieve error code from MySQL.");
         return DALErrorInSQL;
      }

      return DALSuccess;

   }

   DALConnection::ExecutionResult
   MySQLConnection::CheckError(hm_MYSQL *pSQL, const String &sAdditionalInfo, String &sOutputErrorMessage) const
   {
      try
      {
         if (pSQL==NULL) 
            return DALConnection::DALSuccess;

         const char *pError = MySQLInterface::Instance()->p_mysql_error(pSQL);
         if (!pError[0] != '\0')
            return DALConnection::DALSuccess;

         
         DALConnection::ExecutionResult result = DALConnection::DALUnknown;

         int errorCode = MySQLInterface::Instance()->p_mysql_errno(pSQL);
         switch (errorCode)
         {
         case 2006: // MySQL server has gone away 
         case 2013: // Lost connection to MySQL server during query 
            result = DALConnection::DALConnectionProblem;
            break;
         }
         

         AnsiString sMySqlErrorAnsi = pError;
         String sMySQLErrorUnicode = sMySqlErrorAnsi;

         String sErrorMessage;
         sErrorMessage.Format(_T("MySQL: %s (Additional info: %s)"), sMySQLErrorUnicode.c_str(), sAdditionalInfo.c_str());

         sOutputErrorMessage = sErrorMessage;

         return result;
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5009, "MySQLConnection::CheckError", "An unhandled error occurred while checking for errors.");
         return DALConnection::DALUnknown;
      }
   }

   void 
   MySQLConnection::OnConnected()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // This would need refactoring some day. This is the place 
   // where the internal MySQL database structure is managed.
   // The update of the data tables is taken care of by the
   // installation program, but the mysql.* tables are updated
   // here.
   //---------------------------------------------------------------------------()
   {
      // Check if the user is using the internal database. We don't rely
      // entirely on the [Database]->Internal setting in hMailServer.ini so
      // we check a few other properties as well.
      if (IniFileSettings::Instance()->GetDatabasePort() != 3307 || 
          IniFileSettings::Instance()->GetUsername().CompareNoCase(_T("root")) != 0 &&
          IniFileSettings::Instance()->GetUsername().CompareNoCase(_T("hmailserver")) != 0 &&
          IniFileSettings::Instance()->GetIsInternalDatabase())
      {
         // The user is not using the internal database.
         return;
      }

      // Remove dummy user created after installation.
      UpdatePassword_();
         
      // Run the scripts file
      String sScriptsFile = IniFileSettings::Instance()->GetDBScriptDirectory() + "\\Internal MySQL\\HMS4.3-MySQL4.1.18.sql";
      RunScriptFile_(sScriptsFile);

      RunCommand_("FLUSH PRIVILEGES");
   }

   void 
   MySQLConnection::UpdatePassword_()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Remoevs any user that lacks user name. Used to tighten security on the internal
   // database.
   //---------------------------------------------------------------------------()
   {
      // Remove the dummy user.
      RunCommand_("DELETE FROM mysql.user WHERE User = ''");
   }

   void 
   MySQLConnection::RunScriptFile_(const String &sFile) 
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Runs a SQL script which contains commands separated with semicolons. This
   // function will always succeed, so should only be used for non-important
   // SQL epressions
   //---------------------------------------------------------------------------()
   {
#ifndef _DISABLE_MYSQL_AUTOUPGRADE
      String sContents = FileUtilities::ReadCompleteTextFile(sFile);

      std::vector<String> vecCommands = StringParser::SplitString(sContents, ";");

      auto iterCommand = vecCommands.begin();
      auto iterEnd = vecCommands.end();
      for (; iterCommand != iterEnd; iterCommand++)
      {
         String sSQL = (*iterCommand);

         sSQL.TrimLeft(_T("\r\n "));
         sSQL.TrimRight(_T("\r\n "));

         if (!sSQL.IsEmpty())
            RunCommand_(sSQL);
      }
#endif
   }

   void 
   MySQLConnection::RunCommand_(const String &sCommand) 
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Runs a single SQL command without any error handling.
   //---------------------------------------------------------------------------()
   {
      String sError;
      
      TryExecute(SQLCommand(sCommand), sError, 0);
   }

   bool 
   MySQLConnection::BeginTransaction(String &sErrorMessage)
   {
      if (supports_transactions_)
      {
         return TryExecute(SQLCommand("BEGIN"), sErrorMessage, 0)  == DALSuccess;
      }

      return true;
   }

   bool 
   MySQLConnection::CommitTransaction(String &sErrorMessage)
   {
      if (supports_transactions_)
      {
         return TryExecute(SQLCommand("COMMIT"), sErrorMessage, 0)  == DALSuccess;
      }
      

      return true;
   }

   bool 
   MySQLConnection::RollbackTransaction(String &sErrorMessage)
   {
      if (supports_transactions_)
      {
         return TryExecute(SQLCommand("ROLLBACK"), sErrorMessage, 0)  == DALSuccess;
      }
      else
      {
         sErrorMessage = "Rollback of MySQL statements failed. You may need to restore the latest database backup to ensure database integrity";
         ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5104, "MySQLConnection::RollbackTransaction", sErrorMessage);

         return false;
      }
   }

   void 
   MySQLConnection::LoadSupportsTransactions_(const String &database)
   {
      supports_transactions_ = false;

      if (database.GetLength() == 0)
         return;

      MySQLRecordset rec;
      if (!rec.Open(shared_from_this(), SQLCommand("SHOW TABLE STATUS in " + database)))
         return;

      int tableCount = 0;

      while (!rec.IsEOF())
      {
         String sEngine = rec.GetStringValue("Engine");
         if (sEngine.CompareNoCase(_T("InnoDB")) != 0)
         {
            return;
         }

         tableCount++;

         rec.MoveNext();
      }

      if (tableCount > 0)
      {
         // Only InnoDB tables in this database. Enable transactions.
         supports_transactions_ = true;
      }
   }

   void 
   MySQLConnection::SetConnectionCharacterSet_()
   {
      std::set<String> utf_character_sets;

      MySQLRecordset rec;
      if (!rec.Open(shared_from_this(), SQLCommand("SHOW CHARACTER SET LIKE 'UTF%'")))
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5008, "MySQLConnection::LoadConnectionCharacterSet_", "Unable to find appropriate MySQL character set. Command SHOW CHARACTER SET LIKE 'UTF%' failed.");
         return;
      }


      while (!rec.IsEOF())
      {
         String character_set  = rec.GetStringValue("Charset");
         utf_character_sets.insert(character_set);
         rec.MoveNext();
      }

      String character_set_to_use;

      if (utf_character_sets.find("utf8mb4") != utf_character_sets.end())
         character_set_to_use = "utf8mb4";
      else if (utf_character_sets.find("utf8") != utf_character_sets.end())
         character_set_to_use = "utf8";
      else
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5008, "MySQLConnection::LoadConnectionCharacterSet_", "Unable to find appropriate MySQL character set.");
         return;
      }

      String error_message;
      AnsiString set_names_command = Formatter::Format("SET NAMES {0}", character_set_to_use);

      if (TryExecute(SQLCommand(set_names_command), error_message, 0, 0) != DALConnection::DALSuccess)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5008, "MySQLConnection::LoadConnectionCharacterSet_", set_names_command);
      }
   }

   std::shared_ptr<DALRecordset> 
   MySQLConnection::CreateRecordset()
   {
      std::shared_ptr<MySQLRecordset> recordset = std::shared_ptr<MySQLRecordset>(new MySQLRecordset());
      return recordset;
   }

   void
   MySQLConnection::EscapeString(String &sInput)
   {
      sInput.Replace(_T("'"), _T("''"));
      sInput.Replace(_T("\\"), _T("\\\\"));
   }

   std::shared_ptr<IMacroExpander> 
   MySQLConnection::CreateMacroExpander()
   {
      std::shared_ptr<MySQLMacroExpander> expander = std::shared_ptr<MySQLMacroExpander>(new MySQLMacroExpander());
      return expander;
   }
}