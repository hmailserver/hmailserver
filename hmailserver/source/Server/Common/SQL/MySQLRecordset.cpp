// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "MySQLRecordset.h"
#include "MySQLConnection.h"

#include "../Util/Unicode.h"
#include "../SQL/ColumnPositionCache.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   MySQLRecordset::MySQLRecordset() :
      m_pResult(0)
   {
      
   }

   MySQLRecordset::~MySQLRecordset()
   {
      _Close();
   }

   DALConnection::ExecutionResult
   MySQLRecordset::TryOpen(shared_ptr<DALConnection> pDALConn, const SQLCommand &command, String &sErrorMessage)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Opens a recordset specified by the given SQL string sSQL.
   //---------------------------------------------------------------------------()
   {  
      shared_ptr<MySQLConnection> pConn = static_pointer_cast<MySQLConnection>(pDALConn);

      String sSQL = command.GetQueryString();

      try
      {
         hm_MYSQL *pMYSQL = pConn->GetConnection();

         AnsiString sQuery;
         if (!Unicode::WideToMultiByte(sSQL, sQuery))
         {
            ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5108, "MySQLRecordset::TryOpen", "Could not convert string into multi-byte.");
            return DALConnection::DALUnknown;
         }

         if (MySQLInterface::Instance()->p_mysql_query(pMYSQL, sQuery))
         {
            DALConnection::ExecutionResult result = pConn->CheckError(pMYSQL, sSQL, sErrorMessage);
            return result;
         }

         // Store the result of the query
         m_pResult = MySQLInterface::Instance()->p_mysql_store_result(pMYSQL);

         // Move to the first row.
         MoveNext();

         // Cache positions so we don't have to pick them up later...
         _columnPositions = pConn->GetColumnPositionCache().GetPositions(sQuery, this);
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 4202, "MySQLRecordset::TryOpen", "An unknown error occurred while executing " + sSQL);
         return DALConnection::DALErrorInSQL;
      }

      return DALConnection::DALSuccess;

   }


   void
   MySQLRecordset::_Close()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Closes a recordset.
   //---------------------------------------------------------------------------()
   {
      try
      {
         if (m_pResult)
         {
            MySQLInterface::Instance()->p_mysql_free_result(m_pResult);
            m_pResult = 0;
         }
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4203, "MySQLRecordset::Close", "An unknown error occurred while closing recordset.");
      }
   }

   long
   MySQLRecordset::RecordCount() const
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Returns the number of rows in current recordset. 
   //---------------------------------------------------------------------------()
   {
      if (!m_pResult)
         return 0;
 
      try
      {
         long lResult = (long) MySQLInterface::Instance()->p_mysql_num_rows(m_pResult);
         return lResult;
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 4204, "MySQLRecordset::RecordCount", "An unknown error occurred while determining number of rows in record set.");
         throw;
      }

   }

   bool
   MySQLRecordset::IsEOF() const
   {
      return m_rowCurrent == 0;
   }

   bool
   MySQLRecordset::MoveNext()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Moves the cursor to the next row in the recordset.
   //---------------------------------------------------------------------------()
   {
      if (!m_pResult)
      {
         return false;
      }


      try
      {
         m_rowCurrent = MySQLInterface::Instance()->p_mysql_fetch_row(m_pResult);
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 4206, "MySQLRecordset::MoveNext", "An unknown error occurred while reading the next row from an record set.");
         throw;
      }

      if (m_rowCurrent)
         return true;
      else
         return false;
   }

   int 
   MySQLRecordset::_GetColumnIndex(const AnsiString &sColumnName) const
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Returns the index of a column in the recordset, based on the columns name.
   //---------------------------------------------------------------------------()
   {
      int index = _columnPositions->GetColumnIndex(sColumnName);
      if (index >= 0)
         return index;

      ErrorManager::Instance()->ReportError(ErrorManager::High, 4231, "MySQLRecordset::_GetColumnIndex", "The requested column was not found. Column name: " + sColumnName);
      return -1;
   }

   String
   MySQLRecordset::GetStringValue(const AnsiString &FieldName) const
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
         char *pValue = m_rowCurrent[iColIdx];
         
         if (pValue == 0 || strlen(pValue) == 0)
            return "";

         String sOutput;
         if (!Unicode::MultiByteToWide(pValue, sOutput))
         {
            ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5109, "MySQLRecordset::GetStringValue", "Could not convert multi byte to wide char.");
            return "";            
         }

         return sOutput;
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 4207, "MySQLRecordset::GetStringValue", "An unknown error occurred while reading value from recordset: " + FieldName);
         throw;
      }
   }


   long
   MySQLRecordset::GetLongValue(const AnsiString &FieldName) const
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Returns the value of a long in the current row in the recordset.
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
         char *pValue = m_rowCurrent[iColIdx];
         long lVal = pValue ? atoi(pValue) : 0;
         return lVal;
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 4207, "MySQLRecordset::GetLongValue", "An unknown error occurred while reading value from recordset: " + FieldName);
         throw;
      }
   }

   bool 
   MySQLRecordset::GetIsNull(const AnsiString &FieldName) const
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
         char *pValue = m_rowCurrent[iColIdx];

         if (pValue == 0)
            return true;

         return false;
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5302, "MySQLRecordset::GetIsNull", "An error occured while checking for null.");
         throw;
      }
   }

    __int64
   MySQLRecordset::GetInt64Value(const AnsiString &FieldName) const
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
         char *pValue = m_rowCurrent[iColIdx];
         __int64 lVal = pValue ? _atoi64(pValue) : 0;
         return lVal;
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 4207, "MySQLRecordset::GetInt64Value", "An unknown error occurred while reading value from recordset: " + FieldName);
         throw;
      }
   }

   double
   MySQLRecordset::GetDoubleValue(const AnsiString &FieldName) const
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
         char *pValue = m_rowCurrent[iColIdx];
         double dbVal = pValue ? atof(pValue) : 0;

         return dbVal;
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 4207, "MySQLRecordset::GetLongValue", "An unknown error occurred while reading value from recordset: " + FieldName);
         throw;
      }
   }

   vector<AnsiString> 
   MySQLRecordset::GetColumnNames() const
   {
      vector<AnsiString> result;
      unsigned int iFieldCount = MySQLInterface::Instance()->p_mysql_num_fields(m_pResult);

      for (unsigned int i = 0; i < iFieldCount; i++)
      {
         hm_st_mysql_field *pField = MySQLInterface::Instance()->p_mysql_fetch_field_direct(m_pResult, i);

         result.push_back(pField->name);
      }  
      return result;
   }
}
