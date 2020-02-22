// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System.Text;
using NUnit.Framework;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.IMAP
{
   [TestFixture]
   public class CommandSequences : TestFixtureBase
   {
      [Test]
      public void TestBatchOfCommands()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "batch@test.com", "test");

         var oSimulator = new ImapClientSimulator();
         string sWelcomeMessage = oSimulator.Connect();
         oSimulator.Logon(account.Address, "test");

         string commandSequence = "";
         for (int i = 0; i < 200; i++)
         {
            commandSequence += "A" + i.ToString() + " SELECT INBOX\r\n";
         }
         commandSequence = commandSequence.TrimEnd("\r\n".ToCharArray());

         string result = oSimulator.Send(commandSequence);
         Assert.IsFalse(result.StartsWith("* BYE"));

         oSimulator.Disconnect();

         sWelcomeMessage = oSimulator.Connect();
         oSimulator.Logon(account.Address, "test");
         commandSequence = "";
         for (int i = 0; i < 500; i++)
         {
            commandSequence += "A" + i.ToString() + " SELECT INBOX\r\n";
         }
         commandSequence = commandSequence.TrimEnd("\r\n".ToCharArray());

         result = oSimulator.Send(commandSequence);
         Assert.IsFalse(result.StartsWith("* BYE Excessive number of buffered commands"));
         oSimulator.Disconnect();
      }

      [Test]
      public void TestLongCommand()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "batch@test.com", "test");

         var oSimulator = new ImapClientSimulator();
         string sWelcomeMessage = oSimulator.Connect();
         oSimulator.Logon(account.Address, "test");

         var sb = new StringBuilder();

         for (int i = 0; i < 240000; i++)
         {
            sb.Append("A");
         }

         string result = oSimulator.Send("A01 " + sb);
         Assert.IsTrue(result.Length == 0 || result.StartsWith("A01"));
      }
   }
}