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

         var simulator = new ImapClientSimulator();
         string sWelcomeMessage = simulator.Connect();
         simulator.Logon(account.Address, "test");
         Assert.IsTrue(simulator.CreateFolder(folderName));
         string listResponse = simulator.List();
         Assert.IsTrue(listResponse.Contains("\"" + folderName + "\""));
         Assert.IsTrue(!listResponse.Contains("\"Test\""));
         simulator.Disconnect();
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

         var simulator = new ImapClientSimulator();
         string sWelcomeMessage = simulator.Connect();
         simulator.Logon(account.Address, "test");
         Assert.IsTrue(simulator.CreateFolder(folderName));
         string listResponse = simulator.List();
         Assert.IsTrue(listResponse.Contains("\"Test\\Test\""));
         Assert.IsTrue(listResponse.Contains("\"Test\""));
         Assert.IsTrue(simulator.DeleteFolder("Test\\Test"));
         listResponse = simulator.List();
         Assert.IsFalse(listResponse.Contains("Test\\Test"));
         Assert.IsTrue(listResponse.Contains("Test"));
         simulator.Disconnect();
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

         var simulator = new ImapClientSimulator();
         string sWelcomeMessage = simulator.Connect();
         simulator.Logon(account.Address, "test");
         Assert.IsTrue(simulator.CreateFolder(folderName));
         string listResponse = simulator.List();
         Assert.IsTrue(listResponse.Contains("\"Test\\Test\""));
         Assert.IsTrue(listResponse.Contains("\"Test\""));
         simulator.Disconnect();
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

         var simulator = new ImapClientSimulator();
         string sWelcomeMessage = simulator.Connect();
         simulator.Logon(account.Address, "test");
         Assert.IsTrue(simulator.CreateFolder(folderName));
         Assert.IsTrue(simulator.Subscribe("Test"));
         Assert.IsTrue(simulator.Subscribe("Test/Test"));
         string lsubResponse = simulator.LSUB();
         Assert.IsTrue(lsubResponse.Contains("\"Test/Test\""));
         Assert.IsTrue(lsubResponse.Contains("\"Test\""));
         simulator.Disconnect();
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

         var simulator = new ImapClientSimulator();
         simulator.ConnectAndLogon(account.Address, "test");
         string result = simulator.Send("A01 NAMESPACE");
         string correctNamespaceSetting = "* NAMESPACE ((\"\" \"\\\\\")) NIL ((\"" + publicFolderName +
                                          "\" \"\\\\\"))";
         Assert.IsTrue(result.Contains(correctNamespaceSetting), result);
         simulator.Disconnect();

         settings.IMAPHierarchyDelimiter = ".";

         simulator = new ImapClientSimulator();
         simulator.ConnectAndLogon(account.Address, "test");

         result = simulator.Send("A01 NAMESPACE");
         correctNamespaceSetting = "* NAMESPACE ((\"\" \".\")) NIL ((\"" + publicFolderName + "\" \".\"))";
         Assert.IsTrue(result.Contains(correctNamespaceSetting), result);
         simulator.Disconnect();

         settings.IMAPHierarchyDelimiter = "/";

         simulator = new ImapClientSimulator();
         simulator.ConnectAndLogon(account.Address, "test");

         result = simulator.Send("A01 NAMESPACE");
         correctNamespaceSetting = "* NAMESPACE ((\"\" \"/\")) NIL ((\"" + publicFolderName + "\" \"/\"))";
         Assert.IsTrue(result.Contains(correctNamespaceSetting), result);
         simulator.Disconnect();
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

         var simulator = new ImapClientSimulator();
         string sWelcomeMessage = simulator.Connect();
         simulator.Logon(account.Address, "test");
         Assert.IsTrue(simulator.CreateFolder(folderName));
         string listResponse = simulator.List();
         Assert.IsTrue(listResponse.Contains("\"" + folderName + "\""));
         Assert.IsTrue(listResponse.Contains("\"Test\""));
         Assert.IsTrue(simulator.RenameFolder("Test", "Apa"));
         listResponse = simulator.List();
         Assert.IsTrue(listResponse.Contains("\"" + newFolderName + "\""));
         Assert.IsTrue(listResponse.Contains("\"Apa\""));
         simulator.Disconnect();
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

         var simulator = new ImapClientSimulator();
         string sWelcomeMessage = simulator.Connect();
         simulator.Logon(account.Address, "test");
         Assert.IsTrue(simulator.CreateFolder(folderName));
         Assert.IsTrue(simulator.Subscribe("Test"));
         Assert.IsTrue(simulator.Subscribe("Test\\Test"));
         string lsubResponse = simulator.LSUB();
         Assert.IsTrue(lsubResponse.Contains("\"Test\\Test\""));
         Assert.IsTrue(lsubResponse.Contains("\"Test\""));
         simulator.Disconnect();
      }
   }
}