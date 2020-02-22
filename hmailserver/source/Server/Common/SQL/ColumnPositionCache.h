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

      std::vector<AnsiString> columns_;
   };

   class ColumnPositionCache
   {
   public:

      ColumnPositionCache(void);

      std::shared_ptr<ColumnPositions> GetPositions(const AnsiString &sqlStatement, DALRecordset *pRecordset);

   private:

      std::map<AnsiString, std::shared_ptr<ColumnPositions> > map_table_columns_;
   };
}