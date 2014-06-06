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
