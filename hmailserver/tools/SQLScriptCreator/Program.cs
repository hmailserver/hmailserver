// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using SQLScriptCreator.Statements;

namespace SQLScriptCreator
{
   class Program
   {
      static int Main(string[] args)
      {
         if (args.Length != 2)
         {
            Console.WriteLine("Two parameters expected.");
            Console.WriteLine("SQLScriptCreator inputfile.xml outputdir");
            return -1;
         }

         string fileName = args[0];
         string outputDir = args[1];

         if (!File.Exists(fileName))
         {
            Console.WriteLine("Input file not found.");
            return -1;
         }

         if (!Directory.Exists(outputDir))
         {
            Console.WriteLine("Output directory not found.");
            return -1;
         }

         try
         {
            DDLFileParser parser = new DDLFileParser(fileName);
            List<IStatement> statements = parser.Parse();

            ScriptGenerator generator = new ScriptGenerator(statements, outputDir);
            generator.Generate();



         }
         catch (Exception e)
         {
            Console.WriteLine(e.Message);
            return -1;
         }

         return 0;

      }
   }
}
