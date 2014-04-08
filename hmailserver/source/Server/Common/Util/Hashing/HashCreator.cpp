// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include <stdafx.h>

#include "../PasswordGenerator.h"
#include "../../Mime/MimeCode.h"

#include <openssl/sha.h>
#include <openssl/md5.h>
#include <openssl/evp.h>

#include "HashCreator.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   HashCreator::HashCreator(HashCreator::HashType hashType) :
      _hashType(hashType)
   {

   }

   AnsiString HashCreator::GenerateHash(const AnsiString &inputString, const AnsiString &salt)
   {
      AnsiString saltString = salt;
      if (saltString.GetLength() == 0 && _hashType == SHA256)
      {     
         AnsiString randomString = PasswordGenerator::Generate();
         saltString = _GetHash(randomString, hex);
         saltString = saltString.Mid(0, SALT_LENGTH);
      }

      AnsiString value = saltString + _GetHash(saltString + inputString, hex);
      return value;
   }

   AnsiString 
   HashCreator::GenerateHashNoSalt(const AnsiString &inputString, RequestedEncoding encoding)
   {
      return _GetHash(inputString, encoding);
   }

   AnsiString 
   HashCreator::GenerateHashNoSalt(unsigned char *input, int inputLength, RequestedEncoding encoding)
   {
      return _GetHashRaw(input, inputLength, encoding);
   }


   bool 
   HashCreator::ValidateHash(const AnsiString &password, const AnsiString &originalHash, bool useSalt)
   {
      if (useSalt)
      {
         AnsiString salt = _GetSalt(originalHash);
         AnsiString result = GenerateHash(password, salt);

         if (result == originalHash)
            return true;
         else
            return false;
      }
      else
      {
         AnsiString result = _GetHash(password, hex);

         if (result == originalHash)
            return true;
         else
            return false;
      }


   }

   AnsiString HashCreator::_GetSalt(const AnsiString &inputString)
   {
      AnsiString result = inputString.Mid(0,SALT_LENGTH);
      return result;
   }

   AnsiString HashCreator::_GetHash(const AnsiString &sInputString, HashCreator::RequestedEncoding encoding)
   {
      AnsiString temp = sInputString;
      return _GetHashRaw((unsigned char*) temp.GetBuffer(), temp.GetLength(), encoding);
   }

   AnsiString HashCreator::_GetHashRaw(const unsigned char *input, int inputLength, HashCreator::RequestedEncoding encoding)
   {
      int digestLength = 0;

      switch (_hashType)
      {
      case SHA1:
         digestLength = SHA_DIGEST_LENGTH;
         break;
      case SHA256:
         digestLength = SHA256_DIGEST_LENGTH;
         break;
      case MD5:
         digestLength = MD5_DIGEST_LENGTH;
         break;
      }

      unsigned char *results = new unsigned char[digestLength];

      switch (_hashType)
      {
      case SHA1:
         {
            SHA_CTX context;
            SHA1_Init(&context);
            SHA1_Update(&context, input, inputLength);
            SHA1_Final(results, &context);
            break;
         }
      case MD5:
         {
            MD5_CTX context;
            MD5_Init(&context);
            MD5_Update(&context, input, inputLength);
            MD5_Final(results, &context);
            break;
         }
      case SHA256:
         {
            SHA256_CTX context;
            SHA256_Init(&context);
            SHA256_Update(&context, input, inputLength);
            SHA256_Final(results, &context);
            break;
         }

      }


      HM::String retVal;
      if (encoding == hex)
      {
         char *s = new char[digestLength*2+1];

         for (int i=0; i<digestLength; i++)
            sprintf(s+i*2, "%02x", results[i]);

         s[digestLength*2]='\0';

         retVal = s;

         delete [] s;
      }
      else if (encoding == base64)
      {
         MimeCodeBase64 encoder;
         encoder.SetInput((const char*) results, digestLength, true);

         AnsiString sEncodedValue;
         encoder.GetOutput(sEncodedValue);

         retVal = sEncodedValue;
         retVal = retVal.Mid(0, retVal.GetLength()-2);
      }

      delete [] results;

      return retVal;
   }

   void
   HashCreatorTester::Test()
   {
      // Run basic test.
      HashCreator hasher(HashCreator::SHA256);
      AnsiString result = hasher.GenerateHash("The quick brown fox jumps over the lazy dog", "");

      if (!hasher.ValidateHash("The quick brown fox jumps over the lazy dog", result, true))
         throw 0;

      // Check that same password hashed twice yealds separate hashes.
      AnsiString test1 = hasher.GenerateHash("The quick brown fox jumps over the lazy dog", "");
      AnsiString test2 = hasher.GenerateHash("The quick brown fox jumps over the lazy dog", "");
      if (test1 == test2)
         throw 0;

      for (int i = 0; i < 250; i++)
      {
         HashCreator memoryTester(HashCreator::SHA256);

         String temp;
         temp.Format(_T("%d"), i);
         AnsiString hashableString = temp;

         hasher.GenerateHash(hashableString, "test");
      }
   }
}