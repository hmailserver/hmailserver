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
      _columns(columns)
   {

   }

   int 
   ColumnPositions::GetColumnIndex(const AnsiString &columnName)
   {
      for (unsigned int i = 0; i < _columns.size(); i++)
      {
         if (_columns[i] == columnName)
            return i;
      }

      return -1;
   }

   ColumnPositionCache::ColumnPositionCache()
   {

   }


   shared_ptr<ColumnPositions> 
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

      shared_ptr<ColumnPositions> positions;

      std::map<AnsiString, shared_ptr<ColumnPositions> >::iterator iter = _mapTableColumns.find(selectIdentifier);
      if (iter == _mapTableColumns.end())
      {
         positions = shared_ptr<ColumnPositions>(new ColumnPositions(pRecordset->GetColumnNames()));
         _mapTableColumns[selectIdentifier] = positions;
      }
      else
      {
         positions = (*iter).second;
      }

      return positions;
   }


}
