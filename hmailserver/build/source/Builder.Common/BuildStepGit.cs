// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;

namespace Builder.Common
{
   internal class BuildStepGit : BuildStep
   {
      public enum GITAction
      {
         RevertLocalChanges = 1,
         Pull = 2
      }

      private readonly GITAction _action;
      private readonly string _directory;


      public BuildStepGit(Builder oBuilder, GITAction action, string sDirectory)
      {
         _builder = oBuilder;

         _action = action;
         _directory = sDirectory;
      }


      public override string Name
      {
         get
         {
            switch (_action)
            {
               case GITAction.RevertLocalChanges:
                  return "Git Reset " + _directory;
               case GITAction.Pull:
                  return "Git Pull " + _directory;
            }

            return "<Unknown>";
         }
      }

      public override void Run()
      {
         string directory = ExpandMacros(_directory);

         string command = "";
         switch (_action)
         {
            case GITAction.RevertLocalChanges:
               _builder.Log("Reverting local changes in " + directory + "...\r\n", true);
               command = "checkout .";
               break;
            case GITAction.Pull:
               _builder.Log("Pulling " + directory + "...\r\n", true);
               command = "pull";
               break;
            default:
               throw new Exception("Failed. Unknown action.");
         }

         var launcher = new ProcessLauncher();
         launcher.Output += launcher_Output;
         string output;

         int exitCode = launcher.LaunchProcess(ExpandMacros(_builder.ParameterGitPath), ExpandMacros(command), directory,
            out output);

         if (exitCode != 0)
            throw new Exception(string.Format("{0} failed. Exit code: {1}. Output: {2}", command, exitCode, output));
      }

      private void launcher_Output(string output)
      {
         _builder.Log(output, true);
      }
   }
}