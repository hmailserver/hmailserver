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

      const char BASE64_ALPHABET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

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
      parameters.version = ARGON2_VERSION_13;
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

      if (parameters.iterations < GetConfiguredIterations())
         return true;

      if (parameters.algorithm == AlgorithmArgon2id)
      {
         if (parameters.memory_cost_kb < GetConfiguredMemoryCostKb())
            return true;

         if (parameters.lanes != 1)
            return true;
      }

      if (parameters.salt.size() < (size_t) MinimumSaltLength)
         return true;

      if (parameters.hash.size() < (size_t) HashLength)
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

      return (unsigned int) memoryCost;
   }

   unsigned int
   PasswordHasher::GetConfiguredIterations()
   {
      int iterations = Configuration::Instance()->GetPasswordHashIterations();

      if (iterations > 0)
         return (unsigned int) iterations;

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
         ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5516, "PasswordHasher::DeriveArgon2id",
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
                                      std::vector<unsigned char> &output)
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

      // Without this, OpenSSL rejects the parameter combinations used by the
      // published test vectors. Our own parameters are far above the lower bounds.
      int disableLowerBoundChecks = 1;

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

         if (!ParseNamedValue(fields[2], "v", parameters.version))
            return false;

         if (parameters.version != ARGON2_VERSION_13)
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
         parameters.version = 0;
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

      return true;
   }

   AnsiString
   PasswordHasher::Format_(const HashParameters &parameters)
   {
      AnsiString salt = EncodeBase64_(parameters.salt);
      AnsiString hash = EncodeBase64_(parameters.hash);

      AnsiString result;

      switch (parameters.algorithm)
      {
      case AlgorithmArgon2id:
         result.Format("$%s$v=%u$m=%u,t=%u,p=%u$%s$%s",
            ARGON2ID_IDENTIFIER,
            parameters.version,
            parameters.memory_cost_kb,
            parameters.iterations,
            parameters.lanes,
            salt.c_str(),
            hash.c_str());
         break;
      case AlgorithmPBKDF2SHA256:
         result.Format("$%s$i=%u$%s$%s",
            PBKDF2_SHA256_IDENTIFIER,
            parameters.iterations,
            salt.c_str(),
            hash.c_str());
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
      // The PHC string format uses the standard base64 alphabet without padding.
      AnsiString result;

      size_t index = 0;

      while (index < input.size())
      {
         size_t remaining = input.size() - index;

         unsigned int block = ((unsigned int) input[index]) << 16;

         if (remaining > 1)
            block |= ((unsigned int) input[index + 1]) << 8;
         if (remaining > 2)
            block |= (unsigned int) input[index + 2];

         result += BASE64_ALPHABET[(block >> 18) & 0x3f];
         result += BASE64_ALPHABET[(block >> 12) & 0x3f];

         if (remaining > 1)
            result += BASE64_ALPHABET[(block >> 6) & 0x3f];
         if (remaining > 2)
            result += BASE64_ALPHABET[block & 0x3f];

         index += 3;
      }

      return result;
   }

   bool
   PasswordHasher::DecodeBase64_(const AnsiString &input, std::vector<unsigned char> &output)
   {
      output.clear();

      unsigned int block = 0;
      int bitsInBlock = 0;

      for (int i = 0; i < input.GetLength(); i++)
      {
         char c = input.GetAt(i);

         const char *position = strchr(BASE64_ALPHABET, c);

         if (position == nullptr || c == '\0')
         {
            output.clear();
            return false;
         }

         block = (block << 6) | (unsigned int) (position - BASE64_ALPHABET);
         bitsInBlock += 6;

         if (bitsInBlock >= 8)
         {
            bitsInBlock -= 8;
            output.push_back((unsigned char) ((block >> bitsInBlock) & 0xff));
         }
      }

      // Any bits left over must be zero padding, and there can never be a full
      // byte of them.
      if (bitsInBlock >= 6 || (block & ((1u << bitsInBlock) - 1u)) != 0)
      {
         output.clear();
         return false;
      }

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

            if (!PasswordHasher::DerivePBKDF2SHA256(password, salt, testVector.iterations, output))
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
         };

         for (const char *malformedHash : malformedHashes)
         {
            if (PasswordHasher::Verify("hMailServer", malformedHash))
               throw std::logic_error("A malformed hash was verified successfully.");

            if (!PasswordHasher::NeedsRehash(malformedHash))
               throw std::logic_error("A hash we cannot make sense of was not reported as needing a rehash.");
         }
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
