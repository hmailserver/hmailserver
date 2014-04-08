// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "DALConnection.h"

namespace HM
{
   class SQLCommand;

   class DatabaseConnectionManager
   {
   public:
      DatabaseConnectionManager(void);
      ~DatabaseConnectionManager(void);

      bool CreateConnections(String &sErrorMessage);
      
      void Disconnect();

      bool Execute(const SQLStatement &statement, __int64 *iInsertID = 0, int iIgnoreErrors = 0, String &sErrorMessage = String(_T("")));
      bool Execute(const SQLCommand &command, __int64 *iInsertID = 0, int iIgnoreErrors = 0, String &sErrorMessage = String(_T("")));
      
      shared_ptr<DALRecordset> OpenRecordset(const SQLStatement &statement);
      shared_ptr<DALRecordset> OpenRecordset(const SQLCommand &command);

      int GetCurrentDatabaseVersion();

      bool GetIsConnected();

      shared_ptr<DALConnection> BeginTransaction(String &sErrorMessage);
      bool CommitTransaction(shared_ptr<DALConnection> pConnection, String &sErrorMessage);
      bool RollbackTransaction(shared_ptr<DALConnection> pConnection, String &sErrorMessage);
      bool ExecuteScript(const String &sFile, String &sErrorMessage);

      bool EnsuresPrerequisites(long DBVersion, String &sErrorMessage);
   private:

      DALConnection::ConnectionResult _Connect(String &sErrorMessage);

      shared_ptr<DALConnection> _GetConnection();
      void _ReleaseConnection(shared_ptr<DALConnection> pConn);
     
      CriticalSection m_oCritSec;
      
      std::set<shared_ptr<DALConnection> > m_setBusyConnections;
      std::set<shared_ptr<DALConnection> > m_setAvailableConnections;
      

   };
}