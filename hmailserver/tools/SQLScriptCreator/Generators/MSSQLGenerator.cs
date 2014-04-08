// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SQLScriptCreator.Statements;

namespace SQLScriptCreator.Generators
{
   class MSSQLGenerator : IGenerator
   {
      public string Name
      {
         get
         {
            return "MSSQL";
         }
      }

      public string StatementSeparator
      {
         get {return "\r\n\r\n";}
      }

      public List<string> GenerateRenameColumnStatement(RenameColumn renameColumn)
      {
         StringBuilder sb = new StringBuilder();
         sb.AppendFormat("sp_rename '{0}.{1}', '{2}', 'COLUMN'", renameColumn.Table, renameColumn.OldName, renameColumn.NewName);
         
         return new List<string> () {sb.ToString()};
      }

      public List<string> GenerateAddColumnStatement(AddColumn addColumn)
      {
         StringBuilder sb = new StringBuilder();
         sb.AppendFormat("ALTER TABLE {0} ADD {1} {2} {3}", addColumn.Table, addColumn.Name, addColumn.DataType, addColumn.Nullable ? "NULL" : "NOT NULL");

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
            ALTER TABLE hm_accounts ADD CONSTRAINT test2 UNIQUE CLUSTERED (accountid)
          */
         StringBuilder sb = new StringBuilder();
         sb.AppendFormat("CREATE {0} {1} INDEX {2} ON {3} ({4})",
            statement.Unique ? "UNIQUE" : "",
            statement.Clustered ? "CLUSTERED" : "NONCLUSTERED",
            statement.Name,
            statement.Table,
            columnList.ToString());

         return new List<string>() { sb.ToString() };
      }

      public List<string> GenerateDropColumnObjectsStatement(DropColumnObjects statement)
      {
         StringBuilder sb = new StringBuilder();
         sb.AppendFormat("hm_drop_column_objects '{0}', '{1}'",
            statement.TableName,
            statement.ColumnName
            );

         return new List<string>() { sb.ToString() };
      }

      public List<string> GenerateRawSQLStatement(RawSQL statement)
      {
         if (statement.Engines.Count > 0 &&
             statement.Engines.Contains("MSSQL") == false)
              return new List<string>(){};

         return new List<string>() { statement.Statement };
      }
   }
}
