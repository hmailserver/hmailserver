using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Mail;
using System.Text;
using NUnit.Framework;
using RegressionTests.Shared;

namespace StressTest
{
   [TestFixture]
   public class LargeMessagesTest : TestFixtureBase
   {
      [SetUp]
      public new void SetUp()
      {
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
      }

      [Test]
      public void SendLargeMessage()
      {
         _settings.MaxMessageSize = 0;

         // create a 200mb attachment
         var largeFile = Path.Combine(Path.GetTempPath(), Guid.NewGuid().ToString());

         try
         {

            for (int i = 0; i < 10; i++)
            {
               var sb = new StringBuilder();
               for (int c = 0; c < 350000; c++)
                  sb.Append("012345678901234567890123456789012345678901234567890123456789");

            
               File.AppendAllText(largeFile, sb.ToString());
            }

            using (var mail = new MailMessage())
            {
               mail.From = new MailAddress("test@test.com");
               mail.To.Add("test@test.com");
               mail.Subject = "Automatic server test";
               mail.Body = "Automatic server test";
               mail.BodyEncoding = Encoding.GetEncoding(1252);
               mail.SubjectEncoding = Encoding.GetEncoding(1252);
               mail.Attachments.Add(new Attachment(largeFile));

               var oClient = new SmtpClient("localhost", 25);
               {
                  oClient.Send(mail);
               }
            }

         }
         finally
         {
            if (File.Exists(largeFile))
               File.Delete(largeFile);
         }

      }

   }
}
