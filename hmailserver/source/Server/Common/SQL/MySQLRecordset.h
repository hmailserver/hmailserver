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

      virtual DALConnection::ExecutionResult TryOpen(std::shared_ptr<DALConnection> pDALConn, const SQLCommand &command, String &sErrorMessage);
      
      virtual bool MoveNext();
      virtual bool IsEOF() const;
      virtual long RecordCount() const; 

      virtual long GetLongValue(const AnsiString &FieldName) const;
      virtual String GetStringValue(const AnsiString &FieldName) const;
      virtual __int64 GetInt64Value(const AnsiString &FieldName) const;
      virtual double GetDoubleValue(const AnsiString &FieldName) const;

      std::vector<AnsiString> GetColumnNames() const;

      virtual bool GetIsNull(const AnsiString &FieldName) const;

   private:

      int GetColumnIndex_(const AnsiString &sColumnName) const;

      

      void Close_();

      hm_MYSQL_RES *result_;
      hm_MYSQL_ROW current_;

      std::vector<AnsiString> columns_;

      std::shared_ptr<ColumnPositions> column_positions_;
   };
}
