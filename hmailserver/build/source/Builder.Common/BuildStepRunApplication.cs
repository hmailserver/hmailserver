// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.IO;

namespace Builder.Common
{
   internal class BuildStepRunApplication : BuildStep
   {
      private readonly string _arguments;
      private readonly string _executable;

      public BuildStepRunApplication(Builder builder, string executable, string arguments)
      {
         _builder = builder;

         _executable = executable;
         _arguments = arguments;
      }


      public override string Name
      {
         get { return "Run application " + _executable; }
      }

      public override void Run()
      {
         _builder.Log("Running application " + _executable + "...\r\n", true);

         var launcher = new ProcessLauncher();
         launcher.Output += launcher_Output;

         string workingDirectory = Path.GetDirectoryName(ExpandMacros(_executable));

         string output;
         int exitCode = launcher.LaunchProcess(ExpandMacros(_executable), ExpandMacros(_arguments), workingDirectory,
            out output);

         if (exitCode != 0)
            throw new Exception(string.Format("Running of application failed. Exit code: {0}", exitCode));
      }

      private void launcher_Output(string output)
      {
         _builder.Log(output, false);
      }
   }
}