// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "DALConnection.h"

namespace HM
{
   class SQLCEConnection : public DALConnection, public boost::enable_shared_from_this<SQLCEConnection>
   {
   public:
	   SQLCEConnection(shared_ptr<DatabaseSettings> pSettings);
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

      virtual shared_ptr<DALRecordset> CreateRecordset();

      virtual void EscapeString(String &sInput);

      virtual shared_ptr<IMacroExpander> CreateMacroExpander();

      void InitializeCommandParameters(_CommandPtr &adoCommand, const SQLCommand &sqlCommand, String &queryString) const;

   private:

      bool _GetRequiresUpgrade(const String &sConnectionString);


      bool _InternalConnect();
      String _GetConnectionString(const String &sDatabase, const String &sPassword) const;
      String _GetDatabaseFileName(const String &sShortName) const;

      virtual bool _UpgradeDatabase();

      DALConnection::ExecutionResult _GetErrorType(int iErrorCode);

      __int64 _GetIdentityFromRS(_RecordsetPtr pRS) const;

      _ConnectionPtr cSQLCEConnection;  
      bool m_bConnected;


   };
}
