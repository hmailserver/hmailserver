using System;
using System.IO;

namespace VMTestRunner.Console
{
   public abstract class GuestSession : IGuestSession
   {
      private static readonly NLog.Logger Logger = NLog.LogManager.GetCurrentClassLogger();

      private readonly int _testIndex;

      protected GuestSession(int testIndex)
      {
         _testIndex = testIndex;
      }

      public abstract void WaitUntilReady();

      public abstract void CreateDirectory(string path);

      public abstract void CopyFileToGuest(string hostPath, string guestPath);

      public abstract void CopyFileToHost(string guestPath, string hostPath);

      public abstract void RunProgram(string fullPath, string parameters, bool throwOnFailure = false);

      public void CopyFolderToGuest(string source, string destination)
      {
         if (!Directory.Exists(source))
            throw new Exception($"CopyFolderToGuest: The source directory {source} does not exist.");

         CreateDirectory(destination);

         foreach (string fileName in Directory.GetFiles(source))
         {
            var fileInfo = new FileInfo(fileName);
            CopyFileToGuest(fileInfo.FullName, Path.Combine(destination, fileInfo.Name));
         }

         foreach (string subDir in Directory.GetDirectories(source))
         {
            var dirInfo = new DirectoryInfo(subDir);
            CopyFolderToGuest(subDir, Path.Combine(destination, dirInfo.Name));
         }
      }

      protected void Debug(string message)
      {
         // The test index tells the status board which row the message belongs to.
         var logEvent = new NLog.LogEventInfo(NLog.LogLevel.Debug, Logger.Name, $"[Test {_testIndex}] {message}");
         logEvent.Properties[TestBoardConsoleTarget.TestIndexProperty] = _testIndex;
         Logger.Log(logEvent);
      }

      public virtual void Dispose()
      {
      }
   }
}
