// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

using System;
using System.IO;
using System.Linq;
using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.Infrastructure
{
   /// <summary>
   /// The server looks for hMailServer.ini in ProgramData first and falls back to the
   /// Bin directory. These tests confirm that the file the server actually loaded is
   /// the one that lookup finds.
   /// </summary>
   [TestFixture]
   public class IniFileLocation : TestFixtureBase
   {
      [Test]
      [Description("The ini file the server uses should exist where the lookup expects it")]
      public void IniFileShouldExist()
      {
         Assert.IsTrue(File.Exists(IniFileLocator.GetIniFileName()));
      }

      [Test]
      [Description("The directories the server reports should come from the located ini file")]
      public void DirectoriesShouldMatchLocatedIniFile()
      {
         var directories = SingletonProvider<TestSetup>.Instance.GetApp().Settings.Directories;

         Assert.AreEqual(ReadIniSetting("DataFolder"), directories.DataDirectory.TrimEnd('\\'));
         Assert.AreEqual(ReadIniSetting("LogFolder"), directories.LogDirectory.TrimEnd('\\'));
         Assert.AreEqual(ReadIniSetting("EventFolder"), directories.EventDirectory.TrimEnd('\\'));
      }

      [Test]
      [Description("An ini file in ProgramData takes precedence over the one in Bin")]
      public void ProgramDataShouldTakePrecedenceOverBin()
      {
         var iniFile = IniFileLocator.GetIniFileName();
         var programDataFile = Path.Combine(IniFileLocator.GetProgramDataDirectory(), "hMailServer.ini");

         if (!File.Exists(programDataFile))
            Assert.Inconclusive("This installation keeps hMailServer.ini in the Bin directory.");

         Assert.AreEqual(programDataFile, iniFile);
      }

      private static string ReadIniSetting(string key)
      {
         // Read without taking a lock, so that the server can keep writing to the file.
         using (var fileStream = new FileStream(IniFileLocator.GetIniFileName(), FileMode.Open, FileAccess.Read,
            FileShare.ReadWrite))
         using (var reader = new StreamReader(fileStream))
         {
            var line = reader.ReadToEnd()
               .Split(new[] {"\r\n", "\n"}, StringSplitOptions.None)
               .FirstOrDefault(l => l.StartsWith(key + "=", StringComparison.OrdinalIgnoreCase));

            if (line == null)
               throw new InvalidOperationException(string.Format("{0} is not set in hMailServer.ini.", key));

            return line.Substring(key.Length + 1).TrimEnd('\\');
         }
      }
   }
}
