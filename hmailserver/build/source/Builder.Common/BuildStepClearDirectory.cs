// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.IO;

namespace Builder.Common
{
   internal class BuildStepClearDirectory : BuildStep
   {
      private readonly string _directory;

      public BuildStepClearDirectory(Builder builder, string directory)
      {
         if (string.IsNullOrEmpty(directory))
            throw new ArgumentNullException("Directory must be specified.", "directory");

         if (builder == null)
            throw new ArgumentNullException("Builder must be specified.", "builder");

         _builder = builder;
         _directory = directory;
      }

      public override string Name
      {
         get { return "Clear directory " + ExpandMacros(_directory); }
      }

      public override void Run()
      {
         string directoryName = ExpandMacros(_directory);

         _builder.Log("Clearing folder " + directoryName, true);

         if (!Directory.Exists(directoryName))
         {
            _builder.Log("Directory does not exist.", true);
            return;
         }

         string[] subDirs = Directory.GetDirectories(directoryName);
         foreach (string subDir in subDirs)
         {
            _builder.Log("Deleting directory " + subDir + "...", true);
            Directory.Delete(subDir, true);
         }

         string[] files = Directory.GetFiles(directoryName);

         foreach (string file in files)
         {
            _builder.Log("Deleting file " + file + "...", true);
            File.Delete(file);
         }
      }
   }
}