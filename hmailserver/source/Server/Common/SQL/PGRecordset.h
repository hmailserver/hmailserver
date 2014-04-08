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

      bool _Close();

      PGresult *m_pResult;
      // PG_ROW m_rowCurrent;

      unsigned int m_iCurRowNum;
      unsigned int m_iRowCount;
   };
}
