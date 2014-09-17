// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "DALRecordset.h"

namespace HM
{
   class ADORecordset : public DALRecordset
   {
   public:
	   ADORecordset();
	   virtual ~ADORecordset();

      virtual DALConnection::ExecutionResult TryOpen(std::shared_ptr<DALConnection> pConn, const SQLCommand &command, String &sErrorMessage);
      
      virtual bool MoveNext();
      virtual bool IsEOF() const;
   
      virtual long RecordCount() const;

      virtual String GetStringValue(const AnsiString &FieldName) const;
      virtual long GetLongValue(const AnsiString &FieldName) const;
      virtual __int64 GetInt64Value(const AnsiString &FieldName) const;
      virtual double GetDoubleValue(const AnsiString &FieldName) const;

      virtual std::vector<AnsiString> GetColumnNames() const;

      virtual bool GetIsNull(const AnsiString &FieldName) const;

   private:

      bool Close_();
      _RecordsetPtr cADORecordset;

      long cur_row_;
   };

}
