// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Diagnostics;

namespace Builder.Common
{
   internal class BuildStepInnoSetup : BuildStep
   {
      private readonly string _project;

      public BuildStepInnoSetup(Builder builder, string project)
      {
         _builder = builder;
         _project = project;
      }


      public override string Name
      {
         get { return "Create installation " + _project; }
      }

      public override void Run()
      {
         _builder.Log("Running InnoSetup on " + ExpandMacros(_project) + "...\r\n", true);

         string sArguments = "\"" + ExpandMacros(_project) + "\"";

         var processCompile = new Process
         {
            StartInfo =
            {
               UseShellExecute = false,
               RedirectStandardOutput = true,
               CreateNoWindow = true,
               FileName = ExpandMacros(_builder.ParameterInnoSetupPath),
               Arguments = sArguments
            }
         };

         processCompile.Start();

         // Capture the result
         string output = processCompile.StandardOutput.ReadToEnd();

         processCompile.WaitForExit();


         _builder.Log(output + "\r\n", true);

         if (output.IndexOf("0 succeeded", StringComparison.Ordinal) >= 0)
            throw new Exception("Innosetup compilation failed");
      }
   }
}