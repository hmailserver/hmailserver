// Copyright (c) 2009 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "MSSQLMacroExpander.h"
#include "Macro.h"
#include "../ADORecordset.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   bool
   MSSQLMacroExpander::ProcessMacro(std::shared_ptr<DALConnection> connection, const Macro &macro, String &sErrorMessage)
   {
      switch (macro.GetType())
      {
      case Macro::DropColumnKeys:
         
         SQLCommand command("select DISTINCT CONSTRAINT_NAME from INFORMATION_SCHEMA.KEY_COLUMN_USAGE WHERE TABLE_NAME = @TABLE_NAME AND COLUMN_NAME = @COLUMN_NAME");
         command.AddParameter("@TABLE_NAME", macro.GetTableName());
         command.AddParameter("@COLUMN_NAME", macro.GetColumnName());

         ADORecordset rec;
         if (!rec.Open(connection, command))
         {
            sErrorMessage = "It was not possible to execute the below SQL statement. Please see hMailServer error log for details.\r\n" + command.GetQueryString();
            return false;
         }

         while (!rec.IsEOF())
         {
            String constraintName = rec.GetStringValue("CONSTRAINT_NAME");

            String sqlUpdate;
            sqlUpdate.Format(_T("ALTER TABLE %s DROP %s"), macro.GetTableName().c_str(), constraintName.c_str());

            DALConnection::ExecutionResult execResult = connection->TryExecute(SQLCommand(sqlUpdate), sErrorMessage, 0, 0);

            if (execResult != DALConnection::DALSuccess)
               return false;

            rec.MoveNext();
         }
      }

      return true;
   }
}
