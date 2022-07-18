// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using VixCOM;

namespace VMwareIntegration.Common
{
   class VMware
   {
      VixCOM.VixLibClass lib;
      VixCOM.IVM _virtualMachine;
      VixCOM.IHost _vmwareHost;

      public VMware()
      {
         lib = new VixCOM.VixLibClass();
         _virtualMachine = null;
      }

      public void Connect()
      {
         object results = null;
         VixCOM.IJob job = lib.Connect(VixCOM.Constants.VIX_API_VERSION, VixCOM.Constants.VIX_SERVICEPROVIDER_VMWARE_WORKSTATION, "", 0, "", "", 0, null, null);
         object[] data = { VixCOM.Constants.VIX_PROPERTY_JOB_RESULT_HANDLE };
         UInt64 err = job.Wait(new int[] { VixCOM.Constants.VIX_PROPERTY_JOB_RESULT_HANDLE }, ref results);
         if (lib.ErrorIndicatesFailure(err))
         {
            short errCode = lib.ErrorCode(err);
            string errMsg;
            errMsg = lib.GetErrorText(err, null);

             throw new Exception("Connect: "  + errMsg);
         }

         _vmwareHost = (VixCOM.IHost)((object[])results)[0];
      }

      public void OpenVM(string path)
      {
         object results = null;

         VixCOM.IJob job = _vmwareHost.OpenVM(path, null);
         UInt64 err = job.Wait(new int[] { VixCOM.Constants.VIX_PROPERTY_JOB_RESULT_HANDLE }, ref results);
         if (lib.ErrorIndicatesFailure(err))
         {
            short errCode = lib.ErrorCode(err);
            string errMsg;
            errMsg = lib.GetErrorText(err, null);

            throw new Exception("OpenVM:"  + errMsg);
         }

         _virtualMachine = (VixCOM.IVM)((object[])results)[0];
      }

      public void RevertToSnapshot(string snapshotName)
      {
         VixCOM.ISnapshot snapshot;
         _virtualMachine.GetNamedSnapshot(snapshotName, out snapshot);

         VixCOM.IJob job = _virtualMachine.RevertToSnapshot(snapshot, 0, null, null);
         UInt64 err = job.WaitWithoutResults();
         if (lib.ErrorIndicatesFailure(err))
         {
            short errCode = lib.ErrorCode(err);
            string errMsg;
            errMsg = lib.GetErrorText(err, null);

             throw new Exception("RevertToSnapshot: " + errMsg);
         }

         WaitForToolsInGuest();
      }

      public void PowerOn()
      {
         VixCOM.IJob job = _virtualMachine.PowerOn(0, null, null);
         UInt64 err = job.WaitWithoutResults();
         if (lib.ErrorIndicatesFailure(err))
         {
            short errCode = lib.ErrorCode(err);
            string errMsg;
            errMsg = lib.GetErrorText(err, null);

            throw new Exception("PowerOn: " + errMsg);
         }

         WaitForToolsInGuest();
      }

      private void WaitForToolsInGuest()
      {
          VixCOM.IJob job = _virtualMachine.WaitForToolsInGuest(0, null);
          UInt64 err = job.WaitWithoutResults();
          if (lib.ErrorIndicatesFailure(err))
          {
              short errCode = lib.ErrorCode(err);
              string errMsg;
              errMsg = lib.GetErrorText(err, null);

              throw new Exception("WaitForToolsInGuest: " + errMsg);
          }
      }

      public void PowerOff()
      {
         VixCOM.IJob job = _virtualMachine.PowerOff(0, null);
         UInt64 err = job.WaitWithoutResults();
         if (lib.ErrorIndicatesFailure(err))
         {
            short errCode = lib.ErrorCode(err);
            string errMsg;
            errMsg = lib.GetErrorText(err, null);

            throw new Exception("PowerOff: "+ errMsg);
         }

         
       
      }

      public void CopyFileToHost(string source, string destination)
      {
         // Console.WriteLine(string.Format("Copying file {0} to host...", source));
         VixCOM.IJob job = _virtualMachine.CopyFileFromGuestToHost(source, destination, 0, null, null);
         UInt64 err = job.WaitWithoutResults();

         if (lib.ErrorIndicatesFailure(err))
         {
            short errCode = lib.ErrorCode(err);
            string errMsg;
            errMsg = lib.GetErrorText(err, null);

            throw new Exception("CopyFileToHost: " + errMsg);
         }
      }


      public void LoginInGuest(string username, string password)
      {

         VixCOM.IJob job = _virtualMachine.LoginInGuest(username, password, 0, null);
         UInt64 err = job.WaitWithoutResults();

         if (lib.ErrorIndicatesFailure(err))
         {
            short errCode = lib.ErrorCode(err);
            string errMsg;
            errMsg = lib.GetErrorText(err, null);

            throw new Exception("LoginInGuest: "  + errMsg);
         }
      }


      public void CopyFileToGuest(string source, string destination)
      {
         //Console.WriteLine(string.Format("Copying file {0} to guest...", source));

         if (!File.Exists(source))
            throw new Exception("CopyFileToGuest: The source file " + source + " does not exist.");

         VixCOM.IJob job = _virtualMachine.CopyFileFromHostToGuest(source, destination, 0, null, null);
         UInt64 err = job.WaitWithoutResults();

         if (lib.ErrorIndicatesFailure(err))
         {
            short errCode = lib.ErrorCode(err);
            string errMsg;
            errMsg = lib.GetErrorText(err, null);

            throw new Exception("CopyFileToGuest: " + errMsg);
         }
      }

      public void CopyFolderToGuest(string source, string destination)
      {
         if (!Directory.Exists(source))
            throw new Exception("CopyFolderToGuest: The source directory " + source + " does not exist.");
         
         string[] files = Directory.GetFiles(source);
         if (!files.Any())
            throw new Exception("There are o files in the directory " + source);

         try
         {
            // create the dir first.
            CreateDirectory(destination);
         }
         catch (Exception)
         {

         }

         foreach (string fileName in files)
         {
            FileInfo fileInfo = new FileInfo(fileName);

            string sourceFile = fileInfo.FullName;
            string destinationFile = destination + "\\" + fileInfo.Name;

            CopyFileToGuest(sourceFile, destinationFile);
         }
      }

      public void RunProgramInGuest(string fullPath, string param)
      {
         VixCOM.IJob job = _virtualMachine.RunProgramInGuest(fullPath, param, 0, null, null);
         UInt64 err = job.WaitWithoutResults();
         
         if (lib.ErrorIndicatesFailure(err))
         {
            short errCode = lib.ErrorCode(err);
            string errMsg;
            errMsg = lib.GetErrorText(err, null);

            throw new Exception("RunProgramInGuest: " + errMsg);
         }
      }

      public void CreateDirectory(string name)
      {
         VixCOM.IJob job = _virtualMachine.CreateDirectoryInGuest(name, null, null);
         UInt64 err = job.WaitWithoutResults();

         if (lib.ErrorIndicatesFailure(err))
         {
            short errCode = lib.ErrorCode(err);
            string errMsg;
            errMsg = lib.GetErrorText(err, null);

            throw new Exception("CreateDirectory: " + errMsg);
         }
      }

   }
}
