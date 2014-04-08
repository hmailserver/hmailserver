// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "PGConnection.h"
#include "PGRecordset.h"
#include "DatabaseSettings.h"
#include "..\Util\Unicode.h"
#include "Macros/PGSQLMacroExpander.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   PGConnection::PGConnection(shared_ptr<DatabaseSettings> pSettings) :
      DALConnection(pSettings)
   {
      m_bIsConnected = false;
   }

   PGConnection::~PGConnection()
   {
      if (m_pDBConn)
      {
         PQfinish(m_pDBConn);
         m_pDBConn = 0;
      }
        
   }

   DALConnection::ConnectionResult
   PGConnection::Connect(String &sErrorMessage)
   {
      try
      {

         String sUsername = m_pDatabaseSettings->GetUsername();
         String sPassword = m_pDatabaseSettings->GetPassword();
         String sServer = m_pDatabaseSettings->GetServer();
         String sDatabase = m_pDatabaseSettings->GetDatabaseName();
         long lDBPort = m_pDatabaseSettings->GetPort();
        
         String sConnectionString;
         sConnectionString.Format(_T("host=%s port=%d user=%s password=%s"), sServer, lDBPort, sUsername, sPassword);

         if (sDatabase.IsEmpty())
            sConnectionString += " dbname=postgres";
         else
            sConnectionString += " dbname=" + sDatabase;

         m_pDBConn = PQconnectdb(Unicode::ToANSI(sConnectionString));

        
         if (PQstatus(m_pDBConn) != CONNECTION_OK)
         {
            sErrorMessage = PQerrorMessage(m_pDBConn);
            return TemporaryFailure;
         }

         m_bIsConnected = true;
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5085, "PGConnection::Connect", "An unhandled error occurred when connecting to the database");

         return TemporaryFailure;
      }
          
      return Connected;
   }

   bool
   PGConnection::Disconnect()
   {
      if (m_pDBConn)
      {
         PQfinish(m_pDBConn);
         m_pDBConn = 0;
      }

      return true;
   }

   DALConnection::ExecutionResult
   PGConnection::TryExecute(const SQLCommand &command, String &sErrorMessage, __int64 *iInsertID, int iIgnoreErrors) 
   {
      String SQL = command.GetQueryString();

      try
      {
         // PG_query-doc:
         // Zero if the query was successful. Non-zero if an error occurred.
         // 
         AnsiString sQuery;
         if (!Unicode::WideToMultiByte(SQL, sQuery))
         {
            ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5106, "PGConnection::TryExecute", "Could not convert string into multi-byte.");
            return DALConnection::DALUnknown;
         }

         PGresult *pResult = PQexec(m_pDBConn, sQuery);

         bool bIgnoreErrors = SQL.Find(_T("[IGNORE-ERRORS]")) >= 0;

         if (!bIgnoreErrors)
         {
            DALConnection::ExecutionResult result = CheckError(pResult, SQL, sErrorMessage);
            if (result != DALSuccess)
            {
               if (pResult != 0)
                  PQclear(pResult);

               return result;
            }
         }

         ExecStatusType iExecResult = PQresultStatus(pResult);
         
         // Check if a value has been returned. Will only occur if we've
         // inserted a value.
         if (iInsertID > 0 && iExecResult == PGRES_TUPLES_OK)
         {
            // pick the ID from the first row.
            char *pRetVal = PQgetvalue(pResult, 0, 0);
            *iInsertID = pRetVal ? _atoi64(pRetVal) : 0;
         }

         if (pResult != 0)
            PQclear(pResult);
        
      }
      catch (...)
      {
         sErrorMessage = "Source: PGConnection::TryExecute, Code: HM5084, Description: An unhanded error occurred while executing: " + SQL;
         return DALConnection::DALUnknown;
      }

      return DALConnection::DALSuccess;
   }

   bool
   PGConnection::IsConnected() const
   {
      return m_bIsConnected;
   }

   PGconn*
   PGConnection::GetConnection() const
   {
      return m_pDBConn;
   }

   DALConnection::ExecutionResult
   PGConnection::CheckError(PGresult *pResult, const String &sAdditionalInfo, String &sOutputErrorMessage) const
   {
      try
      {
         String sErrorMsg = "";

         DALConnection::ExecutionResult result = DALConnection::DALUnknown;
         
         if (pResult)
         {  
            ExecStatusType iExecResult = PQresultStatus(pResult);

            if (iExecResult == PGRES_COMMAND_OK || iExecResult == PGRES_TUPLES_OK) 
            {
               result = DALConnection::DALSuccess;
               return result;
            }
            else if (iExecResult == PGRES_FATAL_ERROR)
            {
               result = DALConnection::DALErrorInSQL;
            }

            // Retrieve error message
            sErrorMsg  = PQresultErrorMessage(pResult);
         }
         else
         {
            sErrorMsg  = PQerrorMessage(m_pDBConn);

            if (sErrorMsg.IsEmpty())
               sErrorMsg = "Unknown error. Error structure not initialized.";

            result = DALConnection::DALConnectionProblem;
         }

         String sErrorMessage;
         sErrorMessage.Format(_T("Postgres: %s (Additional info: %s)"), sErrorMsg, sAdditionalInfo);

         sOutputErrorMessage = sErrorMessage;

         return result;
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(HM::ErrorManager::High, 5083, "PGConnection::CheckError", "An unhandled error occurred while checking for errors.");

         return DALConnection::DALUnknown;
      }
   }

   void 
   PGConnection::OnConnected()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Not much going on here...
   //---------------------------------------------------------------------------()
   {
      
   }

   bool 
   PGConnection::BeginTransaction(String &sErrorMessage)
   {
      return TryExecute(SQLCommand("BEGIN TRANSACTION"), sErrorMessage, 0, 0) == DALSuccess;
   }

   bool 
   PGConnection::CommitTransaction(String &sErrorMessage)
   {
      return TryExecute(SQLCommand("COMMIT TRANSACTION"), sErrorMessage, 0, 0) == DALSuccess;
   }

   bool 
   PGConnection::RollbackTransaction(String &sErrorMessage)
   {
      return TryExecute(SQLCommand("ROLLBACK TRANSACTION"), sErrorMessage, 0, 0) == DALSuccess;
   }

   bool 
   PGConnection::CheckServerVersion(String &errorMessage)
   {

      return true;
   }

   shared_ptr<DALRecordset> 
   PGConnection::CreateRecordset()
   {
      shared_ptr<PGRecordset> recordset = shared_ptr<PGRecordset>(new PGRecordset());
      return recordset;
   }

   void
   PGConnection::EscapeString(String &sInput)
   {
      sInput.Replace(_T("'"), _T("''"));
      sInput.Replace(_T("\\"), _T("\\\\"));
   }

   shared_ptr<IMacroExpander> 
   PGConnection::CreateMacroExpander()
   {
      shared_ptr<PGSQLMacroExpander> expander = shared_ptr<PGSQLMacroExpander>(new PGSQLMacroExpander());
      return expander;
   }

}