// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{

   class ColumnPositions
   {
   public:

      ColumnPositions();
      ColumnPositions(std::vector<AnsiString> columns);

      int GetColumnIndex(const AnsiString &columnName);

   private:

      std::vector<AnsiString> _columns;
   };

   class ColumnPositionCache
   {
   public:

      ColumnPositionCache(void);

      shared_ptr<ColumnPositions> GetPositions(const AnsiString &sqlStatement, DALRecordset *pRecordset);

   private:

      std::map<AnsiString, shared_ptr<ColumnPositions> > _mapTableColumns;
   };
}