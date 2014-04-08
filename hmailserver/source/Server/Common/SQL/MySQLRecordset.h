// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "DALRecordset.h"
#include "MySQLInterface.h"

namespace HM
{

   class ColumnPositions;

   class MySQLRecordset  : public DALRecordset
   {
   public:
	   MySQLRecordset();
	   virtual ~MySQLRecordset();

      virtual DALConnection::ExecutionResult TryOpen(shared_ptr<DALConnection> pDALConn, const SQLCommand &command, String &sErrorMessage);
      
      virtual bool MoveNext();
      virtual bool IsEOF() const;
      virtual long RecordCount() const; 

      virtual long GetLongValue(const AnsiString &FieldName) const;
      virtual String GetStringValue(const AnsiString &FieldName) const;
      virtual __int64 GetInt64Value(const AnsiString &FieldName) const;
      virtual double GetDoubleValue(const AnsiString &FieldName) const;

      vector<AnsiString> GetColumnNames() const;

      virtual bool GetIsNull(const AnsiString &FieldName) const;

   private:

      int _GetColumnIndex(const AnsiString &sColumnName) const;

      

      void _Close();

      hm_MYSQL_RES *m_pResult;
      hm_MYSQL_ROW m_rowCurrent;

      vector<AnsiString> m_vecColumns;

      shared_ptr<ColumnPositions> _columnPositions;
   };
}
