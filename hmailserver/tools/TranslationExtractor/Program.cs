// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using MySql.Data.MySqlClient;
using System.IO;
using System.Configuration;

namespace TranslationExtractor
{
   class Program
   {
      static string TranslationFolder = null;

      static void Main(string[] args)
      {
         string username = ConfigurationSettings.AppSettings["MySQLUsername"];
         string password = ConfigurationSettings.AppSettings["MySQLPassword"];
         string port = ConfigurationSettings.AppSettings["MySQLPort"];
         
         TranslationFolder = args[0];

         if (Directory.Exists(TranslationFolder) == false)
         {
            Console.WriteLine(string.Format("Translation folder {0} does not exist.", TranslationFolder));
            Environment.Exit(-1);
            return;
         }

         if (string.IsNullOrEmpty(username))
         {
             Environment.Exit(-1);
             return;
         }

         Console.WriteLine(string.Format("Creating translation in {0}.", TranslationFolder));

         Console.WriteLine("Starting extraction...");

         string connectionString = string.Format("Database=hmailserver_webpage;Data Source=hmailserver.com;User Id={0};Password={1};CharSet=utf8;Port={2}", username, password, port);

         using (MySqlConnection conn = new MySql.Data.MySqlClient.MySqlConnection(connectionString))
         {
            conn.Open();

            List<string> languages = new List<string>();
            string sql = "select languageinternalname from hm_tran_languages order by languageinternalname asc";

            using (MySqlCommand command = new MySqlCommand(sql, conn))
            {
               using (MySqlDataReader reader = command.ExecuteReader())
               {
                  

                  while (reader.Read())
                  {
                     languages.Add(reader.GetString("languageinternalname"));
                  }

                  reader.Close();
               }
            }

            int numberOfStrings = 0;
            numberOfStrings = GetNumberOfStrings(conn);

            List<string> languagesToExtract = new List<string>();

            foreach (string language in languages)
            {
               if (language != "english" && language != "swedish")
               {
                  Console.WriteLine(string.Format("Skipped language {0} (hardcoded exclusion)", language));
                  continue;
               }

               int numberOfTranslatedStrings = GetNumberOfStringsInLanguage(conn, language);

               int translatedPercentage = Convert.ToInt32(((double)numberOfTranslatedStrings / (double)numberOfStrings) * (double)100);

               if (translatedPercentage < 95)
               {
                  Console.WriteLine(string.Format("Skipped language {0} ({1}%)", language, translatedPercentage));
                  continue;
               }

  

               languagesToExtract.Add(language);
               
            }

            string exportedLanguages = "";
            foreach (string language in languagesToExtract)
            {
               Console.Write(string.Format("Extract language {0}...", language));

               ExtractLanguage(conn, language);

               if (exportedLanguages.Length > 0)
                  exportedLanguages += ",";

               exportedLanguages += language;

               Console.WriteLine(" done");
            }

            Console.WriteLine(string.Format("Extraction completed. {0} languages were extracted.", languagesToExtract.Count));
            Console.WriteLine();
            Console.WriteLine("Line for InnoSetup:");

            string output = string.Format(
                  "Filename: \"{0}\"; Section: \"GUILanguages\"; Key: \"ValidLanguages\"; String: \"{1}\";",
                  "{app}\\Bin\\hMailServer.INI",
                  exportedLanguages
               );

            Console.WriteLine(output);
            Console.WriteLine();
         }

         Environment.Exit(0);
      }

      private static void ExtractLanguage(MySqlConnection conn, string language)
      {

         string destinationFile = Path.Combine(TranslationFolder, language + ".ini");

         if (File.Exists(destinationFile))
            File.Delete(destinationFile);

         string sql = string.Format("select stringid, language_{0} translation from hm_tran_strings order by stringid asc", language);

         string contents = "[Strings]" + Environment.NewLine;

         using (MySqlCommand command = new MySqlCommand(sql, conn))
         {
            using (MySqlDataReader reader = command.ExecuteReader())
            {
               while (reader.Read())
               {
                  int stringID = reader.GetInt32("stringID");
                  string value = reader.GetString("translation");

                  string line = string.Format("String_{0}={1}", stringID, value);

                  contents += line + Environment.NewLine;
               }

               reader.Close();
            }
         }

         File.WriteAllText(destinationFile, contents, Encoding.Unicode);
      }

      private static int GetNumberOfStrings(MySqlConnection conn)
      {
         string sql = "select count(*) c from hm_tran_strings";

         using (MySqlCommand command = new MySqlCommand(sql, conn))
         {
            using (MySqlDataReader reader = command.ExecuteReader())
            {
               if (reader.Read())
               {
                  int numberOfStrings = reader.GetInt32("c");
                  return numberOfStrings;
               }

               reader.Close();
            }
         }

         throw new Exception("Database connection failed");
      }

      private static int GetNumberOfStringsInLanguage(MySqlConnection conn, string language)
      {
         string sql = string.Format("select count(*) c from hm_tran_strings where language_{0} <> ''", language);

         using (MySqlCommand command = new MySqlCommand(sql, conn))
         {
            using (MySqlDataReader reader = command.ExecuteReader())
            {
               if (reader.Read())
               {
                  int numberOfStrings = reader.GetInt32("c");
                  return numberOfStrings;
               }

               reader.Close();
            }
         }

         throw new Exception("Database connection failed");
      }

   }
}
