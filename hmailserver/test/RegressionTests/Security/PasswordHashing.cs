// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

using System;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;
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

      // The password the test suite itself authenticates the API session with.
      private const string AdministratorPassword = "testar";

      private const int AlgorithmArgon2id = 1;
      private const int AlgorithmPbkdf2Sha256 = 2;

      private const int DefaultArgon2idMemoryCost = 19456;
      private const int DefaultArgon2idIterations = 2;

      private const int EncryptionPlainText = 0;
      private const int EncryptionBlowfish = 1;
      private const int EncryptionMd5 = 2;

      // Must match PasswordHasher::Constants in the server.
      private const int MinArgon2idMemoryCost = 8192;
      private const int MaxArgon2idMemoryCost = 1048576;
      private const int MinArgon2idIterations = 2;
      private const int MaxArgon2idIterations = 20;
      private const int MinPbkdf2Iterations = 100000;
      private const int MaxPbkdf2Iterations = 10000000;

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

      private static string GetIniFileName()
      {
         // The server administrator password lives in hMailServer.ini rather than in
         // the database, and the file sits next to the running executable.
         var processes = Process.GetProcessesByName("hmailserver");

         if (processes.Length != 1)
            throw new InvalidOperationException("Expected exactly one running hMailServer.exe.");

         return Path.Combine(Path.GetDirectoryName(processes[0].MainModule.FileName), "hMailServer.ini");
      }

      private static string ReadStoredAdministratorPassword()
      {
         const string key = "AdministratorPassword=";

         // Read without taking a lock, so that the server can keep writing to the file.
         using (var fileStream = new FileStream(GetIniFileName(), FileMode.Open, FileAccess.Read, FileShare.ReadWrite))
         using (var textReader = new StreamReader(fileStream))
         {
            string line;

            while ((line = textReader.ReadLine()) != null)
            {
               line = line.Trim();

               if (line.StartsWith(key, StringComparison.OrdinalIgnoreCase))
                  return line.Substring(key.Length);
            }
         }

         return string.Empty;
      }

      private static void RequireKnownAdministratorPassword()
      {
         // These tests have to write the server administrator password, and there is no
         // API to read the stored value back or to restore an empty one - so they can
         // only put back a password they already know. Where the administrator password
         // is not the one this suite authenticates with, there is nothing safe to
         // restore, and the test is skipped rather than left changing the environment.
         if (new Application().Authenticate("Administrator", AdministratorPassword) == null)
            Assert.Ignore("The server administrator password is not '" + AdministratorPassword +
                          "', so this test cannot restore it afterwards.");
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
      public void TheAdministratorPasswordIsNotRehashedWhenAutoUpgradeIsOff()
      {
         // The administrator account has a logon path of its own, in COMAuthentication,
         // rather than going through PasswordValidator. It has to honour the same
         // opt-out: an administrator who switched the upgrade off - to decide when the
         // migration happens - should not find this password migrated behind their back.
         RequireKnownAdministratorPassword();

         try
         {
            _settings.SetAdministratorPassword(AdministratorPassword);

            var beforeChange = ReadStoredAdministratorPassword();
            Assert.IsTrue(beforeChange.StartsWith("$argon2id$"),
               "Expected an Argon2id hash, but got: " + beforeChange);

            // Move the cost away from the one the stored hash was created with, so that
            // a logon would rehash if it were allowed to.
            _settings.PasswordHashMemoryCost = 32768;
            _settings.PasswordHashAutoUpgradeEnabled = false;

            Assert.IsNotNull(new Application().Authenticate("Administrator", AdministratorPassword));

            Assert.AreEqual(beforeChange, ReadStoredAdministratorPassword(),
               "The administrator password was rehashed even though the upgrade is switched off.");

            _settings.PasswordHashAutoUpgradeEnabled = true;

            Assert.IsNotNull(new Application().Authenticate("Administrator", AdministratorPassword));

            var afterUpgrade = ReadStoredAdministratorPassword();

            Assert.AreNotEqual(beforeChange, afterUpgrade);
            StringAssert.Contains("m=32768", afterUpgrade);
         }
         finally
         {
            _settings.PasswordHashMemoryCost = DefaultArgon2idMemoryCost;
            _settings.SetAdministratorPassword(AdministratorPassword);
         }
      }

      [Test]
      public void SettingTheAdministratorPasswordStoresAHashOfIt()
      {
         RequireKnownAdministratorPassword();

         _settings.SetAdministratorPassword(AdministratorPassword);

         var stored = ReadStoredAdministratorPassword();

         Assert.IsTrue(stored.StartsWith("$argon2id$"),
            "Expected an Argon2id hash, but got: " + stored);

         Assert.IsNotNull(new Application().Authenticate("Administrator", AdministratorPassword));
         Assert.IsNull(new Application().Authenticate("Administrator", AdministratorPassword + "x"));
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

      [Test]
      public void PlaintextPasswordsAreLeftAloneWhenAutoUpgradeIsOff()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, Address, Password);

         OverwriteStoredPassword(account, Password, EncryptionPlainText);

         _settings.PasswordHashAutoUpgradeEnabled = false;
         ClearCache();

         // The account must still be able to log on - only the migration is switched off.
         AssertLogonSucceedsOnAllProtocols(Address, Password);

         ClearCache();
         Assert.AreEqual(Password, GetStoredPassword());
      }

      [Test]
      public void PlaintextPasswordsAreRehashedOnLogonWhenAutoUpgradeIsOn()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, Address, Password);

         OverwriteStoredPassword(account, Password, EncryptionPlainText);

         _settings.PasswordHashAutoUpgradeEnabled = true;
         ClearCache();

         Assert.IsTrue(new Pop3ClientSimulator().ConnectAndLogon(Address, Password));
         ClearCache();

         var storedPassword = GetStoredPassword();
         Assert.AreNotEqual(Password, storedPassword);
         Assert.IsTrue(storedPassword.StartsWith("$argon2id$"),
            "Expected an Argon2id hash after the rehash, but got: " + storedPassword);
      }

      [Test]
      public void ReadingAPlaintextAccountDoesNotRehashItRegardlessOfAutoUpgrade()
      {
         // PersistentAccount::ReadObject must not migrate a plaintext account on its
         // own - that would ignore PasswordHashAutoUpgrade and turn a plain account
         // listing into a database write. Re-reading the account (which Password /
         // GetStoredPassword do, through the cache) must leave it untouched even with
         // auto-upgrade on; only a verified logon may migrate it.
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, Address, Password);

         OverwriteStoredPassword(account, Password, EncryptionPlainText);

         _settings.PasswordHashAutoUpgradeEnabled = true;
         ClearCache();

         Assert.AreEqual(Password, GetStoredPassword());

         ClearCache();
         Assert.AreEqual(Password, GetStoredPassword());
      }

      [Test]
      public void APlaintextPasswordCanBeUsedWithDifferentCasingAndStillWorksAfterTheRehash()
      {
         // This is the Fix 1 regression test. Plaintext and Blowfish accounts compare
         // case-insensitively, so a client that has always used one casing keeps
         // working through the migration to Argon2id - which is case sensitive - only
         // if the exact casing the client sent is what gets hashed and stored, not the
         // casing that happened to be recorded when the account was created.
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, Address, Password);

         OverwriteStoredPassword(account, Password, EncryptionPlainText);

         var differentCasing = Password.ToUpperInvariant();
         Assert.AreNotEqual(Password, differentCasing);

         // Allowed pre-migration because the plaintext comparison is case-insensitive.
         Assert.IsTrue(new Pop3ClientSimulator().ConnectAndLogon(Address, differentCasing));
         ClearCache();

         Assert.IsTrue(GetStoredPassword().StartsWith("$argon2id$"));

         // The casing that was actually used to log on - not the originally stored
         // casing - must keep working after the rehash.
         AssertLogonSucceedsOnAllProtocols(Address, differentCasing);
      }

      [Test]
      public void ABlowfishPasswordCanBeUsedWithDifferentCasingAndStillWorksAfterTheRehash()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, Address, Password);

         var blowfish = _application.Utilities.BlowfishEncrypt(Password);
         OverwriteStoredPassword(account, blowfish, EncryptionBlowfish);

         var differentCasing = Password.ToUpperInvariant();
         Assert.AreNotEqual(Password, differentCasing);

         Assert.IsTrue(new Pop3ClientSimulator().ConnectAndLogon(Address, differentCasing));
         ClearCache();

         Assert.IsTrue(GetStoredPassword().StartsWith("$argon2id$"));

         AssertLogonSucceedsOnAllProtocols(Address, differentCasing);
      }

      [Test]
      public void APermissiveScriptHandlerDoesNotOverwriteTheStoredPassword()
      {
         // hash_verified in PasswordValidator::ValidatePassword must never be set on
         // the OnClientValidatePassword script-override path - a permissive handler
         // that lets everyone through must not cause whatever the client sent to be
         // hashed and written over the real stored password.
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, Address, Password);

         var storedBeforeScript = GetStoredPassword();

         var scripting = _application.Settings.Scripting;

         var script =
            @"Sub OnClientValidatePassword(account, password)
                 Result.Value = 0
              End Sub";

         System.IO.File.WriteAllText(scripting.CurrentScriptFile, script);

         scripting.Enabled = true;
         scripting.Reload();

         try
         {
            Assert.IsTrue(ImapClientSimulator.ValidatePassword(Address, "WhateverTheClientSent"));

            ClearCache();
            Assert.AreEqual(storedBeforeScript, GetStoredPassword());
         }
         finally
         {
            scripting.Enabled = false;
         }
      }

      [Test]
      public void MemoryCostBelowTheMinimumIsRejected()
      {
         var ex = Assert.Throws<COMException>(() => _settings.PasswordHashMemoryCost = MinArgon2idMemoryCost - 1);
         StringAssert.Contains("Invalid password hash memory cost", ex.Message);
      }

      [Test]
      public void MemoryCostAtTheMinimumIsAccepted()
      {
         _settings.PasswordHashMemoryCost = MinArgon2idMemoryCost;
         Assert.AreEqual(MinArgon2idMemoryCost, _settings.PasswordHashMemoryCost);
      }

      [Test]
      public void MemoryCostAboveTheMaximumIsRejected()
      {
         var ex = Assert.Throws<COMException>(() => _settings.PasswordHashMemoryCost = MaxArgon2idMemoryCost + 1);
         StringAssert.Contains("Invalid password hash memory cost", ex.Message);
      }

      [Test]
      public void MemoryCostAtTheMaximumIsAccepted()
      {
         _settings.PasswordHashMemoryCost = MaxArgon2idMemoryCost;
         Assert.AreEqual(MaxArgon2idMemoryCost, _settings.PasswordHashMemoryCost);
      }

      [Test]
      public void MemoryCostZeroIsStillAccepted()
      {
         _settings.PasswordHashMemoryCost = 32768;
         _settings.PasswordHashMemoryCost = 0;
         Assert.AreEqual(0, _settings.PasswordHashMemoryCost);
      }

      [Test]
      public void Argon2idIterationsBelowTheMinimumIsRejected()
      {
         var ex = Assert.Throws<COMException>(() => _settings.PasswordHashIterations = MinArgon2idIterations - 1);
         StringAssert.Contains("Invalid password hash iteration count", ex.Message);
      }

      [Test]
      public void Argon2idIterationsAtTheMinimumIsAccepted()
      {
         _settings.PasswordHashIterations = MinArgon2idIterations;
         Assert.AreEqual(MinArgon2idIterations, _settings.PasswordHashIterations);
      }

      [Test]
      public void Argon2idIterationsAboveTheMaximumIsRejected()
      {
         var ex = Assert.Throws<COMException>(() => _settings.PasswordHashIterations = MaxArgon2idIterations + 1);
         StringAssert.Contains("Invalid password hash iteration count", ex.Message);
      }

      [Test]
      public void Argon2idIterationsAtTheMaximumIsAccepted()
      {
         _settings.PasswordHashIterations = MaxArgon2idIterations;
         Assert.AreEqual(MaxArgon2idIterations, _settings.PasswordHashIterations);
      }

      [Test]
      public void Pbkdf2IterationsBelowTheMinimumIsRejected()
      {
         _settings.PasswordHashAlgorithm = AlgorithmPbkdf2Sha256;

         var ex = Assert.Throws<COMException>(() => _settings.PasswordHashIterations = MinPbkdf2Iterations - 1);
         StringAssert.Contains("Invalid password hash iteration count", ex.Message);
      }

      [Test]
      public void Pbkdf2IterationsAtTheMinimumIsAccepted()
      {
         _settings.PasswordHashAlgorithm = AlgorithmPbkdf2Sha256;

         _settings.PasswordHashIterations = MinPbkdf2Iterations;
         Assert.AreEqual(MinPbkdf2Iterations, _settings.PasswordHashIterations);
      }

      [Test]
      public void Pbkdf2IterationsAboveTheMaximumIsRejected()
      {
         _settings.PasswordHashAlgorithm = AlgorithmPbkdf2Sha256;

         var ex = Assert.Throws<COMException>(() => _settings.PasswordHashIterations = MaxPbkdf2Iterations + 1);
         StringAssert.Contains("Invalid password hash iteration count", ex.Message);
      }

      [Test]
      public void Pbkdf2IterationsAtTheMaximumIsAccepted()
      {
         _settings.PasswordHashAlgorithm = AlgorithmPbkdf2Sha256;

         _settings.PasswordHashIterations = MaxPbkdf2Iterations;
         Assert.AreEqual(MaxPbkdf2Iterations, _settings.PasswordHashIterations);
      }

      [Test]
      public void IterationsZeroIsStillAcceptedForBothAlgorithms()
      {
         _settings.PasswordHashIterations = 6;
         _settings.PasswordHashIterations = 0;
         Assert.AreEqual(0, _settings.PasswordHashIterations);

         _settings.PasswordHashAlgorithm = AlgorithmPbkdf2Sha256;
         _settings.PasswordHashIterations = 700000;
         _settings.PasswordHashIterations = 0;
         Assert.AreEqual(0, _settings.PasswordHashIterations);
      }
   }
}
