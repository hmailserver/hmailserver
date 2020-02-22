// Copyright (c) 2009 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "MySQLMacroExpander.h"
#include "Macro.h"

#include "../MySQLRecordset.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   bool
   MySQLMacroExpander::ProcessMacro(std::shared_ptr<DALConnection> connection, const Macro &macro, String &sErrorMessage)
   {
      switch (macro.GetType())
      {
      case Macro::DropColumnKeys:
         // MySQL4 doesn't support WHERE clauses in SHOW INDEX so
         // we must manually sort the result below.
         String sql;
         sql.Format(_T("SHOW INDEX IN %s"), macro.GetTableName().c_str());

         MySQLRecordset rec;
         if (!rec.Open(connection, SQLCommand(sql)))
         {
            sErrorMessage = "It was not possible to execute the below SQL statement. Please see hMailServer error log for details.\r\n" + sql;
            return false;
         }

         while (!rec.IsEOF())
         {
            String columnName = rec.GetStringValue("Column_name");

            if (columnName.CompareNoCase(macro.GetColumnName()) != 0)
            {
               // Wrong column
               rec.MoveNext();
               continue;
            }

            String constraintName = rec.GetStringValue("Key_name");

            String sqlUpdate;
            sqlUpdate.Format(_T("ALTER TABLE %s DROP INDEX %s"), macro.GetTableName().c_str(), constraintName.c_str());

            DALConnection::ExecutionResult execResult = connection->TryExecute(SQLCommand(sqlUpdate), sErrorMessage, 0, 0);

            if (execResult != DALConnection::DALSuccess)
               return false;

            rec.MoveNext();
         }
         break;
      }

      return true;
   }
}
