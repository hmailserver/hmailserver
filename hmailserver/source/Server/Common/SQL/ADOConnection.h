// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "DALConnection.h"

namespace HM
{
   class ADOConnection : public DALConnection, public boost::enable_shared_from_this<ADOConnection>
   {
   public:
      enum ServerInfo
      {
         RequiredVersion = 8
      };

	   ADOConnection(shared_ptr<DatabaseSettings> pSettings);
	   virtual ~ADOConnection();

      ConnectionResult Connect(String &sErrorMessage);
      bool Disconnect();
      ExecutionResult TryExecute(const SQLCommand &command, String &sErrorMessage, __int64 *iInsertID = 0, int iIgnoreErrors = 0);
      bool IsConnected() const;
   
      virtual bool GetSupportsCommandParameters() const {return true; }
      _ConnectionPtr GetConnection() const;

      virtual void OnConnected();

      virtual bool BeginTransaction(String &sErrorMessage);
      virtual bool CommitTransaction(String &sErrorMessage);
      virtual bool RollbackTransaction(String &sErrorMessage);
      virtual void SetTimeout(int seconds);

      static DALConnection::ExecutionResult GetErrorType(int iErrorCode);
      virtual bool CheckServerVersion(String &errorMessage);
      virtual shared_ptr<DALRecordset> CreateRecordset();
      virtual void EscapeString(String &sInput);
      virtual shared_ptr<IMacroExpander> CreateMacroExpander();

      void InitializeCommandParameters(_CommandPtr &adoCommand, const SQLCommand &sqlCommand, String &queryString) const;

   private:

      __int64 _GetIdentityFromRS(_RecordsetPtr pRS) const;

      _ConnectionPtr cADOConnection;  
      bool bConnected;
      
   };
}
