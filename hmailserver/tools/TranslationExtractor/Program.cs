// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Net;
using System.IO;
using System.Text;

namespace TranslationExtractor
{
   internal class Program
   {
      private static string TranslationFolder;

      private static string TranslationScript =
         "http://www.hmailserver.com/devnet/translation_getlanguage.php?language={0}";

      private static void Main(string[] args)
      {
          ServicePointManager.SecurityProtocol = SecurityProtocolType.Tls12;

         TranslationFolder = args[0];

         if (Directory.Exists(TranslationFolder) == false)
         {
            Console.WriteLine(string.Format("Translation folder {0} does not exist.", TranslationFolder));
            Environment.Exit(-1);
            return;
         }

         DownloadTranslation("english", TranslationFolder);
         DownloadTranslation("swedish", TranslationFolder);

      }

      private static void DownloadTranslation(string language, string targetDir)
      {
         var url = string.Format(TranslationScript, language);

         var request = (HttpWebRequest) WebRequest.Create(url);

         using (var response = request.GetResponse())
         using (var responseStream = response.GetResponseStream())
         using (var streamReader = new StreamReader(responseStream))
         {
            var content = streamReader.ReadToEnd();

            var targetFile = Path.Combine(targetDir, string.Format("{0}.ini", language));

            File.WriteAllText(targetFile, content, Encoding.UTF8);
         }

      }
   }
}
