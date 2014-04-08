// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.IO;

namespace Builder.Common
{
   internal class BuildStepCompileVSNet : BuildStep
   {
      private readonly string _configuratio;
      private readonly string _project;

      public BuildStepCompileVSNet(Builder builder, string project, string configuration)
      {
         _builder = builder;


         _project = project;
         _configuratio = configuration;
      }


      public override string Name
      {
         get { return "Compile " + _project; }
      }

      public override void Run()
      {
         _builder.Log("Compiling " + ExpandMacros(_project) + "...\r\n", true);

         string arguments =
            "\"" + ExpandMacros(_project) + "\" /rebuild \"" + ExpandMacros(_configuratio) + "\"";

         if (!_project.ToLower().EndsWith(".sln"))
            arguments += " /project \"" + ExpandMacros(_project) + "\"";

         var launcher = new ProcessLauncher();
         launcher.Output += launcher_Output;

         string output;
         string workingDirectory = Path.GetDirectoryName(_builder.ExpandMacros(_builder.ParameterVS8Path));

         int exitCode = launcher.LaunchProcess(_builder.ExpandMacros(_builder.ParameterVS8Path), arguments,
            workingDirectory, out output);

         if (exitCode != 0)
            throw new Exception(string.Format("Compilation failed. Exit code: {0}", exitCode));

         if (output.IndexOf("0 succeeded") >= 0)
            throw new Exception("Compilation failed.");
      }

      private void launcher_Output(string output)
      {
         _builder.Log(output, false);
      }
   }
}