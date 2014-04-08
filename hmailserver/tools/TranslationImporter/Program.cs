// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using MySql.Data.MySqlClient;

namespace TranslationImporter
{
   class Program
   {
      static void Main(string[] args)
      {
         using (MySqlConnection conn = new MySql.Data.MySqlClient.MySqlConnection("Database=hmailserver_webpage;Data Source=hmailserver.com;User Id=user;Password=password;CharSet=utf8"))
         {
            conn.Open();

            //string folder = @"C:\Dev\hMailServer\Build\branches\H-4-4\source\Translations";
            // string folder = @"C:\Temp\translation";
            string folder = @"C:\Temp\translation";

            DirectoryInfo dirInfo = new DirectoryInfo(folder);

            FileInfo[] files = dirInfo.GetFiles();

            foreach (FileInfo file in files)
            {
               ImportFile(file, conn);
            }

            conn.Close();
         }
      }

      static void ImportFile(FileInfo file, MySqlConnection conn)
      {
         string language = file.Name.Substring(0, file.Name.IndexOf("."));

         string contents = System.IO.File.ReadAllText(file.FullName);

         string [] lines = contents.Split("\r\n".ToCharArray());

         string completeStatement = "";
         foreach (string line in lines)
         {
            if (string.IsNullOrEmpty(line))
               continue;

            int equalsPos = line.IndexOf("=");
            if (equalsPos <= 0)
               continue;

            int underscorePos = line.IndexOf("_");

            string stringIndex = line.Substring(underscorePos+1, equalsPos-underscorePos-1);
            string value = line.Substring(equalsPos + 1);
            
            int stringID = Convert.ToInt32(stringIndex);

            value = value.Replace("'", "''");

            string sqlStatement = string.Format("update hm_tran_strings set language_{0} = '{1}' where stringid = {2}", language, value, stringID);



            completeStatement += sqlStatement + ";";
         }

         MySqlCommand command = new MySqlCommand(completeStatement, conn);
         command.ExecuteNonQuery();
      }
   }
}
