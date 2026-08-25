using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;

namespace VMTestRunner.Console
{
   public enum TestStatus
   {
      Pending,
      Running,
      Passed,
      Failed
   }

   /// <summary>
   /// Renders one console row per test and keeps the rows updated while the tests run.
   /// Log messages are printed above the rows, so the rows always stay at the bottom.
   /// </summary>
   public sealed class TestStatusBoard
   {
      public static TestStatusBoard Instance { get; } = new TestStatusBoard();

      private const string PendingText = "Waiting to start";

      private readonly object _lock = new object();
      private readonly List<Row> _rows = new List<Row>();

      private Timer _timer;
      private bool _interactive;
      private bool _finished;
      private bool _cursorWasVisible = true;
      private int _drawnLines;
      private int _nameWidth;

      private sealed class Row
      {
         public string Name;
         public TestStatus Status = TestStatus.Pending;
         public string LastOutput = PendingText;
         public DateTime StartedUtc;
         public TimeSpan Duration;
      }

      /// <summary>
      /// Creates one row per test. Rows are addressed by a one-based index.
      /// </summary>
      public void Initialize(IEnumerable<string> testNames)
      {
         lock (_lock)
         {
            _rows.Clear();
            _rows.AddRange(testNames.Select(name => new Row { Name = name }));

            _nameWidth = _rows.Count == 0 ? 0 : _rows.Max(row => row.Name.Length);

            _interactive = !System.Console.IsOutputRedirected;

            if (!_interactive)
               return;

            // The rows are redrawn in place; a visible caret would jump around while they are.
            HideCursor();

            DrawRows();

            _timer = new Timer(_ => Tick(), null, 1000, 1000);
         }
      }

      public void SetRunning(int testIndex, string latestOutput)
      {
         lock (_lock)
         {
            var row = GetRow(testIndex);

            if (row == null)
               return;

            if (row.Status != TestStatus.Running)
            {
               row.Status = TestStatus.Running;
               row.StartedUtc = DateTime.UtcNow;
            }

            row.LastOutput = latestOutput;

            Redraw();
         }
      }

      public void SetCompleted(int testIndex, TestStatus status, TimeSpan duration)
      {
         lock (_lock)
         {
            var row = GetRow(testIndex);

            if (row == null)
               return;

            row.Status = status;
            row.Duration = duration;

            if (_interactive)
               Redraw();
            else
               System.Console.WriteLine(Render(row));
         }
      }

      /// <summary>
      /// Writes a log message above the rows.
      /// </summary>
      public void WriteLine(string text, ConsoleColor? color)
      {
         lock (_lock)
         {
            if (!_interactive || _finished)
            {
               WriteColored(text, color);
               return;
            }

            MoveToBoardTop();

            foreach (var line in text.Split(new[] { "\r\n", "\n" }, StringSplitOptions.None))
               WriteColored(Fit(line), color);

            DrawRows();
         }
      }

      /// <summary>
      /// Stops updating the rows. Called when all tests have completed. Anything written
      /// after this ends up below the rows.
      /// </summary>
      public void Complete()
      {
         lock (_lock)
         {
            _timer?.Dispose();
            _timer = null;

            if (_interactive)
            {
               Redraw();
               RestoreCursor();
            }

            _finished = true;
         }
      }

      private void HideCursor()
      {
         try
         {
            _cursorWasVisible = System.Console.CursorVisible;
            System.Console.CursorVisible = false;

            // Make sure the caret comes back if the run is interrupted.
            AppDomain.CurrentDomain.ProcessExit += (sender, e) => RestoreCursor();
            System.Console.CancelKeyPress += (sender, e) => RestoreCursor();
         }
         catch (Exception)
         {
            // Console may not support this; the rows still render.
         }
      }

      private void RestoreCursor()
      {
         try
         {
            System.Console.CursorVisible = _cursorWasVisible;
         }
         catch (Exception)
         {
         }
      }

      private Row GetRow(int testIndex) =>
         testIndex >= 1 && testIndex <= _rows.Count ? _rows[testIndex - 1] : null;

      private void Tick()
      {
         lock (_lock)
         {
            if (_interactive && _rows.Any(row => row.Status == TestStatus.Running))
               Redraw();
         }
      }

      private void Redraw()
      {
         if (!_interactive)
            return;

         MoveToBoardTop();
         DrawRows();
      }

      private void MoveToBoardTop()
      {
         if (_drawnLines == 0)
            return;

         try
         {
            System.Console.SetCursorPosition(0, Math.Max(0, System.Console.CursorTop - _drawnLines));
         }
         catch (Exception)
         {
            // Console may have been resized or redirected; keep going.
         }

         _drawnLines = 0;
      }

      private void DrawRows()
      {
         foreach (var row in _rows)
            WriteColored(Fit(Render(row)), GetColor(row.Status));

         _drawnLines = _rows.Count;
      }

      private string Render(Row row)
      {
         switch (row.Status)
         {
            case TestStatus.Running:
               var elapsed = DateTime.UtcNow - row.StartedUtc;
               return $"{row.Name.PadRight(_nameWidth)}  Running [{FormatDuration(elapsed)}]: {row.LastOutput}";
            case TestStatus.Passed:
               return $"{row.Name.PadRight(_nameWidth)}  Completed after {FormatDuration(row.Duration)}";
            case TestStatus.Failed:
               return $"{row.Name.PadRight(_nameWidth)}  Failed after {FormatDuration(row.Duration)}";
            default:
               return $"{row.Name.PadRight(_nameWidth)}  {PendingText}";
         }
      }

      private static ConsoleColor? GetColor(TestStatus status)
      {
         switch (status)
         {
            case TestStatus.Passed: return ConsoleColor.Green;
            case TestStatus.Failed: return ConsoleColor.Red;
            case TestStatus.Pending: return ConsoleColor.DarkGray;
            default: return null;
         }
      }

      public static string FormatDuration(TimeSpan duration) =>
         $"{(int)duration.TotalMinutes:D2}:{duration.Seconds:D2}";

      /// <summary>
      /// Truncates and pads the text so that it occupies exactly one console line.
      /// </summary>
      private static string Fit(string text)
      {
         var width = GetWidth();

         if (width <= 0)
            return text;

         return text.Length > width ? text.Substring(0, width) : text.PadRight(width);
      }

      private static int GetWidth()
      {
         try
         {
            return System.Console.WindowWidth - 1;
         }
         catch (Exception)
         {
            return 0;
         }
      }

      private static void WriteColored(string text, ConsoleColor? color)
      {
         if (color.HasValue)
            System.Console.ForegroundColor = color.Value;

         System.Console.WriteLine(text);

         if (color.HasValue)
            System.Console.ResetColor();
      }
   }
}
