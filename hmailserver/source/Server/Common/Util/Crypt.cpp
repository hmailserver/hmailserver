// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "Crypt.h"
#include "BlowFish.h"
#include "Hashing/HashCreator.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   Crypt::Crypt()
   {
      blow_fish_ = new BlowFishEncryptor();
   }  

   Crypt::~Crypt()
   {
      delete blow_fish_;
   }

   String
   Crypt::EnCrypt(const String &sInput,EncryptionType iType) const
   {
      switch (iType)
      {
      case ETNone:
         return sInput;
      case ETBlowFish:
         {
            if (sInput.IsEmpty())
               return "";

            return blow_fish_->EncryptToString(sInput);
         }
      case ETMD5:
         {
            HashCreator crypter(HashCreator::MD5);
            String sResult = crypter.GenerateHashNoSalt(sInput, HashCreator::hex);
            return sResult;
         }
      case ETSHA256:
         {
            HashCreator encrypter(HashCreator::SHA256);
            AnsiString result = encrypter.GenerateHash(sInput, "");
            return result;
         }
      default:
         {
            assert(0);
         }
      }

      return "";

   }

   bool
   Crypt::Validate(const String &password, const String &originalHash, EncryptionType iType) const
   {
      switch (iType)
      {
      case ETMD5:
         {
            // Salts are not used for the MD5 hashes.
            HashCreator encrypter(HashCreator::MD5);
            bool result = encrypter.ValidateHash(password, originalHash, false);
            return result;
         }
      case ETSHA256:
         {
            // Salts are always used for the SHA256 hashes.
            HashCreator encrypter(HashCreator::SHA256);
            bool result = encrypter.ValidateHash(password, originalHash, true);
            return result;
         }
      default:
         {
            assert(0);
         }
      }

      return false;

   }

   Crypt::EncryptionType 
   Crypt::GetHashType(const String &hash)
   {
      int length = hash.GetLength();
      if (length == 32)
         return ETMD5;
      else if (length == 70)
         return ETSHA256;
      else
         return ETNone;
   }

   String
   Crypt::DeCrypt(const String &sInput, EncryptionType iType) const
   {
      switch (iType)
      {
         case ETNone:
            return sInput;
         case ETBlowFish:
            {
               if (sInput.IsEmpty())
                  return "";

               return blow_fish_->DecryptFromString(sInput);
            }
            break;
         default:
            assert(0);
      }
      
      return "";
   }
}
