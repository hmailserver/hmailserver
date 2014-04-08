// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SQLScriptCreator.Generators;
using System.IO;

namespace SQLScriptCreator.Statements
{
   class ScriptGenerator
   {
      private List<IStatement> _statements;
      private string _outputDir;

      public ScriptGenerator(List<IStatement> statements, string outputDir)
      {
         _outputDir = outputDir;
         _statements = statements;
      }

      public void Generate()
      {
         List<IGenerator> generators = new List<IGenerator>();
         generators.Add(new MSSQLGenerator());
         generators.Add(new SQLCEGenerator());
         generators.Add(new MySQLGenerator());
         generators.Add(new PGSQLGenerator());

         Dictionary<string, string> scripts = new Dictionary<string, string>();
         foreach (IStatement statement in _statements)
         {
            bool found = false;

            foreach (IGenerator generator in generators)
            {
               string sqlString = "";

               if (statement is AddColumn)
               {
                  foreach (string singleStatement in generator.GenerateAddColumnStatement(statement as AddColumn))
                  {
                     found = true;
                     sqlString += singleStatement;
                     sqlString += generator.StatementSeparator;
                  }
               }
               else if (statement is RenameColumn)
               {
                  foreach (string singleStatement in generator.GenerateRenameColumnStatement(statement as RenameColumn))
                  {
                     found = true;
                     sqlString += singleStatement;
                     sqlString += generator.StatementSeparator;
                  }
               }
               else if (statement is CreateIndex)
               {
                  foreach (string singleStatement in generator.GenerateCreateIndexStatement(statement as CreateIndex))
                  {
                     found = true;
                     sqlString += singleStatement;
                     sqlString += generator.StatementSeparator;
                  }
               }
               else if (statement is DropColumnObjects)
               {
                  foreach (string singleStatement in generator.GenerateDropColumnObjectsStatement(statement as DropColumnObjects))
                  {
                     found = true;
                     sqlString += singleStatement;
                     sqlString += generator.StatementSeparator;
                  }
               }
               else if (statement is DropColumnObjects)
               {
                  foreach (string singleStatement in generator.GenerateDropColumnObjectsStatement(statement as DropColumnObjects))
                  {
                     found = true;
                     sqlString += singleStatement;
                     sqlString += generator.StatementSeparator;
                  }
               }
               else if (statement is RawSQL)
               {
                  foreach (string singleStatement in generator.GenerateRawSQLStatement(statement as RawSQL))
                  {
                     if (singleStatement.Length > 0)
                        found = true;

                     string trimmedResult = singleStatement.Trim();

                     if (trimmedResult.EndsWith(";"))
                        throw new Exception("RawSQL statement terminated. Last statement should not be terminated: " + Environment.NewLine + trimmedResult);

                     sqlString += trimmedResult;
                     sqlString += generator.StatementSeparator;
                  }
               }
               else
                  throw new Exception("Unknown statement type:" + statement.ToString());
                              

               if (scripts.ContainsKey(generator.Name))
                  scripts[generator.Name] += sqlString;
               else
                  scripts[generator.Name] = sqlString;


            }


            if (found == false)
            {
               throw new Exception("SQL not generated for statement: " + statement.ToString());
            }
            
         }

         foreach (var script in scripts)
         {
            string engineName = script.Key;
            string fileName = string.Format("UpgradeXtoY{0}.sql", script.Key);

            fileName = Path.Combine(_outputDir, fileName);
            if (File.Exists(fileName))
               File.Delete(fileName);

            File.WriteAllText(fileName, script.Value, Encoding.ASCII);
         }
      }
   }
}

