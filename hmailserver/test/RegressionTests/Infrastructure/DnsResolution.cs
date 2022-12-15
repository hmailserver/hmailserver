using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.Infrastructure
{
   [TestFixture]
   public class DnsResolution : TestFixtureBase
   {
      private hMailServer.Utilities _utilities;

      [OneTimeSetUp]
      public void OneTimeSetUp()
      {
         var application = SingletonProvider<TestSetup>.Instance.GetApp();
         _utilities = application.Utilities;
      }

      [Test]
      public void CNameRecordsShouldBeFollowed()
      {
         // If a MX record contains a CNAME record, the CNAME record should be followed.
         // According to RFC, a server owner should not add a CNAME record to MX record,
         // but many do and hMailServer has supported this historically.
         var actualServer = _utilities.GetMailServer("example@cname-test.hmailserver.com");
         var expectedServer= _utilities.GetMailServer("example@hmailserver.com");

         Assert.AreEqual(expectedServer, actualServer);
      }

      [Test]
      public void NoneExistantRecordsShouldNotResolve()
      {
         // If a MX record contains a CNAME record, the CNAME record should be followed.
         // According to RFC, a server owner should not add a CNAME record to MX record,
         // but many do and hMailServer has supported this historically.
         var actualServer = _utilities.GetMailServer("example@invalid.hmailserver.com");

         Assert.AreEqual("", actualServer);
      }

      [Test]
      public void InfinitelyRecursiveRecordsShouldNotResolve()
      {
         // If a MX record contains a CNAME record, the CNAME record should be followed.
         // According to RFC, a server owner should not add a CNAME record to MX record,
         // but many do and hMailServer has supported this historically.
         var actualServer = _utilities.GetMailServer("example@recursive1.hmailserver.com");

         Assert.AreEqual("", actualServer);

         var errorLog = LogHandler.ReadAndDeleteErrorLog();

         StringAssert.Contains("Too many recursions during IP address lookup. Query: recursive2.hmailserver.com", errorLog);
      }
   }
}
