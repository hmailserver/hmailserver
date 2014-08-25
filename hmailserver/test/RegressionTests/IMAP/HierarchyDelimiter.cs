// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using NUnit.Framework;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.IMAP
{
   [TestFixture]
   public class HierarchyDelimiter : TestFixtureBase
   {
      [Test]
      [Description("Test that the hierchary delimiter has effect on the Create command.")]
      public void TestHierarchyDelimiterCreate()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();
         Settings settings = _settings;
         settings.IMAPHierarchyDelimiter = "/";

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "quote@test.com", "test");

         string folderName = "Test.Test";

         var oSimulator = new IMAPClientSimulator();
         string sWelcomeMessage = oSimulator.Connect();
         oSimulator.Logon(account.Address, "test");
         CustomAssert.IsTrue(oSimulator.CreateFolder(folderName));
         string listResponse = oSimulator.List();
         CustomAssert.IsTrue(listResponse.Contains("\"" + folderName + "\""));
         CustomAssert.IsTrue(!listResponse.Contains("\"Test\""));
         oSimulator.Disconnect();
      }

      [Test]
      [Description("Test that the hierchary delimiter has effect on the Delete command.")]
      public void TestHierarchyDelimiterDelete()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();
         Settings settings = _settings;
         settings.IMAPHierarchyDelimiter = "\\";

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "quote@test.com", "test");

         string folderName = "Test\\Test";

         var oSimulator = new IMAPClientSimulator();
         string sWelcomeMessage = oSimulator.Connect();
         oSimulator.Logon(account.Address, "test");
         CustomAssert.IsTrue(oSimulator.CreateFolder(folderName));
         string listResponse = oSimulator.List();
         CustomAssert.IsTrue(listResponse.Contains("\"Test\\Test\""));
         CustomAssert.IsTrue(listResponse.Contains("\"Test\""));
         CustomAssert.IsTrue(oSimulator.DeleteFolder("Test\\Test"));
         listResponse = oSimulator.List();
         CustomAssert.IsFalse(listResponse.Contains("Test\\Test"));
         CustomAssert.IsTrue(listResponse.Contains("Test"));
         oSimulator.Disconnect();
      }

      [Test]
      [Description("Test that the hierchary delimiter has effect on the List response.")]
      public void TestHierarchyDelimiterListResponse()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();
         Settings settings = _settings;
         settings.IMAPHierarchyDelimiter = "\\";

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "quote@test.com", "test");

         string folderName = "Test\\Test";

         var oSimulator = new IMAPClientSimulator();
         string sWelcomeMessage = oSimulator.Connect();
         oSimulator.Logon(account.Address, "test");
         CustomAssert.IsTrue(oSimulator.CreateFolder(folderName));
         string listResponse = oSimulator.List();
         CustomAssert.IsTrue(listResponse.Contains("\"Test\\Test\""));
         CustomAssert.IsTrue(listResponse.Contains("\"Test\""));
         oSimulator.Disconnect();
      }

      [Test]
      [Description("Test that the hierchary delimiter has effect on the LSUB response.")]
      public void TestHierarchyDelimiterLsubResponse()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();
         Settings settings = _settings;
         settings.IMAPHierarchyDelimiter = "/";

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "quote@test.com", "test");

         string folderName = "Test/Test";

         var oSimulator = new IMAPClientSimulator();
         string sWelcomeMessage = oSimulator.Connect();
         oSimulator.Logon(account.Address, "test");
         CustomAssert.IsTrue(oSimulator.CreateFolder(folderName));
         CustomAssert.IsTrue(oSimulator.Subscribe("Test"));
         CustomAssert.IsTrue(oSimulator.Subscribe("Test/Test"));
         string lsubResponse = oSimulator.LSUB();
         CustomAssert.IsTrue(lsubResponse.Contains("\"Test/Test\""));
         CustomAssert.IsTrue(lsubResponse.Contains("\"Test\""));
         oSimulator.Disconnect();
      }

      [Test]
      [Description("Test that the hierchary delimiter has effect on the Namespace response.")]
      public void TestHierarchyDelimiterNamespaceResponse()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();
         Settings settings = _settings;
         settings.IMAPHierarchyDelimiter = "\\";
         string publicFolderName = _settings.IMAPPublicFolderName;

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "quote@test.com", "test");

         var oSimulator = new IMAPClientSimulator();
         oSimulator.ConnectAndLogon(account.Address, "test");
         string result = oSimulator.Send("A01 NAMESPACE");
         string correctNamespaceSetting = "* NAMESPACE ((\"\" \"\\\\\")) NIL ((\"" + publicFolderName +
                                          "\" \"\\\\\"))";
         CustomAssert.IsTrue(result.Contains(correctNamespaceSetting), result);
         oSimulator.Disconnect();

         settings.IMAPHierarchyDelimiter = ".";

         oSimulator = new IMAPClientSimulator();
         oSimulator.ConnectAndLogon(account.Address, "test");

         result = oSimulator.Send("A01 NAMESPACE");
         correctNamespaceSetting = "* NAMESPACE ((\"\" \".\")) NIL ((\"" + publicFolderName + "\" \".\"))";
         CustomAssert.IsTrue(result.Contains(correctNamespaceSetting), result);
         oSimulator.Disconnect();

         settings.IMAPHierarchyDelimiter = "/";

         oSimulator = new IMAPClientSimulator();
         oSimulator.ConnectAndLogon(account.Address, "test");

         result = oSimulator.Send("A01 NAMESPACE");
         correctNamespaceSetting = "* NAMESPACE ((\"\" \"/\")) NIL ((\"" + publicFolderName + "\" \"/\"))";
         CustomAssert.IsTrue(result.Contains(correctNamespaceSetting), result);
         oSimulator.Disconnect();
      }

      [Test]
      [Description("Test that the hierchary delimiter has effect on the Rename command.")]
      public void TestHierarchyDelimiterRename()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();
         Settings settings = _settings;
         settings.IMAPHierarchyDelimiter = "/";

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "quote@test.com", "test");

         string folderName = "Test/Test";
         string newFolderName = "Apa/Test";

         var oSimulator = new IMAPClientSimulator();
         string sWelcomeMessage = oSimulator.Connect();
         oSimulator.Logon(account.Address, "test");
         CustomAssert.IsTrue(oSimulator.CreateFolder(folderName));
         string listResponse = oSimulator.List();
         CustomAssert.IsTrue(listResponse.Contains("\"" + folderName + "\""));
         CustomAssert.IsTrue(listResponse.Contains("\"Test\""));
         CustomAssert.IsTrue(oSimulator.RenameFolder("Test", "Apa"));
         listResponse = oSimulator.List();
         CustomAssert.IsTrue(listResponse.Contains("\"" + newFolderName + "\""));
         CustomAssert.IsTrue(listResponse.Contains("\"Apa\""));
         oSimulator.Disconnect();
      }

      [Test]
      [Description("Test that the hierchary delimiter has effect on the LSUB response.")]
      public void TestHierarchyDelimiterSlash()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();
         Settings settings = _settings;
         settings.IMAPHierarchyDelimiter = "\\";

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "quote@test.com", "test");

         string folderName = "Test\\Test";

         var oSimulator = new IMAPClientSimulator();
         string sWelcomeMessage = oSimulator.Connect();
         oSimulator.Logon(account.Address, "test");
         CustomAssert.IsTrue(oSimulator.CreateFolder(folderName));
         CustomAssert.IsTrue(oSimulator.Subscribe("Test"));
         CustomAssert.IsTrue(oSimulator.Subscribe("Test\\Test"));
         string lsubResponse = oSimulator.LSUB();
         CustomAssert.IsTrue(lsubResponse.Contains("\"Test\\Test\""));
         CustomAssert.IsTrue(lsubResponse.Contains("\"Test\""));
         oSimulator.Disconnect();
      }
   }
}