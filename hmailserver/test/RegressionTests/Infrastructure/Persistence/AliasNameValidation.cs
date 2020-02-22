using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.Infrastructure.Persistence
{
   [TestFixture]
   public class AliasNameValidation : TestFixtureBase
   {
      [Test]
      public void TestAliasWithSpecialCharacters1()
      {
         AssertValidAliasAddress("user+mailbox@test.com");
      }

      [Test]
      public void TestAliasWithSpecialCharacters2()
      {
         AssertValidAliasAddress("customer/department=shipping@test.com");
      }

      [Test]
      public void TestAliasWithSpecialCharacters3()
      {
         AssertValidAliasAddress("$A12345@test.com");
      }

      [Test]
      public void TestAliasWithSpecialCharacters4()
      {
         AssertValidAliasAddress("!def!xyz%abc@test.com");
      }

      [Test]
      public void TestAliasWithSpecialCharacters5()
      {
         AssertValidAliasAddress("_somename@test.com");
      }

      [Test]
      public void TestAliasWithSpecialCharacters6()
      {
         AssertValidAliasAddress("!#$%&'*+-/=?^_`.{|}~@test.com");
      }

      private void AssertValidAliasAddress(string address)
      {
         SingletonProvider<TestSetup>.Instance.AddAlias(_domain, address, "secret@example.com");
      }

   }
}
