// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System.Diagnostics;

namespace Builder.Common
{
   internal class ProcessLauncher
   {
      public delegate void OutputDelegate(string output);

      public string _receivedData;
      public event OutputDelegate Output;

      public int LaunchProcess(string path, string arguments, string workingDirectory, out string writtenData)
      {
         _receivedData = "";

         var proc = new Process();
         proc.StartInfo.FileName = path;
         proc.StartInfo.WorkingDirectory = workingDirectory;
         proc.StartInfo.Arguments = arguments;
         proc.StartInfo.UseShellExecute = false;

         // set up output redirection
         proc.StartInfo.RedirectStandardOutput = true;
         proc.StartInfo.RedirectStandardError = true;
         proc.EnableRaisingEvents = true;
         proc.StartInfo.CreateNoWindow = true;
         // see below for output handler
         proc.ErrorDataReceived += proc_DataReceived;
         proc.OutputDataReceived += proc_DataReceived;

         proc.Start();

         proc.BeginErrorReadLine();
         proc.BeginOutputReadLine();

         proc.WaitForExit();

         writtenData = _receivedData;

         return proc.ExitCode;
      }

      private void proc_DataReceived(object sender, DataReceivedEventArgs e)
      {
         if (e.Data == null)
            return;

         if (Output != null)
            Output(e.Data);

         _receivedData += e.Data;
      }
   }
}