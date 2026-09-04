// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using NUnit.Framework;

namespace VolumeTests
{
   public static class Shared
   {
      /// <summary>
      /// Locates a folder below test\TestData. The test assembly is built to different output
      /// folders depending on platform, so the folder is searched for rather than assumed.
      /// </summary>
      public static string GetTestDataPath(string relativePath)
      {
         var directory = new DirectoryInfo(TestContext.CurrentContext.TestDirectory);

         while (directory != null)
         {
            string candidate = Path.Combine(directory.FullName, "TestData");

            if (Directory.Exists(candidate))
               return Path.Combine(candidate, relativePath);

            directory = directory.Parent;
         }

         throw new Exception("Unable to locate the TestData folder above " +
                             TestContext.CurrentContext.TestDirectory);
      }

      public static long AssertLowMemoryUsage(long max)
      {
         System.Diagnostics.Process[] process = System.Diagnostics.Process.GetProcessesByName("hMailServer");
         if (process.Length != 1)
            throw new Exception("hMailServer.exe not running");

         long l = process[0].PrivateMemorySize64 / 1024 / 1024;

         Assert.Less(l, max);

         return process[0].PrivateMemorySize64;
      }

      public static int GetCurrentMemoryUsage()
      {
         System.Diagnostics.Process[] process = System.Diagnostics.Process.GetProcessesByName("hMailServer");
         if (process.Length != 1)
            throw new Exception("hMailServer.exe not running");

         return Convert.ToInt32((process[0].PrivateMemorySize64 / 1024 / 1024));
      }

      public static string GetExecutableName()
      {
         System.Diagnostics.Process[] process = System.Diagnostics.Process.GetProcessesByName("hMailServer");
         if (process.Length != 1)
            throw new Exception("hMailServer.exe not running");

         return process[0].MainModule.FileName;
         
      }
   }
}
