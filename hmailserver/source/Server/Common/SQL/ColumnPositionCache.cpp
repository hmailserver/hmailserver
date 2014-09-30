// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "ColumnPositionCache.h"

#include "DALRecordset.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   ColumnPositions::ColumnPositions()
   {

   }

   ColumnPositions::ColumnPositions(std::vector<AnsiString> columns):
      columns_(columns)
   {

   }

   int 
   ColumnPositions::GetColumnIndex(const AnsiString &columnName)
   {
      for (unsigned int i = 0; i < columns_.size(); i++)
      {
         if (columns_[i] == columnName)
            return i;
      }

      return -1;
   }

   ColumnPositionCache::ColumnPositionCache()
   {

   }


   std::shared_ptr<ColumnPositions> 
   ColumnPositionCache::GetPositions(const AnsiString &sqlStatement, DALRecordset *pRecordset)
   {
      // Extract a select statement identifier.
      AnsiString sql = sqlStatement;
      sql.ToLower();

      AnsiString selectIdentifier;
      int whereStartPos = sql.Find(" where ");
      if (whereStartPos >= 0)
         selectIdentifier = sql.Mid(0, whereStartPos);
      else
         selectIdentifier = sql;

      std::shared_ptr<ColumnPositions> positions;

      auto iter = map_table_columns_.find(selectIdentifier);
      if (iter == map_table_columns_.end())
      {
         positions = std::shared_ptr<ColumnPositions>(new ColumnPositions(pRecordset->GetColumnNames()));
         map_table_columns_[selectIdentifier] = positions;
      }
      else
      {
         positions = (*iter).second;
      }

      return positions;
   }


}
