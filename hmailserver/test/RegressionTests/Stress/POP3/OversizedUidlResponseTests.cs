// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

using hMailServer;
using NUnit.Framework;
using RegressionTests.POP3;
using RegressionTests.Shared;

namespace RegressionTests.Stress.POP3
{
   /// <summary>
   ///    UIDL responses that only exceed a limit after a large amount of data has been sent.
   /// </summary>
   [TestFixture]
   [Explicit]
   [Category("Stress")]
   public class OversizedUidlResponseTests : TestFixtureBase
   {
      [SetUp]
      public void SetUpTest()
      {
         _account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@example.test", "test");
         _serverPort = TestSetup.GetNextFreePort();
      }

      private Account _account;
      private int _serverPort;

      [TestCase(Pop3ServerSimulator.OversizedResponseType.UidlEntries)]
      [TestCase(Pop3ServerSimulator.OversizedResponseType.UidlTotalSize)]
      [Description("hMailServer should disconnect when the UIDL response has too many entries or is too large in total.")]
      public void TestOversizedUidlResponseShouldDisconnect(Pop3ServerSimulator.OversizedResponseType responseType)
      {
         ServerBehaviors.AssertOversizedResponseDisconnects(_account, _serverPort, responseType, "UIDL");
      }
   }
}
