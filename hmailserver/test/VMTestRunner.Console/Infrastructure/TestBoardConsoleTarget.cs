using System;
using NLog;
using NLog.Targets;

namespace VMTestRunner.Console
{
   /// <summary>
   /// NLog target which feeds the test rows of <see cref="TestStatusBoard"/> and prints
   /// warnings and errors above them. Everything else only goes to the log file.
   /// </summary>
   [Target("TestBoardConsole")]
   public sealed class TestBoardConsoleTarget : TargetWithLayout
   {
      public const string TestIndexProperty = "testIndex";
      public const string FileOnlyProperty = "fileOnly";

      protected override void Write(LogEventInfo logEvent)
      {
         if (logEvent.Properties.ContainsKey(FileOnlyProperty))
            return;

         if (logEvent.Level == LogLevel.Debug && logEvent.Properties.TryGetValue(TestIndexProperty, out var testIndex))
         {
            TestStatusBoard.Instance.SetRunning(Convert.ToInt32(testIndex), logEvent.FormattedMessage);
            return;
         }

         // Informational messages would only push the test rows around; they are kept in the log file.
         if (logEvent.Level < LogLevel.Warn)
            return;

         var color = logEvent.Level >= LogLevel.Error ? ConsoleColor.Red : ConsoleColor.Yellow;

         TestStatusBoard.Instance.WriteLine(RenderLogEvent(Layout, logEvent), color);
      }
   }
}
