// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "PGRecordset.h"
#include "PGConnection.h"

#include "..\Util\Unicode.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   PGRecordset::PGRecordset() :
      m_iRowCount(0),
      m_pResult(0),
      m_iCurRowNum(0)
   {
       
   }

   PGRecordset::~PGRecordset()
   {
      _Close();
   }

   DALConnection::ExecutionResult
   PGRecordset::TryOpen(shared_ptr<DALConnection> pDALConn, const SQLCommand &command, String &sErrorMessage)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Opens a recordset specified by the given SQL string sSQL.
   //---------------------------------------------------------------------------()
   {  
      shared_ptr<PGConnection> pConn = static_pointer_cast<PGConnection>(pDALConn);
      
      String sSQL = command.GetQueryString();

      try
      {
         AnsiString sQuery;
         if (!Unicode::WideToMultiByte(sSQL, sQuery))
         {
            ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5107, "PGConnection::TryOpen", "Could not convert string into multi-byte.");
            return DALConnection::DALUnknown;
         }

         PGconn *pPG = pConn->GetConnection();
        
         m_pResult = PQexec(pPG, sQuery);

         DALConnection::ExecutionResult result = pConn->CheckError(m_pResult, sSQL, sErrorMessage);
         if (result != DALConnection::DALSuccess)
            return result;

         ExecStatusType iExecResult = PQresultStatus(m_pResult);

         if (iExecResult == PGRES_COMMAND_OK)
            m_iRowCount = 0;
         else
            m_iRowCount = PQntuples(m_pResult);

         // We're at the first row.
         m_iCurRowNum = 0;
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(HM::ErrorManager::High, 5085, "PGConnection::TryOpen", "An unknown error occurred while executing" + sSQL);
         throw;
      }
      
      return DALConnection::DALSuccess;
   }

   bool
   PGRecordset::_Close()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Closes a recordset.
   //---------------------------------------------------------------------------()
   {
      try
      {
         if (m_pResult)
         {
            PQclear(m_pResult);
         }
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(HM::ErrorManager::High, 5086, "PGConnection::Close", "An unknown error occurred while closing recordset.");
         throw;
      }
   
      return true;
   }

   long
   PGRecordset::RecordCount() const
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Returns the number of rows in current recordset. 
   //---------------------------------------------------------------------------()
   {
      return m_iRowCount;
   }

   bool
   PGRecordset::IsEOF() const
   {
      if (!m_pResult)
         return true;

      try
      {
         if ((long) m_iCurRowNum >= RecordCount())
            return true;
         else 
            return false;
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5087, "PGRecordset::IsEOF", "An unknown error occurred while checking EOF in record set.");
         throw;
      }
   }

   bool
   PGRecordset::MoveNext()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Moves the cursor to the next row in the recordset.
   //---------------------------------------------------------------------------()
   {
      m_iCurRowNum++;
      return false;
   }

   String
   PGRecordset::GetStringValue(const AnsiString &FieldName) const
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Returns the value of a string in the current row in the recordset.
   //---------------------------------------------------------------------------()
   {
      if (IsEOF())
      {
         _ReportEOFError(FieldName);
         return "";
      }

      try
      {
         int iColIdx = _GetColumnIndex(FieldName);
         char *pValue = PQgetvalue(m_pResult, m_iCurRowNum, iColIdx);

         if (pValue == 0 || strlen(pValue) == 0)
            return "";

         String sOutput;
         if (!Unicode::MultiByteToWide(pValue, sOutput))
         {
            ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5110, "PGRecordset::GetStringValue", "Could not convert multi byte to wide char.");
            return "";
         }

         return sOutput;
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5088, "PGRecordset::GetStringValue", "An unknown error occurred while reading value from recordset: " + FieldName);
         throw;
      }
   }


   long
   PGRecordset::GetLongValue(const AnsiString &FieldName) const
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Returns the value of a string in the current row in the recordset.
   //---------------------------------------------------------------------------()
   {
      if (IsEOF())
      {
         _ReportEOFError(FieldName);
         return 0;
      }

      try
      {
         int iColIdx = _GetColumnIndex(FieldName);
         char *pValue = PQgetvalue(m_pResult, m_iCurRowNum, iColIdx);
         long lVal = pValue ? atoi(pValue) : 0;
         return lVal;
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5089, "PGRecordset::GetLongValue", "An unknown error occurred while reading value from recordset: " + FieldName);
         throw;
      }
   }

   __int64
   PGRecordset::GetInt64Value(const AnsiString &FieldName) const
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Returns the value of a string in the current row in the recordset.
   //---------------------------------------------------------------------------()
   {
      if (IsEOF())
      {
         _ReportEOFError(FieldName);
         return 0;
      }

      try
      {
         int iColIdx = _GetColumnIndex(FieldName);
         char *pValue = PQgetvalue(m_pResult, m_iCurRowNum, iColIdx);
         __int64 lVal = pValue ? _atoi64(pValue) : 0;
         return lVal;
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5090, "PGRecordset::GetInt64Value", "An unknown error occurred while reading value from recordset: " + FieldName);
         throw;
      }
   }

   double
   PGRecordset::GetDoubleValue(const AnsiString &FieldName) const
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Returns the value of a string in the current row in the recordset.
   //---------------------------------------------------------------------------()
   {
      if (IsEOF())
      {
         _ReportEOFError(FieldName);
         return 0;
      }

      try
      {
         int iColIdx = _GetColumnIndex(FieldName);
         char *pValue = PQgetvalue(m_pResult, m_iCurRowNum, iColIdx);
         double dbVal = pValue ? atof(pValue) : 0;

         return dbVal;
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5091, "PGRecordset::GetDoubleValue", "An unknown error occurred while reading value from recordset: " + FieldName);
         throw;
      }
   }

   int 
   PGRecordset::_GetColumnIndex(const AnsiString &sColumnName) const
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Returns the index of a column in the recordset, based on the columns name.
   //---------------------------------------------------------------------------()
   {
      if (!m_pResult)
      {
         // Result set wasn't initialized. Shouldn't happen.
         ErrorManager::Instance()->ReportError(HM::ErrorManager::High, 5093, "PGConnection::Close", "An unknown error occurred while closing recordset.");
         return 0;
      }

      unsigned int iFieldCount = PQnfields(m_pResult);

      for (unsigned int i = 0; i <= iFieldCount; i++)
      {
         AnsiString sColName = PQfname(m_pResult, i);

         if (sColName == sColumnName)
            return i;

      }      

      // Result set wasn't initialized. Shouldn't happen.
      ErrorManager::Instance()->ReportError(ErrorManager::High, 5092, "MySQLRecordset::_GetColumnIndex", "The requested column was not found. Column name: " + sColumnName);

      return -1;
   }

   vector<AnsiString> 
   PGRecordset::GetColumnNames() const
   {
      vector<AnsiString> result;

      unsigned int iFieldCount = PQnfields(m_pResult);

      for (unsigned int i = 0; i <= iFieldCount; i++)
      {
         AnsiString sColName = PQfname(m_pResult, i);

         result.push_back(sColName);

      }      

      return result;

   }


   bool 
   PGRecordset::GetIsNull(const AnsiString &FieldName) const
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Returns true if the column contains NULL
   //---------------------------------------------------------------------------()
   {
      if (IsEOF())
      {
         _ReportEOFError(FieldName);
         return false;
      }

      try
      {
         int iColIdx = _GetColumnIndex(FieldName);
         bool isNull = PQgetisnull(m_pResult, m_iCurRowNum, iColIdx) == 1;

         return isNull;
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5301, "PGRecordset::GetIsNull", "An error occured while checking for null.");
         throw;
      }
   }

}
