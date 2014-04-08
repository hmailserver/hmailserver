// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SQLScriptCreator.Statements;

namespace SQLScriptCreator.Generators
{
   class PGSQLGenerator : IGenerator
   {
      public string Name
      {
         get
         {
            return "PGSQL";
         }
      }

      public string StatementSeparator
      {
         get { return ";\r\n\r\n"; }
      }

      public List<string> GenerateRenameColumnStatement(RenameColumn renameColumn)
      {
         StringBuilder sb = new StringBuilder();
         sb.AppendFormat("ALTER TABLE {0} RENAME COLUMN {1} TO {2}",
               renameColumn.Table,
               renameColumn.OldName,
               renameColumn.NewName);

         return new List<string>() { sb.ToString() };
      }

      public List<string> GenerateAddColumnStatement(AddColumn addColumn)
      {
         StringBuilder sb = new StringBuilder();
         sb.AppendFormat("ALTER TABLE {0} ADD COLUMN {1} {2} {3}", addColumn.Table, addColumn.Name, addColumn.DataType, addColumn.Nullable ? "NULL" : "NOT NULL");

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
            CREATE INDEX idx_greylisting_triplets ON hm_greylisting_triplets (glipaddress, glsenderaddress, glrecipientaddress);
          */
         StringBuilder sb = new StringBuilder();
         sb.AppendFormat("CREATE {0} INDEX {1} ON {2} ({3})",
            statement.Unique ? "UNIQUE" : "",
            statement.Name,
            statement.Table,
            columnList.ToString());

         return new List<string>() { sb.ToString() };
      }

      public List<string> GenerateDropColumnObjectsStatement(DropColumnObjects statement)
      {
         StringBuilder sb = new StringBuilder();
         sb.AppendFormat("SELECT hm_drop_column_objects('{0}', '{1}')",
            statement.TableName,
            statement.ColumnName
            );

         return new List<string>() { sb.ToString() };
      }

      public List<string> GenerateRawSQLStatement(RawSQL statement)
      {
         if (statement.Engines.Count > 0 &&
            statement.Engines.Contains("PGSQL") == false)
            return new List<string>() { };

         return new List<string>() { statement.Statement };
      }
   }
}
