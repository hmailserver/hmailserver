using NUnit.Framework;
using hMailServer;

namespace RegressionTests.Shared
{
   [TestFixture]
   public class UtilityServices : TestFixtureBase
   {
      [TestFixtureSetUp]
      public void SetUpFixture()
      {
         SingletonProvider<TestSetup>.Instance.Authenticate();
      }

      [Test]
      [Description("Test usage of algorithm MD5.")]
      public void TestMD5Hashing()
      {
         Utilities utilities = _application.Utilities;

         string passwordHash = utilities.MD5("Password");
      }

      [Test]
      public void TestStrongPasswords()
      {
         Utilities utilities = _application.Utilities;

         // The password should not be a part of a user name.
         Assert.IsFalse(utilities.IsStrongPassword("testar@test.com", "testar"));

         // The password should be more than 4 characters long.
         Assert.IsFalse(utilities.IsStrongPassword("vaffe@test.com", "test"));

         // If the password is 6 characters or less, it should contain a number or special char
         Assert.IsFalse(utilities.IsStrongPassword("vaffe@test.com", "testar"));

         // Typically bad passwords...
         Assert.IsFalse(utilities.IsStrongPassword("vaffe@test.com", "secret"));
         Assert.IsFalse(utilities.IsStrongPassword("vaffe@test.com", "password"));
         Assert.IsFalse(utilities.IsStrongPassword("vaffe@test.com", "info"));
         Assert.IsFalse(utilities.IsStrongPassword("vaffe@test.com", "webmaster"));
         Assert.IsFalse(utilities.IsStrongPassword("vaffe@test.com", "admin"));
         Assert.IsFalse(utilities.IsStrongPassword("vaffe@test.com", "12345"));
         Assert.IsFalse(utilities.IsStrongPassword("vaffe@test.com", "qwerty"));

         // Contains more than 6 characters - OK
         Assert.IsTrue(utilities.IsStrongPassword("vaffe@test.com", "testarp"));

         // Contains more than 6 characters - OK
         Assert.IsTrue(utilities.IsStrongPassword("vaffe@test.com", "test_"));
      }
   }
}