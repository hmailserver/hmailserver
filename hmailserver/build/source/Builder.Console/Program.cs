// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Threading;
using Builder.Common;

namespace Builder.Console
{
   internal class Program
   {
      private static void Main(string[] args)
      {
         if (args.Length != 3)
         {
            System.Console.WriteLine("Wrong number of parameters passed to Builder.Console");
            Environment.ExitCode = -1;
            return;
         }

         var settings = new Settings();
         settings.LoadSettings();

         var loader = new BuildLoader();
         Common.Builder builder = loader.Load(settings.BuildInstructions);


         // Run all steps.
         builder.StepStart = -1;
         builder.StepEnd = -1;

         builder.LoadMacros(args[0], args[1], args[2]);

         string result;
         if (!settings.ValidateSettings(builder, out result))
         {
            System.Console.WriteLine(result);
            Environment.ExitCode = -1;
            return;
         }

         builder.LoadSettings(settings);


         var eventStopThread = new ManualResetEvent(false);
         var eventThreadStopped = new ManualResetEvent(false);

         var runner = new BuildRunner(eventStopThread, eventThreadStopped, builder);
         runner.StepError += runner_StepError;
         builder.MessageLog += builder_MessageLog;
         runner.Run();
      }

      private static void builder_MessageLog(bool timestamp, string message)
      {
         if (timestamp)
            System.Console.Write(DateTime.Now + " - ");

         System.Console.WriteLine(message);
      }

      private static void runner_StepError(int stepindex, string errorMessage)
      {
         System.Console.WriteLine(errorMessage);
         Environment.ExitCode = -1;
      }
   }
}