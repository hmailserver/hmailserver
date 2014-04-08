// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SQLScriptCreator.Statements;

namespace SQLScriptCreator.Generators
{
   class SQLCEGenerator : IGenerator
   {
      public string Name
      {
         get
         {
            return "SQLCE";
         }
      }

      public string StatementSeparator
      {
         get { return "\r\n\r\n"; }
      }

      public List<string> GenerateRenameColumnStatement(RenameColumn renameStatement)
      {
         List<string> result = new List<string>();

         string sqlExpression = string.Format("@@@HM_DROP_COLUMN_OBJECTS {0}.{1}@@@", renameStatement.Table, renameStatement.OldName);
         result.Add(sqlExpression);

         sqlExpression = string.Format("ALTER TABLE {0} ADD {1} {2} {3} DEFAULT {4}", 
            renameStatement.Table, 
            renameStatement.NewName, 
            renameStatement.DataType,
            renameStatement.Nullable ? "NULL" : "NOT NULL",
            renameStatement.Default);

         result.Add(sqlExpression);

         sqlExpression = string.Format("UPDATE {0} SET {1} = {2}",
            renameStatement.Table,
            renameStatement.NewName,
            renameStatement.OldName);

         result.Add(sqlExpression);

         sqlExpression = string.Format("ALTER TABLE {0} DROP COLUMN {1}",
            renameStatement.Table,
            renameStatement.OldName);

         result.Add(sqlExpression);

         return result;
      }

      public List<string> GenerateAddColumnStatement(AddColumn addColumnStatement)
      {

         List<string> result = new List<string>();
         StringBuilder sb = new StringBuilder();
         sb.AppendFormat("ALTER TABLE {0} ADD {1} {2} {3}", addColumnStatement.Table, addColumnStatement.Name, addColumnStatement.DataType, addColumnStatement.Nullable ? "NULL" : "NOT NULL");

         return new List<string>() { sb.ToString() };
      }

      public List<string> GenerateCreateIndexStatement(CreateIndex statement)
      {
         StringBuilder columnList = new StringBuilder();
         foreach (CreateIndex.Column col in statement.Columns)
         {
            if (columnList.Length > 0)
               columnList.Append(", ");

            columnList.Append(col.Name);
         }

         /*
            ALTER TABLE hm_accounts ADD CONSTRAINT test1 UNIQUE NONCLUSTERED (accountid)
            
            Clustered indexes are not supported in SQL CE.
          */
         StringBuilder sb = new StringBuilder();
         sb.AppendFormat("CREATE {0} NONCLUSTERED INDEX {1} ON {2} ({3})",
            statement.Unique ? "UNIQUE" : "",
            statement.Name,
            statement.Table,
            columnList.ToString());

         return new List<string>() { sb.ToString() };
      }

      public List<string> GenerateDropColumnObjectsStatement(DropColumnObjects statement)
      {
         StringBuilder sb = new StringBuilder();
         sb.AppendFormat("@@@HM_DROP_COLUMN_OBJECTS {0}.{1}@@@",
            statement.TableName,
            statement.ColumnName
            );

         return new List<string>() { sb.ToString() };
      }

      public List<string> GenerateRawSQLStatement(RawSQL statement)
      {
         if (statement.Engines.Count > 0 &&
            statement.Engines.Contains("SQLCE") == false)
            return new List<string>() { };

         return new List<string>() { statement.Statement };
      }
   }
}
