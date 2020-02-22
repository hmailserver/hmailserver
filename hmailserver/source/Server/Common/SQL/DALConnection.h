// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class DatabaseSettings;
   class MacroExpander;
   class IMacroExpander;

   class DALConnection  
   {
   public:
	   DALConnection(std::shared_ptr<DatabaseSettings> pDatabaseSettings);
	   virtual ~DALConnection();

      enum ConnectionResult
      {
         Connected = 1,
         TemporaryFailure = 2,
         FatalError = 3
      };

      enum ExecutionResult
      {
         DALSuccess = 0,
         DALErrorInSQL = 1,
         DALConnectionProblem = 2,
         DALUnknown = 3
      };

      bool Execute(const SQLCommand &command, String &sErrorMessage, __int64 *iInsertID = 0, int iIgnoreErrors = 0);

      bool Reconnect(String &sErrorMessage);

      virtual ConnectionResult Connect(String &sErrorMessage) = 0;
      virtual bool Disconnect() = 0;
      virtual ExecutionResult TryExecute(const SQLCommand &command, String &sErrorMessage, __int64 *iInsertID = 0, int iIgnoreErrors = 0) = 0;
      virtual bool IsConnected() const = 0;
      virtual void OnConnected() = 0;

      virtual bool BeginTransaction(String &sErrorMessage) = 0;
      virtual bool CommitTransaction(String &sErrorMessage) = 0;
      virtual bool RollbackTransaction(String &sErrorMessage) = 0;
      virtual void SetTimeout(int seconds) = 0;

      void SetTryCount(int iTryCount) {try_count_ = iTryCount; }
      

      virtual bool GetSupportsCommandParameters() const = 0;

	   virtual bool CheckServerVersion(String &errorMessage) = 0;

      virtual void EscapeString(String &sInput) = 0;

      virtual std::shared_ptr<DALRecordset> CreateRecordset() = 0;
      virtual std::shared_ptr<IMacroExpander> CreateMacroExpander() = 0;

      std::shared_ptr<DatabaseSettings> GetSettings() {return database_settings_; }

   protected:

      std::shared_ptr<DatabaseSettings> database_settings_;

   private:

      int try_count_;
   };

}
