// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SQLScriptCreator.Statements;

namespace SQLScriptCreator.Generators
{
   class MySQLGenerator : IGenerator
   {
      public string Name
      {
         get
         {
            return "MySQL";
         }
      }

      public string StatementSeparator
      {
         get { return ";\r\n\r\n"; }
      }

      public List<string> GenerateRenameColumnStatement(RenameColumn statement)
      {
         StringBuilder sb = new StringBuilder();
         sb.AppendFormat("ALTER TABLE {0} CHANGE {1} {2} {3} {4}", statement.Table,
               statement.OldName,
               statement.NewName,
               statement.DataType,
               statement.Nullable ? "NULL" : "NOT NULL"
               );

         return new List<string>() { sb.ToString() };
      }
       
      public List<string> GenerateAddColumnStatement(AddColumn statement)
      {
         StringBuilder sb = new StringBuilder();
         sb.AppendFormat("ALTER TABLE {0} ADD COLUMN {1} {2} {3}", statement.Table, statement.Name, statement.DataType, statement.Nullable ? "NULL" : "NOT NULL");

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
            if (col.MaxLength > 0)
            {
               columnList.AppendFormat("({0})", col.MaxLength);
            }
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
         sb.AppendFormat("@@@HM_DROP_COLUMN_OBJECTS {0}.{1}@@@",
            statement.TableName,
            statement.ColumnName
            );

         return new List<string>() { sb.ToString() };
      }

      public List<string> GenerateRawSQLStatement(RawSQL statement)
      {
         if (statement.Engines.Count > 0 &&
             statement.Engines.Contains("MySQL") == false)
            return new List<string>() { };

         if (statement.Statement.Contains(";"))
         {
            // Multiple statements.
            if (!statement.Statement.Contains(StatementSeparator))
            {
               throw new Exception("RawSQL statement with multiple statements must contain statement separator." + Environment.NewLine + statement.Statement);
            }
         }

         return new List<string>() { statement.Statement };
      }
   }
}
