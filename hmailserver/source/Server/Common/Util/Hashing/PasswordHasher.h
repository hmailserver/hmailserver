// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

namespace HM
{
   /*
      Hashes and verifies account passwords using a key derivation function with
      a per-installation tunable cost.

      Hashes are stored as self-describing PHC strings, so that the parameters a
      hash was created with travel with the hash itself and can be compared
      against the currently configured target:

         $argon2id$v=19$m=19456,t=2,p=1$<salt>$<hash>
         $pbkdf2-sha256$i=600000$<salt>$<hash>

      Salt and hash are base64 encoded without padding, as the PHC string format
      specifies. Adding another algorithm later requires no database change - the
      algorithm is read from the stored string.

      HashCreator is deliberately left alone; it is used for DKIM and for
      verification of the legacy password hashes.
   */
   class PasswordHasher
   {
   public:

      enum Algorithm
      {
         AlgorithmArgon2id = 1,
         AlgorithmPBKDF2SHA256 = 2
      };

      enum Constants
      {
         DefaultArgon2idMemoryCostKb = 19456,
         DefaultArgon2idIterations = 2,
         DefaultPBKDF2Iterations = 600000,

         SaltLength = 16,
         MinimumSaltLength = 16,
         HashLength = 32
      };

      static AnsiString Hash(const AnsiString &password);
      // Hashes the password using the configured algorithm and cost, and returns
      // a PHC string. Returns an empty string if hashing failed.

      static bool Verify(const AnsiString &password, const AnsiString &stored);
      // Verifies the password against a PHC string, using the parameters recorded
      // in that string.

      static bool NeedsRehash(const AnsiString &stored);
      // True if the stored hash was created using another algorithm, another cost or
      // a shorter salt than what is configured right now. A cost that differs in
      // either direction counts - lowering it is as deliberate as raising it.

      static bool IsPasswordHash(const AnsiString &stored);
      // True if the string looks like a PHC string. Used to tell the new hashes
      // apart from the legacy ones, which never start with a dollar sign.

      static Algorithm GetConfiguredAlgorithm();
      static unsigned int GetConfiguredMemoryCostKb();
      static unsigned int GetConfiguredIterations();
      // The configured settings, with zero resolved to the recommended default for
      // the configured algorithm. The zero must be resolved before any comparison
      // against a stored hash is made, or every login would trigger a rehash.

      static bool DeriveArgon2id(const std::vector<unsigned char> &password,
                                 const std::vector<unsigned char> &salt,
                                 const std::vector<unsigned char> &secret,
                                 const std::vector<unsigned char> &associatedData,
                                 unsigned int lanes,
                                 unsigned int memoryCostKb,
                                 unsigned int iterations,
                                 std::vector<unsigned char> &output);

      static bool DerivePBKDF2SHA256(const std::vector<unsigned char> &password,
                                     const std::vector<unsigned char> &salt,
                                     unsigned int iterations,
                                     std::vector<unsigned char> &output);
      // Low level key derivation. The size of output decides the length of the
      // derived key, and must be set by the caller. Exposed so that the known
      // answer tests can run the published test vectors, which use parameters we
      // would never write ourselves.

   private:

      struct HashParameters
      {
         HashParameters() :
            algorithm(AlgorithmArgon2id),
            memory_cost_kb(0),
            iterations(0),
            lanes(1)
         {
         }

         Algorithm algorithm;
         unsigned int memory_cost_kb;
         unsigned int iterations;
         unsigned int lanes;
         std::vector<unsigned char> salt;
         std::vector<unsigned char> hash;
      };

      static bool Parse_(const AnsiString &stored, HashParameters &parameters);
      static AnsiString Format_(const HashParameters &parameters);

      static bool Compute_(const AnsiString &password, const HashParameters &parameters, std::vector<unsigned char> &output);

      static bool GenerateSalt_(std::vector<unsigned char> &salt);

      static AnsiString EncodeBase64_(const std::vector<unsigned char> &input);
      static bool DecodeBase64_(const AnsiString &input, std::vector<unsigned char> &output);

      static std::vector<unsigned char> ToBytes_(const AnsiString &input);

      enum { ARGON2_VERSION_13 = 0x13 };
   };

   class PasswordHasherTester
   {
   public:
      void Test();
   };
}
