using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Net.Mail;
using System.Text;
using System.Threading.Tasks;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;

namespace hMailServer.PerformanceTests
{
   [TestFixture]
   public class AverageMailSending : TestFixtureBase
   {
      private hMailServer.Account _account;

      [SetUp]
      public new void SetUp()
      {
         _account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
      }

      [Test]
      public void Send1000MessagesOn5Threads()
      {
         SendMessagesInParallell(5, 1000);
      }

      [Test]
      public void Send1000MessagesOn10Threads()
      {
         SendMessagesInParallell(10, 1000);
      }

      [Test]
      public void Send10000MessagesOn5Threads()
      {
         SendMessagesInParallell(5, 10000);
      }


      private void SendMessagesInParallell(int threadCount, int messagesPerThread)
      {
         var actions = new List<Action>();

         for (int i = 0; i < threadCount; i++)
         {
            actions.Add(new Action(() => SendMessages(messagesPerThread)));
         }

         var parallellOptions = new ParallelOptions()
         {
            MaxDegreeOfParallelism = actions.Count
         };

         MeasureTime(
            () =>
            {
               Parallel.Invoke(parallellOptions, actions.ToArray());

               WaitForMessageCount(TimeSpan.FromMinutes(5), threadCount*messagesPerThread);
            });
      }

      private void WaitForMessageCount(TimeSpan timeout, int expectedMessageCount)
      {
         RetryHelper.TryAction(timeout, () =>
            {
               var pop3ClientSimulator = new POP3ClientSimulator();
               int count = pop3ClientSimulator.GetMessageCount("test@test.com", "test");

               Assert.AreEqual(expectedMessageCount, count);
            });
      }

      private MailMessage CreateMailMessage()
      {  
         // body is 75kb, which according to some represents the average email size.
         
         var body = new StringBuilder();

         for (int i = 0; i < 1024; i++)
         {
            body.AppendLine(new string('a', 75));
         }

         var mailMessage = new MailMessage()
            {
               From = new MailAddress("test@test.com"),               
               Subject = Guid.NewGuid().ToString(),
               Body = body.ToString()
            };

         mailMessage.To.Add(new MailAddress("test@test.com"));

         return mailMessage;
      }

      private void SendMessages(int count)
      {
         var mailMessage = CreateMailMessage();
         for (int i = 0; i < count; i++)
         {
            using (var client = new SmtpClient())
            {
               // send mail synchronously.
               client.Host = "127.0.0.1";
               client.SendMailAsync(mailMessage).Wait();
            }

         }
      }

      private void MeasureTime(Action action)
      {
         // stopwatch isn't exact, but exact enough for this type of measurement.
         var stopwatch = new Stopwatch();
         stopwatch.Start();

         action();

         stopwatch.Stop();

         Console.WriteLine("{0};{1};{2};{3}", _application.Version, TestContext.CurrentContext.Test.FullName, DateTime.UtcNow.ToString("yyyyMMdd HH:mm:ss"), stopwatch.Elapsed);
      }

   }
}
