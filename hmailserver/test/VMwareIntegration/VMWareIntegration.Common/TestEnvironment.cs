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
      private List<PostInstallCommand> _postInstallCommands;
      private List<PostInstallFileCopy> _postInstallFileCopy;

      public TestEnvironment(string operatingSystem, string description, string databaseType, string vmwarePath, string snapshotName)
      {
         _vmwarePath = vmwarePath;
         _snapshotName = snapshotName;
         _operatingSystem = operatingSystem;
         _description = description;
         _databaseType = databaseType;
         _postInstallCommands = new List<PostInstallCommand>();
         _postInstallFileCopy = new List<PostInstallFileCopy>();
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

      public List<PostInstallCommand> PostInstallCommands
      {
         get
         {
            return _postInstallCommands;
         }
      }

      public List<PostInstallFileCopy> PostInstallFileCopy
      {
         get
         {
            return _postInstallFileCopy;
         }
      }
   }
}
