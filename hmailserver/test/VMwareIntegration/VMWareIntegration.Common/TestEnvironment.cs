// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;

namespace VMwareIntegration.Common
{
   public class TestEnvironment
   {
      private string _vmwarePath;
      private string _snapshotName;
      private string _operatingSystem;
      private string _description;
      private string _databaseType;
      private List<InstallCommand> _postInstallCommands;
      private List<FileCopyCommand> _postInstallFileCopy;
      private List<FileCopyCommand> _preInstallFileCopy;
      private List<InstallCommand> _preInstallCommands;
      
      public TestEnvironment(string operatingSystem, string description, string databaseType, string vmwarePath, string snapshotName)
      {
         _vmwarePath = vmwarePath;
         _snapshotName = snapshotName;
         _operatingSystem = operatingSystem;
         _description = description;
         _databaseType = databaseType;
         _postInstallCommands = new List<InstallCommand>();
         _preInstallFileCopy = new List<FileCopyCommand>();
         _postInstallFileCopy = new List<FileCopyCommand>();
         _preInstallCommands = new List<InstallCommand>();
      }

      public string OperatingSystem
      {
         get { return _operatingSystem;  }
      }

      public string Description
      {
         get { return _description; }
      }

      public string SnapshotName
      {
         get { return _snapshotName;  }
      }


      public string VMwarePath
      {
         get { return _vmwarePath; }
      }

      public string DatabaseType
      {
         get { return _databaseType; }
      }

      public List<InstallCommand> PostInstallCommands
      {
         get
         {
            return _postInstallCommands;
         }
      }

      public List<FileCopyCommand> PostInstallFileCopy
      {
         get
         {
            return _postInstallFileCopy;
         }
      }

      public List<FileCopyCommand> PreInstallFileCopy
      {
         get
         {
            return _preInstallFileCopy;
         }
      }

      public List<InstallCommand> PreInstallCommands
      {
         get
         {
            return _preInstallCommands;
         }
      }
   }
}
