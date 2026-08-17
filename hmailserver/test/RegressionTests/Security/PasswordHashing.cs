// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

using System;
using System.Security.Authentication;
using System.Text;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;

namespace RegressionTests.Security
{
   [TestFixture]
   public class PasswordHashing : TestFixtureBase
   {
      private const string Address = "test@example.test";
      private const string Password = "SecretPassword";

      private const int AlgorithmArgon2id = 1;
      private const int AlgorithmPbkdf2Sha256 = 2;

      private const int DefaultArgon2idMemoryCost = 19456;
      private const int DefaultArgon2idIterations = 2;

      private const int EncryptionBlowfish = 1;
      private const int EncryptionMd5 = 2;

      private static string EncodeBase64(string s)
      {
         return Convert.ToBase64String(Encoding.UTF8.GetBytes(s));
      }

      private static string GetStoredPassword()
      {
         // Read the account back from the database rather than from the object we
         // created it with. The getter returns the stored value verbatim.
         return SingletonProvider<TestSetup>.Instance.GetApp().Domains[0].Accounts[0].Password;
      }

      private static void ClearCache()
      {
         // Mandatory - the password validator reads accounts through the cache.
         SingletonProvider<TestSetup>.Instance.GetApp().Settings.Cache.Clear();
      }

      private static void OverwriteStoredPassword(Account account, string password, int passwordEncryption)
      {
         var sql = string.Format(
            "update hm_accounts set accountpassword = '{0}', accountpwencryption = {1} where accountid = {2}",
            TestSetup.Escape(password), passwordEncryption, account.ID);

         SingletonProvider<TestSetup>.Instance.GetApp().Database.ExecuteSQL(sql);

         ClearCache();
      }

      private static void AssertLogonSucceedsOnAllProtocols(string address, string password)
      {
         var pop3 = new Pop3ClientSimulator();
         Assert.IsTrue(pop3.ConnectAndLogon(address, password));
         pop3.Disconnect();

         var imap = new ImapClientSimulator();
         Assert.IsTrue(imap.ConnectAndLogon(address, password));
         imap.Disconnect();

         string errorMessage;
         var smtp = new SmtpClientSimulator();
         smtp.ConnectAndLogon(EncodeBase64(address), EncodeBase64(password), out errorMessage);
         smtp.Disconnect();
      }

      private static void AssertLogonFails(string address, string password)
      {
         string errorMessage;

         var pop3 = new Pop3ClientSimulator();
         Assert.IsFalse(pop3.ConnectAndLogon(address, password, out errorMessage));

         var imap = new ImapClientSimulator();
         Assert.IsFalse(imap.ConnectAndLogon(address, password, out errorMessage));

         var smtp = new SmtpClientSimulator();
         CustomAsserts.Throws<AuthenticationException>(() =>
            smtp.ConnectAndLogon(EncodeBase64(address), EncodeBase64(password), out errorMessage));
      }

      [Test]
      public void PasswordHashSettingsHaveExpectedDefaults()
      {
         // The database scripts seed a concrete cost rather than a zero, so that the
         // administration interfaces show what is actually in use.
         Assert.AreEqual(AlgorithmArgon2id, _settings.PasswordHashAlgorithm);
         Assert.AreEqual(DefaultArgon2idMemoryCost, _settings.PasswordHashMemoryCost);
         Assert.AreEqual(DefaultArgon2idIterations, _settings.PasswordHashIterations);
      }

      [Test]
      public void PasswordHashAutoUpgradeCanBeToggled()
      {
         _settings.PasswordHashAutoUpgradeEnabled = false;
         Assert.IsFalse(_settings.PasswordHashAutoUpgradeEnabled);

         _settings.PasswordHashAutoUpgradeEnabled = true;
         Assert.IsTrue(_settings.PasswordHashAutoUpgradeEnabled);
      }

      [Test]
      public void LegacyPasswordsAreLeftAloneWhenAutoUpgradeIsOff()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, Address, Password);

         var md5 = _application.Utilities.MD5(Password);
         OverwriteStoredPassword(account, md5, EncryptionMd5);

         _settings.PasswordHashAutoUpgradeEnabled = false;
         ClearCache();

         // The account must still be able to log on - only the migration is switched off.
         AssertLogonSucceedsOnAllProtocols(Address, Password);

         ClearCache();
         Assert.AreEqual(md5, GetStoredPassword());
      }

      [Test]
      public void AStrongerCostIsNotAppliedWhenAutoUpgradeIsOff()
      {
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, Address, Password);

         var beforeChange = GetStoredPassword();

         _settings.PasswordHashMemoryCost = 32768;
         _settings.PasswordHashAutoUpgradeEnabled = false;
         ClearCache();

         Assert.IsTrue(new Pop3ClientSimulator().ConnectAndLogon(Address, Password));
         ClearCache();

         Assert.AreEqual(beforeChange, GetStoredPassword());
      }

      [Test]
      public void TurningAutoUpgradeBackOnResumesMigration()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, Address, Password);

         var md5 = _application.Utilities.MD5(Password);
         OverwriteStoredPassword(account, md5, EncryptionMd5);

         _settings.PasswordHashAutoUpgradeEnabled = false;
         ClearCache();

         Assert.IsTrue(new Pop3ClientSimulator().ConnectAndLogon(Address, Password));
         ClearCache();
         Assert.AreEqual(md5, GetStoredPassword());

         _settings.PasswordHashAutoUpgradeEnabled = true;
         ClearCache();

         Assert.IsTrue(new Pop3ClientSimulator().ConnectAndLogon(Address, Password));
         ClearCache();

         Assert.IsTrue(GetStoredPassword().StartsWith("$argon2id$"));
      }

      [Test]
      public void NewAccountsAreHashedEvenWhenAutoUpgradeIsOff()
      {
         // The switch governs migration during logon, not how new passwords are stored.
         _settings.PasswordHashAutoUpgradeEnabled = false;

         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, Address, Password);

         Assert.IsTrue(GetStoredPassword().StartsWith("$argon2id$"));
      }

      [Test]
      public void PasswordHashSettingsCanBeChanged()
      {
         _settings.PasswordHashAlgorithm = AlgorithmPbkdf2Sha256;
         _settings.PasswordHashMemoryCost = 32768;
         _settings.PasswordHashIterations = 700000;

         Assert.AreEqual(AlgorithmPbkdf2Sha256, _settings.PasswordHashAlgorithm);
         Assert.AreEqual(32768, _settings.PasswordHashMemoryCost);
         Assert.AreEqual(700000, _settings.PasswordHashIterations);
      }

      [Test]
      public void NewAccountsAreHashedUsingTheConfiguredAlgorithm()
      {
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, Address, Password);

         var storedPassword = GetStoredPassword();

         Assert.IsTrue(storedPassword.StartsWith("$argon2id$"),
            "Expected an Argon2id hash, but got: " + storedPassword);
      }

      [Test]
      public void NewAccountsAreHashedUsingPbkdf2WhenSelected()
      {
         _settings.PasswordHashAlgorithm = AlgorithmPbkdf2Sha256;

         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, Address, Password);

         var storedPassword = GetStoredPassword();

         Assert.IsTrue(storedPassword.StartsWith("$pbkdf2-sha256$"),
            "Expected a PBKDF2-SHA256 hash, but got: " + storedPassword);
      }

      [Test]
      public void LegacyMd5PasswordsCanStillBeUsedToLogOn()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, Address, Password);

         var md5 = _application.Utilities.MD5(Password);
         OverwriteStoredPassword(account, md5, EncryptionMd5);

         AssertLogonSucceedsOnAllProtocols(Address, Password);
      }

      [Test]
      public void LegacyMd5PasswordsRejectAnIncorrectPassword()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, Address, Password);

         var md5 = _application.Utilities.MD5(Password);
         OverwriteStoredPassword(account, md5, EncryptionMd5);

         AssertLogonFails(Address, "WrongPassword");
      }

      [Test]
      public void LegacyBlowfishPasswordsCanStillBeUsedToLogOn()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, Address, Password);

         var blowfish = _application.Utilities.BlowfishEncrypt(Password);
         OverwriteStoredPassword(account, blowfish, EncryptionBlowfish);

         AssertLogonSucceedsOnAllProtocols(Address, Password);
      }

      [Test]
      public void LegacyBlowfishPasswordsRejectAnIncorrectPassword()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, Address, Password);

         var blowfish = _application.Utilities.BlowfishEncrypt(Password);
         OverwriteStoredPassword(account, blowfish, EncryptionBlowfish);

         AssertLogonFails(Address, "WrongPassword");
      }

      [Test]
      public void LegacyPasswordsAreRehashedOnLogon()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, Address, Password);

         var md5 = _application.Utilities.MD5(Password);
         OverwriteStoredPassword(account, md5, EncryptionMd5);

         Assert.AreEqual(md5, GetStoredPassword());

         Assert.IsTrue(new Pop3ClientSimulator().ConnectAndLogon(Address, Password));

         ClearCache();

         var storedPassword = GetStoredPassword();

         Assert.AreNotEqual(md5, storedPassword);
         Assert.IsTrue(storedPassword.StartsWith("$argon2id$"),
            "Expected an Argon2id hash after the rehash, but got: " + storedPassword);
      }

      [Test]
      public void RehashingIsNotRepeatedOnSubsequentLogons()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, Address, Password);

         OverwriteStoredPassword(account, _application.Utilities.MD5(Password), EncryptionMd5);

         Assert.IsTrue(new Pop3ClientSimulator().ConnectAndLogon(Address, Password));
         ClearCache();

         var afterFirstLogon = GetStoredPassword();

         Assert.IsTrue(new Pop3ClientSimulator().ConnectAndLogon(Address, Password));
         ClearCache();
         Assert.AreEqual(afterFirstLogon, GetStoredPassword());

         Assert.IsTrue(new Pop3ClientSimulator().ConnectAndLogon(Address, Password));
         ClearCache();
         Assert.AreEqual(afterFirstLogon, GetStoredPassword());
      }

      [Test]
      public void AStrongerCostCausesARehashOnLogon()
      {
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, Address, Password);

         var beforeChange = GetStoredPassword();
         Assert.IsTrue(beforeChange.Contains("m=19456"));

         _settings.PasswordHashMemoryCost = 32768;
         ClearCache();

         Assert.IsTrue(new Pop3ClientSimulator().ConnectAndLogon(Address, Password));
         ClearCache();

         var afterChange = GetStoredPassword();

         Assert.AreNotEqual(beforeChange, afterChange);
         Assert.IsTrue(afterChange.Contains("m=32768"),
            "Expected the new memory cost to be recorded in the hash, but got: " + afterChange);

         // And the account must still be usable afterwards.
         AssertLogonSucceedsOnAllProtocols(Address, Password);
      }

      [Test]
      public void ALowerMemoryCostAlsoCausesARehashOnLogon()
      {
         // Lowering the cost is as deliberate a decision as raising it, so the stored
         // hashes are expected to follow it down as well as up.
         _settings.PasswordHashMemoryCost = 32768;

         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, Address, Password);

         Assert.IsTrue(GetStoredPassword().Contains("m=32768"));

         _settings.PasswordHashMemoryCost = DefaultArgon2idMemoryCost;
         ClearCache();

         Assert.IsTrue(new Pop3ClientSimulator().ConnectAndLogon(Address, Password));
         ClearCache();

         var afterChange = GetStoredPassword();

         Assert.IsTrue(afterChange.Contains("m=19456"),
            "Expected the lowered memory cost to be recorded in the hash, but got: " + afterChange);

         AssertLogonSucceedsOnAllProtocols(Address, Password);
      }

      [Test]
      public void ALowerIterationCountAlsoCausesARehashOnLogon()
      {
         _settings.PasswordHashIterations = 6;

         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, Address, Password);

         Assert.IsTrue(GetStoredPassword().Contains("t=6"));

         _settings.PasswordHashIterations = DefaultArgon2idIterations;
         ClearCache();

         Assert.IsTrue(new Pop3ClientSimulator().ConnectAndLogon(Address, Password));
         ClearCache();

         var afterChange = GetStoredPassword();

         Assert.IsTrue(afterChange.Contains("t=2"),
            "Expected the lowered iteration count to be recorded in the hash, but got: " + afterChange);

         AssertLogonSucceedsOnAllProtocols(Address, Password);
      }

      [Test]
      public void ChangingTheAlgorithmCausesARehashOnLogon()
      {
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, Address, Password);

         Assert.IsTrue(GetStoredPassword().StartsWith("$argon2id$"));

         _settings.PasswordHashAlgorithm = AlgorithmPbkdf2Sha256;
         ClearCache();

         Assert.IsTrue(new Pop3ClientSimulator().ConnectAndLogon(Address, Password));
         ClearCache();

         Assert.IsTrue(GetStoredPassword().StartsWith("$pbkdf2-sha256$"));

         AssertLogonSucceedsOnAllProtocols(Address, Password);
      }

      [Test]
      public void AFailedLogonDoesNotChangeTheStoredPassword()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, Address, Password);

         var md5 = _application.Utilities.MD5(Password);
         OverwriteStoredPassword(account, md5, EncryptionMd5);

         string errorMessage;
         Assert.IsFalse(new Pop3ClientSimulator().ConnectAndLogon(Address, "WrongPassword", out errorMessage));

         ClearCache();

         Assert.AreEqual(md5, GetStoredPassword());
      }

      [Test]
      public void AnAccountRemainsUsableAfterBeingRehashed()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, Address, Password);

         OverwriteStoredPassword(account, _application.Utilities.MD5(Password), EncryptionMd5);

         Assert.IsTrue(new Pop3ClientSimulator().ConnectAndLogon(Address, Password));
         ClearCache();

         AssertLogonSucceedsOnAllProtocols(Address, Password);
         AssertLogonFails(Address, "WrongPassword");
      }
   }
}
