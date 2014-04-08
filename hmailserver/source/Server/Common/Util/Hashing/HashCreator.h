// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class HashCreator
   {
   public:
      enum HashType
      {
         SHA1 = 1,
         SHA256 = 2,
         MD5 = 3
      };

      enum RequestedEncoding
      {
         hex = 1,
         base64 = 2
      };

      HashCreator(HashType hashType);

      AnsiString GenerateHash(const AnsiString &inputString, const AnsiString &salt);
      bool ValidateHash(const AnsiString &password, const AnsiString &originalHash, bool useSalt);
      
      AnsiString GenerateHashNoSalt(const AnsiString &inputString, RequestedEncoding encoding);
      AnsiString GenerateHashNoSalt(unsigned char *input, int inputLength, RequestedEncoding encoding);


   private:
   
      AnsiString _GetSalt(const AnsiString &inputString);
      AnsiString _GetHash(const AnsiString &sInputString, RequestedEncoding encoding);
      AnsiString _GetHashRaw(const unsigned char *input, int inputLength, RequestedEncoding encoding);

      enum Sizes
      {
         SALT_LENGTH = 6
      };

      HashType _hashType;
   };

   class HashCreatorTester
   {
   public:
      void Test();
   };

}