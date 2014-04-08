// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace APIDocumentationCreator
{
    class Program
    {
        const string inputFile = @"C:\Dev\hMailServer\Dev\trunk\source\hMailServer\hMailServer.idl";
        const string outputDirectory = @"C:\Temp\COMAPI";

        static void Main(string[] args)
        {
            // Make sure that the output directory exists.
            if (!Directory.Exists(outputDirectory))
                Directory.CreateDirectory(outputDirectory);

            string fileContent = File.ReadAllText(inputFile);

            string[] lines = fileContent.SplitString(Environment.NewLine);

            Parser parser = new Parser();
            foreach (string line in lines)
            {
                parser.ParseLine(line);
            }

            HTMLGenerator generator = new HTMLGenerator();
            generator.Generate(parser, outputDirectory);
        }

    }
}
