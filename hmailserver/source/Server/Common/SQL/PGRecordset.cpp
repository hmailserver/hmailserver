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
      row_count_(0),
      result_(0),
      cur_row_num_(0)
   {
       
   }

   PGRecordset::~PGRecordset()
   {
      try
      {
         Close_();
      }
      catch (...)
      {

      }
   }

   DALConnection::ExecutionResult
   PGRecordset::TryOpen(std::shared_ptr<DALConnection> pDALConn, const SQLCommand &command, String &sErrorMessage)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Opens a recordset specified by the given SQL string sSQL.
   //---------------------------------------------------------------------------()
   {  
      std::shared_ptr<PGConnection> pConn = std::static_pointer_cast<PGConnection>(pDALConn);
      
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
        
         result_ = PQexec(pPG, sQuery);

         DALConnection::ExecutionResult result = pConn->CheckError(result_, sSQL, sErrorMessage);
         if (result != DALConnection::DALSuccess)
            return result;

         ExecStatusType iExecResult = PQresultStatus(result_);

         if (iExecResult == PGRES_COMMAND_OK)
            row_count_ = 0;
         else
            row_count_ = PQntuples(result_);

         // We're at the first row.
         cur_row_num_ = 0;
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(HM::ErrorManager::High, 5085, "PGConnection::TryOpen", "An unknown error occurred while executing" + sSQL);
         throw;
      }
      
      return DALConnection::DALSuccess;
   }

   bool
   PGRecordset::Close_()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Closes a recordset.
   //---------------------------------------------------------------------------()
   {
      try
      {
         if (result_)
         {
            PQclear(result_);
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
      return row_count_;
   }

   bool
   PGRecordset::IsEOF() const
   {
      if (!result_)
         return true;

      try
      {
         if ((long) cur_row_num_ >= RecordCount())
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
      cur_row_num_++;
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
         ReportEOFError_(FieldName);
         return "";
      }

      try
      {
         int iColIdx = GetColumnIndex_(FieldName);
         char *pValue = PQgetvalue(result_, cur_row_num_, iColIdx);

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
         ReportEOFError_(FieldName);
         return 0;
      }

      try
      {
         int iColIdx = GetColumnIndex_(FieldName);
         char *pValue = PQgetvalue(result_, cur_row_num_, iColIdx);
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
         ReportEOFError_(FieldName);
         return 0;
      }

      try
      {
         int iColIdx = GetColumnIndex_(FieldName);
         char *pValue = PQgetvalue(result_, cur_row_num_, iColIdx);
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
         ReportEOFError_(FieldName);
         return 0;
      }

      try
      {
         int iColIdx = GetColumnIndex_(FieldName);
         char *pValue = PQgetvalue(result_, cur_row_num_, iColIdx);
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
   PGRecordset::GetColumnIndex_(const AnsiString &sColumnName) const
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Returns the index of a column in the recordset, based on the columns name.
   //---------------------------------------------------------------------------()
   {
      if (!result_)
      {
         // Result set wasn't initialized. Shouldn't happen.
         ErrorManager::Instance()->ReportError(HM::ErrorManager::High, 5093, "PGConnection::Close", "An unknown error occurred while closing recordset.");
         return 0;
      }

      unsigned int iFieldCount = PQnfields(result_);

      for (unsigned int i = 0; i <= iFieldCount; i++)
      {
         AnsiString sColName = PQfname(result_, i);

         if (sColName == sColumnName)
            return i;

      }      

      // Result set wasn't initialized. Shouldn't happen.
      ErrorManager::Instance()->ReportError(ErrorManager::High, 5092, "MySQLRecordset::GetColumnIndex_", "The requested column was not found. Column name: " + sColumnName);

      return -1;
   }

   std::vector<AnsiString> 
   PGRecordset::GetColumnNames() const
   {
      std::vector<AnsiString> result;

      unsigned int iFieldCount = PQnfields(result_);

      for (unsigned int i = 0; i <= iFieldCount; i++)
      {
         AnsiString sColName = PQfname(result_, i);

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
         ReportEOFError_(FieldName);
         return false;
      }

      try
      {
         int iColIdx = GetColumnIndex_(FieldName);
         bool isNull = PQgetisnull(result_, cur_row_num_, iColIdx) == 1;

         return isNull;
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5301, "PGRecordset::GetIsNull", "An error occured while checking for null.");
         throw;
      }
   }

}
