using System.Runtime.InteropServices;
using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.Infrastructure.Persistence
{
   [TestFixture]
   public class DomainNameValidation : TestFixtureBase
   {
      [Test]
      public void TestDomainNameContainingSpace()
      {
         AssertInvalidDomainName("example 1.com");
      }

      [Test]
      public void TestDomainNameContainigSlash()
      {
         AssertInvalidDomainName("example\\1.com");
      }

      [Test]
      public void TestDomainWithoutName()
      {
         AssertInvalidDomainName("");
      }

      [Test]
      public void TestSimpleDomain()
      {
         AssertValidDomainName("example.com");
      }

      [Test]
      public void TestDomainWithDot()
      {
         AssertValidDomainName("examp.co.uk");
      }

      [Test]
      public void TestDomainWithIPv4Literal()
      {
         AssertValidDomainName("[192.168.1.1]");
      }

      [Test]
      public void TestDomainWithIPv6Literal()
      {
         AssertValidDomainName("[IPv6:2001:0db8:85a3:0000:0000:8a2e:0370:7334]");
      }

      [Test]
      public void TestDomainWithInvalidIPv6Literal()
      {
         AssertInvalidDomainName("[IPv6:invalid]");
      }

      private void AssertInvalidDomainName(string domainName)
      {
         var domain = _application.Domains.Add();
         domain.Name = domainName;

         var exception = Assert.Throws<COMException>(() => domain.Save());
         Assert.AreEqual("Failed to save object. The domain name you have entered is not a valid domain name.",
            exception.Message);
      }

      private void AssertValidDomainName(string domainName)
      {
         var domain = _application.Domains.Add();
         domain.Name = domainName;
         domain.Save();
      }
   }
}