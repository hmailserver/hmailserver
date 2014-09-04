using System;
using System.Collections.Generic;
using System.Text;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.Infrastructure
{
   [TestFixture]
   public class SessionLimitingTests : TestFixtureBase
   {
      [Test]
      public void TestLimitImapSessionCount()
      {
         _settings.MaxIMAPConnections = 2;

         using (var conn1 = new TcpConnection())
         using (var conn2 = new TcpConnection())
         using (var conn3 = new TcpConnection())
         {
            CustomAssert.IsTrue(conn1.Connect(143));
            string s1 = conn1.Receive();
            CustomAssert.IsNotEmpty(s1);
            CustomAssert.IsTrue(conn2.Connect(143));
            string s2 = conn2.Receive();
            CustomAssert.IsNotEmpty(s2);
            CustomAssert.IsTrue(conn3.Connect(143));
            string s3 = conn3.Receive();
            CustomAssert.IsEmpty(s3);
         }

      }

      [Test]
      public void TestLimitSMTPSessionCount()
      {
         _settings.MaxSMTPConnections = 2;

         using (var conn1 = new TcpConnection())
         using (var conn2 = new TcpConnection())
         using (var conn3 = new TcpConnection())
         {
            CustomAssert.IsTrue(conn1.Connect(25));
            string s1 = conn1.Receive();
            CustomAssert.IsNotEmpty(s1);
            CustomAssert.IsTrue(conn2.Connect(25));
            string s2 = conn2.Receive();
            CustomAssert.IsNotEmpty(s2);
            CustomAssert.IsTrue(conn3.Connect(25));
            string s3 = conn3.Receive();
            CustomAssert.IsEmpty(s3);
         }
      }

      [Test]
      public void TestLimitPOP3SessionCount()
      {
         _settings.MaxPOP3Connections = 2;

         using (var conn1 = new TcpConnection())
         using (var conn2 = new TcpConnection())
         using (var conn3 = new TcpConnection())
         {
            CustomAssert.IsTrue(conn1.Connect(110));
            string s1 = conn1.Receive();
            CustomAssert.IsNotEmpty(s1);
            CustomAssert.IsTrue(conn2.Connect(110));
            string s2 = conn2.Receive();
            CustomAssert.IsNotEmpty(s2);
            CustomAssert.IsTrue(conn3.Connect(110));
            string s3 = conn3.Receive();
            CustomAssert.IsEmpty(s3);
         }
      }

      [Test]
      public void TestConnectingToSmtpServerIncreasesSmtpSessionCount()
      {
         var app = SingletonProvider<TestSetup>.Instance.GetApp();
         var status = app.Status;
         int countBefore = status.get_SessionCount(eSessionType.eSTSMTP);

         using (var conn1 = new TcpConnection())
         {
            CustomAssert.IsTrue(conn1.Connect(25));

            TestSetup.AssertSessionCount(eSessionType.eSTSMTP,countBefore+1);
         }

      }

      [Test]
      public void TestConnectingToImapServerIncreasesImapSessionCount()
      {
         var app = SingletonProvider<TestSetup>.Instance.GetApp();
         var status = app.Status;
         int countBefore = status.get_SessionCount(eSessionType.eSTIMAP);

         using (var conn1 = new TcpConnection())
         {
            CustomAssert.IsTrue(conn1.Connect(143));
            TestSetup.AssertSessionCount(eSessionType.eSTIMAP, countBefore + 1);
         }

      }

      [Test]
      public void TestConnectingToPop3ServerIncreasesPop3SessionCount()
      {
         var app = SingletonProvider<TestSetup>.Instance.GetApp();
         var status = app.Status;
         int countBefore = status.get_SessionCount(eSessionType.eSTPOP3);

         using (var conn1 = new TcpConnection())
         {
            CustomAssert.IsTrue(conn1.Connect(110));
            TestSetup.AssertSessionCount(eSessionType.eSTPOP3, countBefore + 1);
         }

      }


      [Test]
      public void TestDisconnectingFromSmtpServerDecreasesPSmtpSessionCount()
      {
         var app = SingletonProvider<TestSetup>.Instance.GetApp();
         var status = app.Status;

         int countBefore;
         using (var conn1 = new TcpConnection())
         {
            CustomAssert.IsTrue(conn1.Connect(25));
            countBefore = status.get_SessionCount(eSessionType.eSTSMTP);
         }

         TestSetup.AssertSessionCount(eSessionType.eSTSMTP, countBefore -1);
      }

      [Test]
      public void TestDisconnectingFromImapServerDecreasesImapSessionCount()
      {
         var app = SingletonProvider<TestSetup>.Instance.GetApp();
         var status = app.Status;

         int countBefore;
         using (var conn1 = new TcpConnection())
         {
            CustomAssert.IsTrue(conn1.Connect(143));
            countBefore = status.get_SessionCount(eSessionType.eSTIMAP);
         }

         TestSetup.AssertSessionCount(eSessionType.eSTIMAP, countBefore - 1);

      }

      [Test]
      public void TestDisconnectingFromPop3ServerDecreasesPop3SessionCount()
      {
         var app = SingletonProvider<TestSetup>.Instance.GetApp();
         var status = app.Status;

         int countBefore;
         using (var conn1 = new TcpConnection())
         {
            CustomAssert.IsTrue(conn1.Connect(110));
            countBefore = status.get_SessionCount(eSessionType.eSTPOP3);
         }

         TestSetup.AssertSessionCount(eSessionType.eSTPOP3, countBefore - 1);


      }

   }
}
