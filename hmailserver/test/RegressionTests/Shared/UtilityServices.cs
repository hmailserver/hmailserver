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
         CustomAssert.IsFalse(utilities.IsStrongPassword("testar@test.com", "testar"));

         // The password should be more than 4 characters long.
         CustomAssert.IsFalse(utilities.IsStrongPassword("vaffe@test.com", "test"));

         // If the password is 6 characters or less, it should contain a number or special char
         CustomAssert.IsFalse(utilities.IsStrongPassword("vaffe@test.com", "testar"));

         // Typically bad passwords...
         CustomAssert.IsFalse(utilities.IsStrongPassword("vaffe@test.com", "secret"));
         CustomAssert.IsFalse(utilities.IsStrongPassword("vaffe@test.com", "password"));
         CustomAssert.IsFalse(utilities.IsStrongPassword("vaffe@test.com", "info"));
         CustomAssert.IsFalse(utilities.IsStrongPassword("vaffe@test.com", "webmaster"));
         CustomAssert.IsFalse(utilities.IsStrongPassword("vaffe@test.com", "admin"));
         CustomAssert.IsFalse(utilities.IsStrongPassword("vaffe@test.com", "12345"));
         CustomAssert.IsFalse(utilities.IsStrongPassword("vaffe@test.com", "qwerty"));

         // Contains more than 6 characters - OK
         CustomAssert.IsTrue(utilities.IsStrongPassword("vaffe@test.com", "testarp"));

         // Contains more than 6 characters - OK
         CustomAssert.IsTrue(utilities.IsStrongPassword("vaffe@test.com", "test_"));
      }
   }
}