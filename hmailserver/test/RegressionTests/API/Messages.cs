// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.IO;
using System.Net.Mail;
using System.Reflection;
using System.Text;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;
using hMailServer;
using Attachment = System.Net.Mail.Attachment;

namespace RegressionTests.API
{
   [TestFixture]
   public class Messages : TestFixtureBase
   {
      [Test]
      public void DeletingSingleMessageShouldNotAffectOtherMessages()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         // Add 3 messages to the inbox
         var imapClientSimulator = new ImapClientSimulator();
         imapClientSimulator.ConnectAndLogon("test@test.com", "test");
         for (int i = 0; i < 3; i++)
            imapClientSimulator.SendSingleCommandWithLiteral("A01 APPEND INBOX {4}", "ABCD");
         imapClientSimulator.Disconnect();

         var messages = account.Messages;

         var firstId = messages[0].ID;
         var secondId = messages[1].ID;
         var thirdId = messages[2].ID;

         messages.DeleteByDBID(secondId);
         
         Assert.AreEqual(2, messages.Count);
         Assert.AreEqual(firstId, messages[0].ID);
         Assert.AreEqual(thirdId, messages[1].ID);

         imapClientSimulator.ConnectAndLogon("test@test.com", "test");
         var imapMessageCount = imapClientSimulator.GetMessageCount("Inbox");
         Assert.AreEqual(imapMessageCount, messages.Count);

      }

      [Test]
      public void DeletingMultipleMessagesShouldSucceed()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         var imapClientSimulator = new ImapClientSimulator();
         imapClientSimulator.ConnectAndLogon("test@test.com", "test");
         for (int i = 0; i < 10; i++)
            imapClientSimulator.SendSingleCommandWithLiteral("A01 APPEND INBOX {4}", "ABCD");
         imapClientSimulator.Disconnect();

         var messages = account.Messages;

         var messagesToDelete = new List<long>();
         var messagesToSave = new List<long>();

         for (int i = 0; i < 10; i++)
         {
            if (i <= 3)
               messagesToDelete.Add(messages[i].ID);
            else
               messagesToSave.Add(messages[i].ID);
         }

         // Delete the 4 messages
         foreach (var messageToDelete in messagesToDelete)
            messages.DeleteByDBID(messageToDelete);

         // Should be 6 remaining
         Assert.AreEqual(6, messages.Count);

         // Validate that the 6 remaining are the expected ones
         foreach (var messageToSave in messagesToSave)
         {
            var message = messages.get_ItemByDBID(messageToSave);
            Assert.IsNotNull(message);
         }
      }

   }
}