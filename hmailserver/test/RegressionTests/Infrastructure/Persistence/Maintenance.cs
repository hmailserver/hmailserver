// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System.Runtime.InteropServices;
using NUnit.Framework;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.Infrastructure.Persistence
{
   [TestFixture]
   public class Maintenance : TestFixtureBase
   {
      [Test]
      [ExpectedException(typeof (COMException), ExpectedMessage = "Unknown maintenance operation.")]
      public void TestUnknownOperation()
      {
         _application.Utilities.PerformMaintenance((eMaintenanceOperation) 234);
      }

      [Test]
      public void TestUpdateIMAPFolderUID()
      {
         // Set up a basic environment which we can work with.
         var backupRestore = new BackupRestore();
         backupRestore.SetUp();
         backupRestore.SetupEnvironment();

         _application.Utilities.PerformMaintenance(eMaintenanceOperation.eUpdateIMAPFolderUID);
      }
   }
}