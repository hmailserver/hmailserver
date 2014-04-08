// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Threading;

namespace Builder.Common
{
   public class BuildRunner
   {
      public delegate void StepCompletedDelegate(int stepIndex);

      public delegate void StepErrorDelegate(int stepindex, string errorMessage);

      public delegate void StepStartedDelegate(int stepIndex);

      public delegate void ThreadFinishedDelegate();

      private readonly Builder _builder;

      private readonly ManualResetEvent _eventStop;
      private readonly ManualResetEvent _eventStopped;

      public BuildRunner(
         ManualResetEvent eventStop,
         ManualResetEvent eventStopped,
         Builder builder)
      {
         _eventStop = eventStop;
         _eventStopped = eventStopped;

         _builder = builder;
      }

      public event StepStartedDelegate StepStarted;

      public event StepErrorDelegate StepError;

      public event StepCompletedDelegate StepCompleted;

      public event ThreadFinishedDelegate ThreadFinished;


      public void Run()
      {
         for (int i = 0; i < _builder.Count; i++)
         {
            if (_builder.RunAllSteps ||
                (i >= _builder.StepStart && i <= _builder.StepEnd))
            {
               BuildStep oStep = _builder.Get(i);

               if (StepStarted != null)
                  StepStarted(i);

               try
               {
                  oStep.Run();
               }
               catch (Exception e)
               {
                  if (StepError != null)
                     StepError(i, e.Message);

                  break;
               }

               if (StepCompleted != null)
                  StepCompleted(i);
            }

            if (_eventStop.WaitOne(1))
               break;
         }

         // Make synchronous call to main form
         // to inform it that thread finished
         if (ThreadFinished != null)
            ThreadFinished();


         // Reset the event
         _eventStopped.Set();
      }
   }
}