// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "DALConnection.h"
#include "MySQLInterface.h"
#include "ColumnPositionCache.h"

namespace HM
{
   

   class MySQLConnection : public DALConnection, public std::enable_shared_from_this<MySQLConnection>
   {
   public:

      enum Server
      {
         RequiredVersion = 40118
      };

	   MySQLConnection(std::shared_ptr<DatabaseSettings> pSettings);
	   virtual ~MySQLConnection();

      virtual ConnectionResult Connect(String &sErrorMessage);
      virtual bool Disconnect();
      virtual ExecutionResult TryExecute(const SQLCommand &command, String &sErrorMessage, __int64 *iInsertID = 0, int iIgnoreErrors = 0); 
      virtual bool IsConnected() const;

      hm_MYSQL *GetConnection() const;

      ExecutionResult CheckError(hm_MYSQL *pSQL, const String &sAdditionalInfo, String &sOutputErrorMessage) const;

      virtual void OnConnected();

      virtual bool BeginTransaction(String &sErrorMessage);
      virtual bool CommitTransaction(String &sErrorMessage);
      virtual bool RollbackTransaction(String &sErrorMessage);
      virtual void SetTimeout(int seconds) {}

      virtual bool GetSupportsCommandParameters() const {return false; }
      ColumnPositionCache& GetColumnPositionCache() {return column_position_cache_;}

      virtual bool CheckServerVersion(String &errorMessage);

      virtual std::shared_ptr<DALRecordset> CreateRecordset();

      virtual void EscapeString(String &sInput);

      virtual std::shared_ptr<IMacroExpander> CreateMacroExpander();

   private:

      DALConnection::ExecutionResult GetErrorType_(hm_MYSQL *pSQL);

      void UpdatePassword_();
      void RunScriptFile_(const String &sFile) ;
      void RunCommand_(const String &sCommand) ;
      void LoadSupportsTransactions_(const String &database);      
      void SetConnectionCharacterSet_();      

      hm_MYSQL *dbconn_;

      bool is_connected_;
      bool supports_transactions_;

      ColumnPositionCache column_position_cache_;

   };

}
