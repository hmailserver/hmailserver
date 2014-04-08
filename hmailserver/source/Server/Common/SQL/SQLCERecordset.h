#pragma once

#include "DALRecordset.h"

namespace HM
{
   class SQLCERecordset : public DALRecordset
   {
   public:
	   SQLCERecordset();
	   virtual ~SQLCERecordset();

      virtual DALConnection::ExecutionResult TryOpen(shared_ptr<DALConnection> pDALConn, const SQLCommand &command, String &sErrorMessage);
      
      virtual bool MoveNext();
      virtual bool IsEOF() const;
   
      virtual long RecordCount() const;

      virtual String GetStringValue(const AnsiString &FieldName) const;
      virtual long GetLongValue(const AnsiString &FieldName) const;
      virtual __int64 GetInt64Value(const AnsiString &FieldName) const;
      virtual double GetDoubleValue(const AnsiString &FieldName) const;

      virtual vector<AnsiString> GetColumnNames() const;

      virtual bool GetIsNull(const AnsiString &FieldName) const;
   private:

      bool _Close();
      

      _RecordsetPtr cSQLCERecordset;

      long m_iCurRow;

   };

}
