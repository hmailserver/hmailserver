using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using NUnit.Framework;
using RegressionTests.Shared;

namespace StressTest
{
   [TestFixture]
   public class IMAP : TestFixtureBase
   {
      [SetUp]
      public new void SetUp()
      {
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
      }


      [Test]
      public void TestLongIMAPCommand()
      {
         ImapClientSimulator sim = ConnectAndLogon();

         // build a large string.
         StringBuilder sb = new StringBuilder();
         sb.Append("A01");
         for (int i = 0; i < 1000000; i++)
         {
            sb.Append("01234567890");
         }
         int length = sb.Length;

         sb.Append(Environment.NewLine);

         string result = sim.Send(sb.ToString());
         Assert.IsTrue(result.StartsWith("* BYE Excessive amount of data sent to server."));

         sim.Disconnect();
      }

      
      private static ImapClientSimulator ConnectAndLogon()
      {
         ImapClientSimulator sim = new ImapClientSimulator();
         sim.ConnectAndLogon("test@test.com", "test");
         return sim;
      }

   }
}
