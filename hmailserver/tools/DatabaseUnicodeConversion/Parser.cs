// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using Microsoft.VisualBasic;
 
namespace DatabaseUnicodeConversion
{
    class Parser
    {
        private List<string> outCommands;
        private List<string> outCommandsMySQL;

        public void Run(string sInputFile, string sOutputFile)
        {
            outCommands = new List<string>();
            outCommandsMySQL = new List<string>();

            TextReader reader = File.OpenText(sInputFile);
            string sFileContents = reader.ReadToEnd();
            reader.Close();

            sFileContents = sFileContents.ToLower();

            string splitter = Environment.NewLine + Environment.NewLine;
            string[] sqlCommands = Microsoft.VisualBasic.Strings.Split(sFileContents, splitter, -1, CompareMethod.Text);

            foreach (string command in sqlCommands)
            {
                ExtractCommand(command);
            }

            if (File.Exists(sOutputFile))
                File.Delete(sOutputFile);

            sFileContents = "";
            foreach (string command in outCommands)
            {
                sFileContents += command + Environment.NewLine + "";
                sFileContents += Environment.NewLine;
            }

            sFileContents = sFileContents.Replace("---", "");

            File.WriteAllText(sOutputFile, sFileContents);
        }

        public void ExtractCommand(string command)
        {
            if (command.StartsWith("if"))
                return;

            if (command.StartsWith("create table"))
            {
                ParseCreateTableCommand(command);
                return;
            }

            if (command.StartsWith("create index") ||
                command.StartsWith("create clustered index") ||
                command.StartsWith("alter table"))
            {
                outCommands.Add(command);
            }

        }

        private void ParseCreateTableCommand(string command)
        {
            string[] lines = Microsoft.VisualBasic.Strings.Split(command, Environment.NewLine, -1, CompareMethod.Text);
            
            string tableName = "";


            foreach (string line in lines)
            {
                string trimmedLine = line.TrimStart(" \t".ToCharArray());

                if (trimmedLine.StartsWith("create table"))
                {
                    int iTableNameStart = command.IndexOf("create table") + "create table".Length + 1;
                    int iTableNameEnd1 = command.IndexOf(" ", iTableNameStart + 1);
                    int iTableNameEnd2 = command.IndexOf(Environment.NewLine, iTableNameStart + 1);

                    int iTableNameEnd = Math.Min(iTableNameEnd1, iTableNameEnd2);

                    int iTableNameLen = iTableNameEnd - iTableNameStart;

                    tableName = command.Substring(iTableNameStart, iTableNameLen);

                    // Add queries for dropping all indexes

                    // MSSQL:
                    outCommands.Add("hm_drop_table_objects '" + tableName + "'");

                    outCommandsMySQL.Add("ALTER TABLE " + tableName + " CONVERT TO CHARACTER SET utf8");

                    continue;
                }

                // column definition
                string[] columnProperties = Microsoft.VisualBasic.Strings.Split(trimmedLine, " ", -1, CompareMethod.Text);
                if (columnProperties.Length < 2)
                    continue;

                string columnName = columnProperties[0];
                string columnType = columnProperties[1];

      
                if (columnType.StartsWith("nvarchar"))
                {
                    string alterCommand = "ALTER TABLE " +  tableName + " ALTER COLUMN " + columnName + " " + columnType;
                    outCommands.Add(alterCommand);
                    continue;
                }

                if (columnType.StartsWith("ntext"))
                {
                    string alterCommand = "ALTER TABLE " + tableName +  " ADD " + columnName + "_tmp ntext";
                    outCommands.Add(alterCommand);

                    alterCommand = "UPDATE " + tableName + " SET " + columnName + "_tmp = " + columnName;
                    outCommands.Add(alterCommand);

                    alterCommand = "ALTER TABLE " + tableName + " DROP COLUMN " + columnName;
                    outCommands.Add(alterCommand);

                    alterCommand = "sp_rename '" + tableName + "." + columnName + "_tmp', '" + columnName + "', 'COLUMN'";
                    outCommands.Add(alterCommand);
                    continue;
                }
                
            }            
        }
    }
}
