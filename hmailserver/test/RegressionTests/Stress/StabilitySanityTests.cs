// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Diagnostics;
using System.IO;
using System.Text;
using NUnit.Framework;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.Stress
{
   [TestFixture]
   public class SanityTests : TestFixtureBase
   {
      [Test]
      public void TestDeletionOfMessageInDeletedFolder()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();
         string deletedMessageText = _settings.ServerMessages.get_ItemByName("MESSAGE_FILE_MISSING").Text;
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         SMTPClientSimulator.StaticSend(account.Address, account.Address, "Test", "SampleBody");
         IMAPFolder inbox = account.IMAPFolders.get_ItemByName("Inbox");

         TestSetup.AssertFolderMessageCount(inbox, 1);

         Messages messages = inbox.Messages;

         Message message = messages[0];
         var dir = new DirectoryInfo(Path.GetFullPath(message.Filename));
         DirectoryInfo parent = dir.Parent.Parent.Parent;
         parent.Delete(true);

         DateTime timeBeforeDelete = DateTime.Now;
         messages.DeleteByDBID(message.ID);

         TimeSpan executionTime = DateTime.Now - timeBeforeDelete;

         Assert.Greater(1500, executionTime.TotalMilliseconds);
      }

      [Test]
      public void TestInsertionOfTooLongString()
      {
         var watch = new Stopwatch();

         Application application = SingletonProvider<TestSetup>.Instance.GetApp();
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         var sb = new StringBuilder();
         for (int i = 0; i < 1000; i++)
            sb.Append("abcdefgh");

         account.PersonFirstName = sb.ToString();

         try
         {
            watch.Start();
            account.Save();
         }
         catch (Exception)
         {
         }

         watch.Stop();
         Assert.Greater(10000, watch.ElapsedMilliseconds);

         // an error log file may have been created. if we're using MySQL,
         // the value may have been silently truncated.
         TestSetup.AssertDeleteFile(TestSetup.GetErrorLogFileName());
      }

      [Test]
      [Description("Confirms that hMailServer behaves properly if a specific port is in use.")]
      public void TestPortInUse()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();
         application.Stop();

         var sock = new TcpSocket();
         using (var serverSocket = new TcpServer(1, 25))
         {
            serverSocket.StartListen();

            application.Start();
            
            // make sure it's possible to connect to the non blocked port.

            sock.CanConnect(110);
            sock.CanConnect(143);

            //let this our temp server die.
            sock.CanConnect(25);

            // make sure that hMailServer reported an error during start up because the ports were blocked.
            TestSetup.AssertReportedError();
         }

         // restart hMailServer again. everything is now back to normal.
         application.Stop();

         application.Start();
         sock.CanConnect(25);
      }

      [Test]
      public void TestRetrievalOfDeletedMessage()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();
         string deletedMessageText = _settings.ServerMessages.get_ItemByName("MESSAGE_FILE_MISSING").Text;

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         SMTPClientSimulator.StaticSend(account.Address, account.Address, "Test", "SampleBody");

         IMAPFolder inbox = account.IMAPFolders.get_ItemByName("Inbox");


         TestSetup.AssertFolderMessageCount(inbox, 1);

         Message message = inbox.Messages[0];

         File.Delete(message.Filename);

         string text = POP3Simulator.AssertGetFirstMessageText(account.Address, "test");
         Assert.IsTrue(text.Contains(deletedMessageText.Replace("%MACRO_FILE%", message.Filename)));

         TestSetup.AssertReportedError();
      }

      [Test]
      public void TestRetrievalOfMessageInDeletedFolder()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();
         string deletedMessageText = _settings.ServerMessages.get_ItemByName("MESSAGE_FILE_MISSING").Text;

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         SMTPClientSimulator.StaticSend(account.Address, account.Address, "Test", "SampleBody");

         IMAPFolder inbox = account.IMAPFolders.get_ItemByName("Inbox");


         TestSetup.AssertFolderMessageCount(inbox, 1);

         Message message = inbox.Messages[0];

         var dir = new DirectoryInfo(Path.GetFullPath(message.Filename));
         DirectoryInfo parent = dir.Parent.Parent.Parent;
         parent.Delete(true);

         string text = POP3Simulator.AssertGetFirstMessageText(account.Address, "test");
         Assert.IsTrue(text.Contains(deletedMessageText.Replace("%MACRO_FILE%", message.Filename)));
         TestSetup.AssertReportedError();
      }


      [Test]
      public void TestRetrievalOfMessageInDeletedFolderUsingIMAP()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();
         string deletedMessageText = _settings.ServerMessages.get_ItemByName("MESSAGE_FILE_MISSING").Text;

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         SMTPClientSimulator.StaticSend(account.Address, account.Address, "Test", "SampleBody");

         IMAPFolder inbox = account.IMAPFolders.get_ItemByName("Inbox");


         TestSetup.AssertFolderMessageCount(inbox, 1);

         Message message = inbox.Messages[0];

         var dir = new DirectoryInfo(Path.GetFullPath(message.Filename));
         DirectoryInfo parent = dir.Parent.Parent.Parent;
         parent.Delete(true);

         var sim = new IMAPSimulator();
         sim.ConnectAndLogon(account.Address, "test");
         sim.SelectFolder("INBOX");
         string result = sim.Fetch("1 BODY[1]");

         Assert.IsTrue(result.Contains(deletedMessageText.Replace("%MACRO_FILE%", message.Filename)));
         TestSetup.AssertReportedError();
      }
   }
}