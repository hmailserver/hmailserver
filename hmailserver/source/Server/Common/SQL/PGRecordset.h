// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "DALRecordset.h"


#include <libpq-fe.h>

namespace HM
{
   class PGRecordset  : public DALRecordset
   {
   public:
	   PGRecordset();
	   virtual ~PGRecordset();

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

      bool Close_();

      PGresult *result_;
      // PG_ROW current_;

      unsigned int cur_row_num_;
      unsigned int row_count_;
   };
}
