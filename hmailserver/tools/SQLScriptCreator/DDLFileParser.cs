// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using SQLScriptCreator.Statements;

namespace SQLScriptCreator
{
   class DDLFileParser
   {
      private string _inputFile;

      public DDLFileParser(string inputFile)
      {
         _inputFile = inputFile;
      }

      public List<IStatement> Parse()
      {
         List<IStatement> result = new List<IStatement>();
         XmlDocument inputDocument = new XmlDocument();
         inputDocument.Load(_inputFile);

         if (inputDocument.ChildNodes.Count != 2)
            throw new Exception("Incorrect node count during parse.");

         XmlNode statementNode = inputDocument.ChildNodes[1];
         
         foreach (XmlNode statement in statementNode.ChildNodes)
         {
            if (!(statement is System.Xml.XmlElement))
               continue;

            switch (statement.Name)
            {
               case "RenameColumn":
                  result.Add(ParseRenameColumnStatement(statement));
                  break;
               case "AddColumn":
                  result.Add(ParseAddColumnStatement(statement));
                  break;
               case "CreateIndex":
                  result.Add(ParseCreateIndexStatement(statement));
                  break;
               case "DropColumnObjects":
                  result.Add(ParseDropColumnObjectsStatement(statement));
                  break;
               case "RawSQL":
                  result.Add(ParseRawSQLStatement(statement));
                  break;
               default:
                  throw new Exception("Unknown statement type: " + statement.Name);
            }


         }


         return result;
      }

      private IStatement ParseRenameColumnStatement(XmlNode statement)
      {
         RenameColumn renameStatement = new RenameColumn();
         renameStatement.Table = statement.Attributes["Table"].Value;
         renameStatement.OldName = statement.Attributes["OldName"].Value;
         renameStatement.NewName = statement.Attributes["NewName"].Value;
         renameStatement.DataType = statement.Attributes["DataType"].Value;
         renameStatement.Nullable = Convert.ToBoolean(statement.Attributes["Nullable"].Value);
         renameStatement.Default = statement.Attributes["Default"].Value;

         return renameStatement;
      }

      private IStatement ParseAddColumnStatement(XmlNode statement)
      {
         AddColumn addStatement = new AddColumn();
         addStatement.Table = statement.Attributes["Table"].Value;
         addStatement.Name = statement.Attributes["Name"].Value;
         addStatement.DataType = statement.Attributes["DataType"].Value;
         addStatement.Nullable = Convert.ToBoolean(statement.Attributes["Nullable"].Value);
         addStatement.Default = statement.Attributes["Default"].Value;

         return addStatement;
      }

      private IStatement ParseCreateIndexStatement(XmlNode statement)
      {
         CreateIndex createIndexStatement = new CreateIndex();
         createIndexStatement.Name = statement.Attributes["Name"].Value;
         createIndexStatement.Table = statement.Attributes["Table"].Value;
         createIndexStatement.Unique = Convert.ToBoolean(statement.Attributes["Unique"].Value);
         createIndexStatement.Clustered = Convert.ToBoolean(statement.Attributes["Clustered"].Value);
         
         foreach (XmlNode childNode in statement.ChildNodes)
         {
            if (!(statement is System.Xml.XmlElement))
               continue;

            CreateIndex.Column col = new CreateIndex.Column();
            col.Name = childNode.Attributes["Name"].Value;

            XmlAttribute attribute = childNode.Attributes["MaxLength"];
            if (attribute != null)
               col.MaxLength = Convert.ToInt32(attribute.Value.Length);

            createIndexStatement.Columns.Add(col);
         }


         return createIndexStatement;
      }

      private IStatement ParseDropColumnObjectsStatement(XmlNode statement)
      {
         DropColumnObjects dropStatement = new DropColumnObjects();
         dropStatement.ColumnName = statement.Attributes["Column"].Value;
         dropStatement.TableName = statement.Attributes["Table"].Value;

         return dropStatement;
      }

      private IStatement ParseRawSQLStatement(XmlNode statement)
      {
         RawSQL rawSQLStatement = new RawSQL();
         rawSQLStatement.Statement = statement.InnerText;

         foreach (XmlAttribute attribute in statement.Attributes)
         {
            if (string.Compare(attribute.Name, "Engine", true) == 0)
            {
               rawSQLStatement.Engines.Add(attribute.Value);
            }
         }


         return rawSQLStatement;
      }
   }
}
