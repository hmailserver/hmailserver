// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Reflection;

namespace VMwareIntegration.Common
{
   public class TestSettings
   {
      /// <summary>
      /// Determine the output executable to test.
      /// </summary>
      /// <returns></returns>
      public static string GetSoftwareUnderTest()
      {
         string currentDir = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
         string outputDir = Path.Combine(currentDir, @"..\..\..\..\..\Installation\Output");

         DirectoryInfo dir = new DirectoryInfo(outputDir);
         FileInfo[] files = dir.GetFiles("*.exe");

         List<string> fileNames = new List<string>();
         foreach (FileInfo file in files)
         {
            fileNames.Add(file.Name);
         }
         fileNames.Sort();

         string last = fileNames[files.Length - 1];

         return Path.GetFullPath(Path.Combine(outputDir, last));

      }

      /// <summary>
      /// Determine the fixture path.  
      /// </summary>
      /// <returns></returns>
      public static string GetFixturePath()
      {
         string currentDir = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
         string fixturePath = Path.Combine(currentDir, @"..\..\..\..\RegressionTests");

         DirectoryInfo dir = new DirectoryInfo(fixturePath);
         FileInfo[] files = dir.GetFiles("RegressionTests.sln");
         
         if (files.Length == 1)
            return Path.GetFullPath(fixturePath);
         else
            throw new Exception("TestRunner was not able to determine fixture path.");

      }

      /// <summary>
      /// Get test folder.
      /// </summary>
      /// <returns></returns>
      public static string GetTestFolder()
      {
         string currentDir = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
         string fixturePath = Path.Combine(currentDir, @"..\..\..\..\");

         return Path.GetFullPath(fixturePath);
      }
   }
}
