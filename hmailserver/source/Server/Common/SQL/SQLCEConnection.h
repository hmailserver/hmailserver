// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "DALConnection.h"

namespace HM
{
   class SQLCEConnection : public DALConnection, public std::enable_shared_from_this<SQLCEConnection>
   {
   public:
	   SQLCEConnection(std::shared_ptr<DatabaseSettings> pSettings);
	   virtual ~SQLCEConnection();

      ConnectionResult Connect(String &sErrorMessage);
      bool Disconnect();
      ExecutionResult TryExecute(const SQLCommand &command, String &sErrorMessage, __int64 *iInsertID = 0, int iIgnoreErrors = 0);
      bool IsConnected() const;
  
      virtual bool GetSupportsCommandParameters() const {return true; }
      _ConnectionPtr GetConnection() const;

      virtual void OnConnected();

      static bool CreateDatabase(const String &sPath, const String &sFile, const String &sPassword,String &sErrorMessage);

      virtual bool BeginTransaction(String &sErrorMessage);
      virtual bool CommitTransaction(String &sErrorMessage);
      virtual bool RollbackTransaction(String &sErrorMessage);
      virtual void SetTimeout(int seconds);

      virtual bool CheckServerVersion(String &errorMessage);

      virtual std::shared_ptr<DALRecordset> CreateRecordset();

      virtual void EscapeString(String &sInput);

      virtual std::shared_ptr<IMacroExpander> CreateMacroExpander();

      void InitializeCommandParameters(_CommandPtr &adoCommand, const SQLCommand &sqlCommand, String &queryString) const;

   private:

      bool GetRequiresUpgrade_(const String &sConnectionString);
      String GetConnectionString_(const String &sDatabase, const String &sPassword) const;
      String GetDatabaseFileName_(const String &sShortName) const;

      virtual bool UpgradeDatabase_();

      DALConnection::ExecutionResult GetErrorType_(int iErrorCode);

      __int64 GetIdentityFromRS_(_RecordsetPtr pRS) const;

      _ConnectionPtr cSQLCEConnection;  
      bool connected_;


   };
}
