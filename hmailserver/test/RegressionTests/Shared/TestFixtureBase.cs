// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using NUnit.Framework;
using hMailServer;

namespace RegressionTests.Shared
{
   public class TestFixtureBase
   {
      protected Application _application;
      protected Domain _domain;
      protected Settings _settings;

      [TestFixtureSetUp]
      public void TestFixtureSetUp()
      {
         SingletonProvider<TestSetup>.Instance.Authenticate();

         _application = SingletonProvider<TestSetup>.Instance.GetApp();
         _settings = _application.Settings;
      }

      [SetUp]
      public void SetUp()
      {
         _domain = SingletonProvider<TestSetup>.Instance.DoBasicSetup();

         TestSetup.DeleteCurrentDefaultLog();
      }
   }
}