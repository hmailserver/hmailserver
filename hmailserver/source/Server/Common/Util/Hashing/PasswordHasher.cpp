// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include <stdafx.h>

#include <limits.h>
#include <stdexcept>
#include <string.h>

#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <openssl/kdf.h>
#include <openssl/core_names.h>
#include <openssl/params.h>
#include <openssl/rand.h>

#include "../Encoding/Base64.h"

#include "PasswordHasher.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   namespace
   {
      const char *ARGON2ID_IDENTIFIER = "argon2id";
      const char *PBKDF2_SHA256_IDENTIFIER = "pbkdf2-sha256";

      const unsigned char EMPTY_BYTE = 0;

      const unsigned char *
      GetBufferPointer(const std::vector<unsigned char> &buffer)
      {
         // OpenSSL is not given a null pointer even when there is nothing to point at.
         return buffer.empty() ? &EMPTY_BYTE : &buffer[0];
      }

      std::vector<AnsiString>
      SplitFields(const AnsiString &input, char separator)
      {
         std::vector<AnsiString> result;

         AnsiString current;

         for (int i = 0; i < input.GetLength(); i++)
         {
            char c = input.GetAt(i);

            if (c == separator)
            {
               result.push_back(current);
               current = "";
            }
            else
            {
               current += c;
            }
         }

         result.push_back(current);

         return result;
      }

      AnsiString
      ToAnsiString(unsigned int value)
      {
         char buffer[16];
         sprintf_s(buffer, sizeof(buffer), "%u", value);
         return AnsiString(buffer);
      }

      bool
      ParseNamedValue(const AnsiString &field, const AnsiString &name, unsigned int &value)
      {
         AnsiString prefix = name + "=";

         if (field.Mid(0, prefix.GetLength()) != prefix)
            return false;

         AnsiString digits = field.Mid(prefix.GetLength());

         if (digits.GetLength() == 0 || digits.GetLength() > 10)
            return false;

         unsigned __int64 parsed = 0;

         for (int i = 0; i < digits.GetLength(); i++)
         {
            char c = digits.GetAt(i);

            if (c < '0' || c > '9')
               return false;

            parsed = parsed * 10 + (c - '0');
         }

         if (parsed > UINT_MAX)
            return false;

         value = (unsigned int) parsed;
         return true;
      }
   }

   AnsiString
   PasswordHasher::Hash(const AnsiString &password)
   {
      HashParameters parameters;

      parameters.algorithm = GetConfiguredAlgorithm();
      parameters.memory_cost_kb = GetConfiguredMemoryCostKb();
      parameters.iterations = GetConfiguredIterations();
      parameters.lanes = 1;

      if (!GenerateSalt_(parameters.salt))
         return "";

      if (!Compute_(password, parameters, parameters.hash))
         return "";

      return Format_(parameters);
   }

   bool
   PasswordHasher::Verify(const AnsiString &password, const AnsiString &stored)
   {
      HashParameters parameters;

      if (!Parse_(stored, parameters))
         return false;

      std::vector<unsigned char> computed;

      if (!Compute_(password, parameters, computed))
         return false;

      if (computed.size() != parameters.hash.size())
         return false;

      return CRYPTO_memcmp(&computed[0], &parameters.hash[0], computed.size()) == 0;
   }

   bool
   PasswordHasher::NeedsRehash(const AnsiString &stored)
   {
      HashParameters parameters;

      if (!Parse_(stored, parameters))
      {
         // Either a legacy hash, a plain text password or something we cannot make
         // sense of. All of them should be replaced the next time we see the password.
         return true;
      }

      if (parameters.algorithm != GetConfiguredAlgorithm())
         return true;

      // Any difference counts, in either direction. Lowering the cost is a decision
      // the administrator has made deliberately, and the stored hashes are expected
      // to follow it down just as they follow it up.
      if (parameters.iterations != GetConfiguredIterations())
         return true;

      if (parameters.algorithm == AlgorithmArgon2id)
      {
         if (parameters.memory_cost_kb != GetConfiguredMemoryCostKb())
            return true;

         if (parameters.lanes != 1)
            return true;
      }

      if (parameters.salt.size() < (size_t) MinimumSaltLength)
         return true;

      // Not just shorter than expected - any length other than HashLength is one
      // Compute_ can never produce, so nothing would ever verify against it. Parse_
      // rejects those outright, and this stays as the same statement of the invariant.
      if (parameters.hash.size() != (size_t) HashLength)
         return true;

      return false;
   }

   bool
   PasswordHasher::IsPasswordHash(const AnsiString &stored)
   {
      return stored.GetLength() > 0 && stored.GetAt(0) == '$';
   }

   PasswordHasher::Algorithm
   PasswordHasher::GetConfiguredAlgorithm()
   {
      int algorithm = Configuration::Instance()->GetPasswordHashAlgorithm();

      switch (algorithm)
      {
      case AlgorithmPBKDF2SHA256:
         return AlgorithmPBKDF2SHA256;
      case AlgorithmArgon2id:
         return AlgorithmArgon2id;
      default:
         return AlgorithmArgon2id;
      }
   }

   unsigned int
   PasswordHasher::GetConfiguredMemoryCostKb()
   {
      int memoryCost = Configuration::Instance()->GetPasswordHashMemoryCost();

      if (memoryCost <= 0)
         return DefaultArgon2idMemoryCostKb;

      // The COM setter already rejects out-of-range values, but hm_settings can be
      // edited directly, so the value is clamped again here rather than trusted.
      // Clamping to the same bounds the setter enforces means a freshly created hash
      // still round-trips through NeedsRehash as unchanged.
      if ((unsigned int) memoryCost < (unsigned int) MinArgon2idMemoryCostKb)
         return MinArgon2idMemoryCostKb;

      if ((unsigned int) memoryCost > (unsigned int) MaxArgon2idMemoryCostKb)
         return MaxArgon2idMemoryCostKb;

      return (unsigned int) memoryCost;
   }

   unsigned int
   PasswordHasher::GetConfiguredIterations()
   {
      int iterations = Configuration::Instance()->GetPasswordHashIterations();

      if (iterations <= 0)
      {
         // The iteration count means very different things to the two algorithms, so
         // the zero is resolved per algorithm rather than by a single shared default.
         switch (GetConfiguredAlgorithm())
         {
         case AlgorithmPBKDF2SHA256:
            return DefaultPBKDF2Iterations;
         default:
            return DefaultArgon2idIterations;
         }
      }

      // As above: defensive clamping in case hm_settings was edited directly. The
      // valid range differs by an order of magnitude between the two algorithms.
      unsigned int minIterations, maxIterations;

      if (GetConfiguredAlgorithm() == AlgorithmPBKDF2SHA256)
      {
         minIterations = MinPBKDF2Iterations;
         maxIterations = MaxPBKDF2Iterations;
      }
      else
      {
         minIterations = MinArgon2idIterations;
         maxIterations = MaxArgon2idIterations;
      }

      if ((unsigned int) iterations < minIterations)
         return minIterations;

      if ((unsigned int) iterations > maxIterations)
         return maxIterations;

      return (unsigned int) iterations;
   }

   bool
   PasswordHasher::Compute_(const AnsiString &password, const HashParameters &parameters, std::vector<unsigned char> &output)
   {
      std::vector<unsigned char> passwordBytes = ToBytes_(password);
      std::vector<unsigned char> empty;

      output.resize((size_t) HashLength);

      switch (parameters.algorithm)
      {
      case AlgorithmArgon2id:
         return DeriveArgon2id(passwordBytes, parameters.salt, empty, empty, parameters.lanes,
                               parameters.memory_cost_kb, parameters.iterations, output);
      case AlgorithmPBKDF2SHA256:
         return DerivePBKDF2SHA256(passwordBytes, parameters.salt, parameters.iterations, output);
      default:
         return false;
      }
   }

   bool
   PasswordHasher::DeriveArgon2id(const std::vector<unsigned char> &password,
                                  const std::vector<unsigned char> &salt,
                                  const std::vector<unsigned char> &secret,
                                  const std::vector<unsigned char> &associatedData,
                                  unsigned int lanes,
                                  unsigned int memoryCostKb,
                                  unsigned int iterations,
                                  std::vector<unsigned char> &output)
   {
      if (output.empty() || salt.empty() || lanes == 0 || iterations == 0)
         return false;

      EVP_KDF *kdf = EVP_KDF_fetch(nullptr, "ARGON2ID", nullptr);

      if (kdf == nullptr)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5524, "PasswordHasher::DeriveArgon2id",
            "Argon2id is not available in the OpenSSL library being used. OpenSSL 3.2 or later is required.");
         return false;
      }

      EVP_KDF_CTX *context = EVP_KDF_CTX_new(kdf);
      EVP_KDF_free(kdf);

      if (context == nullptr)
         return false;

      // Argon2 is run single-threaded. Asking OpenSSL for more than one thread would
      // require the application to have called OSSL_set_max_threads first, and the
      // result of the derivation is the same either way.
      uint32_t lanesValue = lanes;
      uint32_t threadsValue = 1;
      uint32_t memoryCostValue = memoryCostKb;
      uint32_t iterationsValue = iterations;
      uint32_t versionValue = ARGON2_VERSION_13;

      OSSL_PARAM params[10];
      int index = 0;

      params[index++] = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_PASSWORD,
         const_cast<unsigned char*>(GetBufferPointer(password)), password.size());
      params[index++] = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_SALT,
         const_cast<unsigned char*>(GetBufferPointer(salt)), salt.size());

      if (!secret.empty())
         params[index++] = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_SECRET,
            const_cast<unsigned char*>(GetBufferPointer(secret)), secret.size());

      if (!associatedData.empty())
         params[index++] = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_ARGON2_AD,
            const_cast<unsigned char*>(GetBufferPointer(associatedData)), associatedData.size());

      params[index++] = OSSL_PARAM_construct_uint32(OSSL_KDF_PARAM_ARGON2_LANES, &lanesValue);
      params[index++] = OSSL_PARAM_construct_uint32(OSSL_KDF_PARAM_THREADS, &threadsValue);
      params[index++] = OSSL_PARAM_construct_uint32(OSSL_KDF_PARAM_ARGON2_MEMCOST, &memoryCostValue);
      params[index++] = OSSL_PARAM_construct_uint32(OSSL_KDF_PARAM_ITER, &iterationsValue);
      params[index++] = OSSL_PARAM_construct_uint32(OSSL_KDF_PARAM_ARGON2_VERSION, &versionValue);
      params[index] = OSSL_PARAM_construct_end();

      int result = EVP_KDF_derive(context, &output[0], output.size(), params);

      EVP_KDF_CTX_free(context);

      return result == 1;
   }

   bool
   PasswordHasher::DerivePBKDF2SHA256(const std::vector<unsigned char> &password,
                                      const std::vector<unsigned char> &salt,
                                      unsigned int iterations,
                                      std::vector<unsigned char> &output,
                                      bool enforceLowerBoundChecks)
   {
      if (output.empty() || salt.empty() || iterations == 0)
         return false;

      EVP_KDF *kdf = EVP_KDF_fetch(nullptr, "PBKDF2", nullptr);

      if (kdf == nullptr)
         return false;

      EVP_KDF_CTX *context = EVP_KDF_CTX_new(kdf);
      EVP_KDF_free(kdf);

      if (context == nullptr)
         return false;

      char digest[] = "SHA256";
      uint64_t iterationsValue = iterations;

      // OSSL_KDF_PARAM_PKCS5 = 1 switches off OpenSSL's own SP800-132 floor
      // (iterations >= 1000, salt >= 16 bytes, key >= 14 bytes). Only the RFC 6070
      // test vectors need that - some of them fall below the floor - and only
      // PasswordHasherTester passes enforceLowerBoundChecks = false to get it. Every
      // production caller leaves the floor enabled, on top of the wider bounds
      // enforced at the COM boundary and clamped again in GetConfiguredIterations.
      int disableLowerBoundChecks = enforceLowerBoundChecks ? 0 : 1;

      OSSL_PARAM params[6];
      int index = 0;

      params[index++] = OSSL_PARAM_construct_int(OSSL_KDF_PARAM_PKCS5, &disableLowerBoundChecks);
      params[index++] = OSSL_PARAM_construct_utf8_string(OSSL_KDF_PARAM_DIGEST, digest, 0);
      params[index++] = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_PASSWORD,
         const_cast<unsigned char*>(GetBufferPointer(password)), password.size());
      params[index++] = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_SALT,
         const_cast<unsigned char*>(GetBufferPointer(salt)), salt.size());
      params[index++] = OSSL_PARAM_construct_uint64(OSSL_KDF_PARAM_ITER, &iterationsValue);
      params[index] = OSSL_PARAM_construct_end();

      int result = EVP_KDF_derive(context, &output[0], output.size(), params);

      EVP_KDF_CTX_free(context);

      return result == 1;
   }

   bool
   PasswordHasher::Parse_(const AnsiString &stored, HashParameters &parameters)
   {
      if (!IsPasswordHash(stored))
         return false;

      std::vector<AnsiString> fields = SplitFields(stored, '$');

      // The string starts with the separator, so the first field is always empty.
      if (fields.size() < 2 || fields[0].GetLength() != 0)
         return false;

      AnsiString identifier = fields[1];

      if (identifier == ARGON2ID_IDENTIFIER)
      {
         if (fields.size() != 6)
            return false;

         parameters.algorithm = AlgorithmArgon2id;

         unsigned int version = 0;

         if (!ParseNamedValue(fields[2], "v", version))
            return false;

         // Only version 19 is ever written, and only version 19 is accepted. Should
         // another version need supporting, DeriveArgon2id has to be handed the
         // value - it currently hard codes it.
         if (version != ARGON2_VERSION_13)
            return false;

         std::vector<AnsiString> costFields = SplitFields(fields[3], ',');

         if (costFields.size() != 3)
            return false;

         if (!ParseNamedValue(costFields[0], "m", parameters.memory_cost_kb))
            return false;

         if (!ParseNamedValue(costFields[1], "t", parameters.iterations))
            return false;

         if (!ParseNamedValue(costFields[2], "p", parameters.lanes))
            return false;

         if (parameters.lanes == 0 || parameters.iterations == 0 || parameters.memory_cost_kb == 0)
            return false;

         // ParseNamedValue otherwise accepts anything up to UINT_MAX, and Compute_
         // turns m= directly into an allocation size. Without a ceiling here, a
         // tampered or imported row would make every verification attempt try a
         // multi-terabyte allocation on a shared IOCP worker thread. The setter caps
         // new values at the same ceiling, so no legitimate hash exceeds it.
         if (parameters.memory_cost_kb > (unsigned int) MaxArgon2idMemoryCostKb)
            return false;

         if (!DecodeBase64_(fields[4], parameters.salt))
            return false;

         if (!DecodeBase64_(fields[5], parameters.hash))
            return false;
      }
      else if (identifier == PBKDF2_SHA256_IDENTIFIER)
      {
         if (fields.size() != 5)
            return false;

         parameters.algorithm = AlgorithmPBKDF2SHA256;
         parameters.lanes = 1;
         parameters.memory_cost_kb = 0;

         if (!ParseNamedValue(fields[2], "i", parameters.iterations))
            return false;

         if (parameters.iterations == 0)
            return false;

         if (!DecodeBase64_(fields[3], parameters.salt))
            return false;

         if (!DecodeBase64_(fields[4], parameters.hash))
            return false;
      }
      else
      {
         return false;
      }

      if (parameters.salt.empty() || parameters.hash.empty())
         return false;

      // Compute_ always derives exactly HashLength bytes, so a stored string whose
      // hash portion is any other length can never compare equal - Verify would fail
      // every time, for every password, with nothing to tell an administrator why.
      // Rejecting it here is what makes NeedsRehash report it as unusable rather than
      // as up to date. A salt of another length is a different matter: it is handed to
      // the KDF unchanged and still verifies, so that one is left to NeedsRehash.
      if (parameters.hash.size() != (size_t) HashLength)
         return false;

      return true;
   }

   AnsiString
   PasswordHasher::Format_(const HashParameters &parameters)
   {
      AnsiString salt = EncodeBase64_(parameters.salt);
      AnsiString hash = EncodeBase64_(parameters.hash);

      AnsiString result;

      // Built by concatenation rather than with AnsiString::Format. CStdStr::FormatV
      // sizes its buffer by handing the format string to _vsctprintf, which in this
      // build is the wide character variant - so a narrow format string is cast to
      // wchar_t* and the resulting length is nonsense. The comment on FormatV says
      // as much. Only the wide String::Format is safe to use.
      switch (parameters.algorithm)
      {
      case AlgorithmArgon2id:
         result = "$";
         result += ARGON2ID_IDENTIFIER;
         result += "$v=";
         result += ToAnsiString((unsigned int) ARGON2_VERSION_13);
         result += "$m=";
         result += ToAnsiString(parameters.memory_cost_kb);
         result += ",t=";
         result += ToAnsiString(parameters.iterations);
         result += ",p=";
         result += ToAnsiString(parameters.lanes);
         result += "$";
         result += salt;
         result += "$";
         result += hash;
         break;
      case AlgorithmPBKDF2SHA256:
         result = "$";
         result += PBKDF2_SHA256_IDENTIFIER;
         result += "$i=";
         result += ToAnsiString(parameters.iterations);
         result += "$";
         result += salt;
         result += "$";
         result += hash;
         break;
      default:
         return "";
      }

      return result;
   }

   bool
   PasswordHasher::GenerateSalt_(std::vector<unsigned char> &salt)
   {
      salt.resize((size_t) SaltLength);

      if (RAND_bytes(&salt[0], (int) salt.size()) != 1)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5517, "PasswordHasher::GenerateSalt_",
            "Failed to generate a random salt.");

         salt.clear();
         return false;
      }

      return true;
   }

   std::vector<unsigned char>
   PasswordHasher::ToBytes_(const AnsiString &input)
   {
      std::vector<unsigned char> result;
      result.reserve((size_t) input.GetLength());

      for (int i = 0; i < input.GetLength(); i++)
         result.push_back((unsigned char) input.GetAt(i));

      return result;
   }

   AnsiString
   PasswordHasher::EncodeBase64_(const std::vector<unsigned char> &input)
   {
      if (input.empty())
         return "";

      return Base64::EncodeUnpadded((const char*) &input[0], (int) input.size());
   }

   bool
   PasswordHasher::DecodeBase64_(const AnsiString &input, std::vector<unsigned char> &output)
   {
      AnsiString decoded;

      if (!Base64::DecodeUnpadded(input, decoded))
      {
         output.clear();
         return false;
      }

      output = ToBytes_(decoded);
      return true;
   }

   void
   PasswordHasherTester::Test()
   {
      // Known answer test from RFC 9106, section 5.3.
      {
         std::vector<unsigned char> password(32, 0x01);
         std::vector<unsigned char> salt(16, 0x02);
         std::vector<unsigned char> secret(8, 0x03);
         std::vector<unsigned char> associatedData(12, 0x04);
         std::vector<unsigned char> output(32);

         if (!PasswordHasher::DeriveArgon2id(password, salt, secret, associatedData, 4, 32, 3, output))
            throw std::logic_error("Argon2id derivation failed.");

         const unsigned char expected[] =
         {
            0x0d, 0x64, 0x0d, 0xf5, 0x8d, 0x78, 0x76, 0x6c, 0x08, 0xc0, 0x37, 0xa3, 0x4a, 0x8b, 0x53, 0xc9,
            0xd0, 0x1e, 0xf0, 0x45, 0x2d, 0x75, 0xb6, 0x5e, 0xb5, 0x25, 0x20, 0xe9, 0x6b, 0x01, 0xe6, 0x59
         };

         if (memcmp(&output[0], expected, sizeof(expected)) != 0)
            throw std::logic_error("Argon2id produced an unexpected result for the RFC 9106 test vector.");
      }

      // Known answer tests for PBKDF2-HMAC-SHA256, using the inputs from RFC 6070.
      {
         struct Pbkdf2TestVector
         {
            const char *password;
            const char *salt;
            unsigned int iterations;
            size_t outputLength;
            const char *expected;
         };

         const Pbkdf2TestVector testVectors[] =
         {
            { "password", "salt", 1, 32,
              "120fb6cffcf8b32c43e7225256c4f837a86548c92ccc35480805987cb70be17b" },
            { "password", "salt", 2, 32,
              "ae4d0c95af6b46d32d0adff928f06dd02a303f8ef3c251dfd6e2d85a95474c43" },
            { "password", "salt", 4096, 32,
              "c5e478d59288c841aa530db6845c4c8d962893a001ce4e11a4963873aa98134a" },
            { "passwordPASSWORDpassword", "saltSALTsaltSALTsaltSALTsaltSALTsalt", 4096, 40,
              "348c89dbcbd32b2f32d814b8116e84cf2b17347ebc1800181c4e2a1fb8dd53e1c635518c7dac47e9" },
         };

         for (const Pbkdf2TestVector &testVector : testVectors)
         {
            std::vector<unsigned char> password(testVector.password, testVector.password + strlen(testVector.password));
            std::vector<unsigned char> salt(testVector.salt, testVector.salt + strlen(testVector.salt));
            std::vector<unsigned char> output(testVector.outputLength);

            // These vectors use iteration counts and salt lengths below OpenSSL's own
            // SP800-132 floor, so the floor has to be turned off to run them.
            if (!PasswordHasher::DerivePBKDF2SHA256(password, salt, testVector.iterations, output, false))
               throw std::logic_error("PBKDF2-SHA256 derivation failed.");

            AnsiString actual;
            for (size_t i = 0; i < output.size(); i++)
            {
               char buffer[3];
               sprintf_s(buffer, 3, "%02x", output[i]);
               actual += buffer;
            }

            if (actual != testVector.expected)
               throw std::logic_error("PBKDF2-SHA256 produced an unexpected result for a RFC 6070 test vector.");
         }
      }

      // Verification of complete PHC strings, one per supported algorithm. These
      // cover the parsing and the base64 decoding as well as the derivation.
      {
         const char *storedHashes[] =
         {
            "$argon2id$v=19$m=64,t=1,p=1$MDEyMzQ1Njc4OWFiY2RlZg$iVKEwm6AiziWdVGSRxFEXum0G7Sul+rkFiuuw+jYw+0",
            "$pbkdf2-sha256$i=1000$MDEyMzQ1Njc4OWFiY2RlZg$2cHIiEcjumYYlfmFN7/yQBDxY+WwFDBLOXS/yZXvBlw",
         };

         for (const char *storedHash : storedHashes)
         {
            if (!PasswordHasher::Verify("hMailServer", storedHash))
               throw std::logic_error("A known password could not be verified against its stored hash.");

            if (PasswordHasher::Verify("hmailserver", storedHash))
               throw std::logic_error("An incorrect password was verified successfully.");
         }
      }

      // Malformed strings must not verify, and must not be mistaken for valid hashes.
      {
         const char *malformedHashes[] =
         {
            "",
            "5f4dcc3b5aa765d61d8327deb882cf99",
            "$argon2id$v=19$m=64,t=1$MDEyMzQ1Njc4OWFiY2RlZg$iVKEwm6AiziWdVGSRxFEXum0G7Sul+rkFiuuw+jYw+0",
            "$argon2id$v=16$m=64,t=1,p=1$MDEyMzQ1Njc4OWFiY2RlZg$iVKEwm6AiziWdVGSRxFEXum0G7Sul+rkFiuuw+jYw+0",
            "$scrypt$ln=16,r=8,p=1$MDEyMzQ1Njc4OWFiY2RlZg$iVKEwm6AiziWdVGSRxFEXum0G7Sul+rkFiuuw+jYw+0",
            "$pbkdf2-sha256$i=1000$MDEyMzQ1Njc4OWFiY2Rl*g$2cHIiEcjumYYlfmFN7/yQBDxY+WwFDBLOXS/yZXvBlw",
            // An absurd m= must be rejected by Parse_ rather than handed to Compute_ -
            // without a ceiling, this would make every verification attempt against a
            // tampered or imported row try a multi-terabyte allocation.
            "$argon2id$v=19$m=4294967295,t=1,p=1$MDEyMzQ1Njc4OWFiY2RlZg$iVKEwm6AiziWdVGSRxFEXum0G7Sul+rkFiuuw+jYw+0",
            // A hash portion of any length other than HashLength - imported from a
            // differently configured implementation, or written by a version of this
            // code that derived a different number of bytes. Compute_ always derives
            // exactly HashLength bytes, so no password can ever verify against these.
            // They have to be reported as needing a rehash; reporting them as current
            // would leave the account permanently unable to log in, with no path back.
            "$argon2id$v=19$m=64,t=1,p=1$MDEyMzQ1Njc4OWFiY2RlZg$AAECAwQFBgcICQoLDA0ODxAREhMUFRYXGBkaGxwdHh8gISIjJCUmJygpKissLS4v",
            "$argon2id$v=19$m=64,t=1,p=1$MDEyMzQ1Njc4OWFiY2RlZg$AAECAwQFBgcICQoLDA0ODw",
            "$pbkdf2-sha256$i=1000$MDEyMzQ1Njc4OWFiY2RlZg$AAECAwQFBgcICQoLDA0ODxAREhMUFRYXGBkaGxwdHh8gISIjJCUmJygpKissLS4v",
            "$pbkdf2-sha256$i=1000$MDEyMzQ1Njc4OWFiY2RlZg$AAECAwQFBgcICQoLDA0ODw",
         };

         for (const char *malformedHash : malformedHashes)
         {
            if (PasswordHasher::Verify("hMailServer", malformedHash))
               throw std::logic_error("A malformed hash was verified successfully.");

            if (!PasswordHasher::NeedsRehash(malformedHash))
               throw std::logic_error("A hash we cannot make sense of was not reported as needing a rehash.");
         }
      }

      // A stored hash whose cost differs from the configured one must be rehashed
      // whichever way the difference goes. Lowering the cost is as deliberate a
      // decision as raising it, so a stronger-than-configured hash is not left alone.
      {
         // Sixteen salt bytes and thirty two hash bytes, so that only the cost
         // differs from what NeedsRehash expects. The hash is never derived here.
         const AnsiString salt = "MDEyMzQ1Njc4OWFiY2RlZg";
         const AnsiString hash = "iVKEwm6AiziWdVGSRxFEXum0G7Sul+rkFiuuw+jYw+0";

         AnsiString strongerThanConfigured;

         if (PasswordHasher::GetConfiguredAlgorithm() == PasswordHasher::AlgorithmArgon2id)
         {
            strongerThanConfigured = "$argon2id$v=19$m=";
            strongerThanConfigured += ToAnsiString(PasswordHasher::GetConfiguredMemoryCostKb() + 1024);
            strongerThanConfigured += ",t=";
            strongerThanConfigured += ToAnsiString(PasswordHasher::GetConfiguredIterations() + 1);
            strongerThanConfigured += ",p=1$";
         }
         else
         {
            strongerThanConfigured = "$pbkdf2-sha256$i=";
            strongerThanConfigured += ToAnsiString(PasswordHasher::GetConfiguredIterations() + 1);
            strongerThanConfigured += "$";
         }

         strongerThanConfigured += salt;
         strongerThanConfigured += "$";
         strongerThanConfigured += hash;

         if (!PasswordHasher::NeedsRehash(strongerThanConfigured))
            throw std::logic_error("A hash created with a cost other than the configured one was not reported as needing a rehash.");
      }

      // Round trip using the configured algorithm and cost.
      {
         AnsiString password = "The quick brown fox jumps over the lazy dog";

         AnsiString hash = PasswordHasher::Hash(password);

         if (hash.GetLength() == 0)
            throw std::logic_error("Hashing the password failed.");

         if (!PasswordHasher::IsPasswordHash(hash))
            throw std::logic_error("The generated hash was not recognized as a password hash.");

         if (!PasswordHasher::Verify(password, hash))
            throw std::logic_error("The password could not be verified against its own hash.");

         if (PasswordHasher::Verify("The quick brown fox jumps over the lazy dot", hash))
            throw std::logic_error("An incorrect password was verified successfully.");

         // A freshly created hash must never ask to be rehashed. If it does, every
         // single login results in a database write.
         if (PasswordHasher::NeedsRehash(hash))
            throw std::logic_error("A newly created hash was reported as needing a rehash.");

         // The same password hashed twice must yield different hashes.
         if (PasswordHasher::Hash(password) == hash)
            throw std::logic_error("Two hashes of the same password were identical.");
      }
   }
}
