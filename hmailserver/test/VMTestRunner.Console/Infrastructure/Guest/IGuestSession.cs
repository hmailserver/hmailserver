using System;

namespace VMTestRunner.Console
{
   /// <summary>
   /// Everything the test runner needs to do inside a guest. There is one
   /// implementation per transport, since older guests can't be reached with
   /// PowerShell Direct.
   /// </summary>
   public interface IGuestSession : IDisposable
   {
      /// <summary>
      /// Blocks until the guest has booted and can be reached.
      /// </summary>
      void WaitUntilReady();

      void CreateDirectory(string path);

      void CopyFileToGuest(string hostPath, string guestPath);

      void CopyFileToHost(string guestPath, string hostPath);

      void CopyFolderToGuest(string source, string destination);

      void RunProgram(string fullPath, string parameters, bool throwOnFailure = false);
   }
}
