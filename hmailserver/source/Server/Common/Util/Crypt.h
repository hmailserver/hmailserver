// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class BlowFishEncryptor;
   
   class Crypt : public Singleton<Crypt>
   {
   public:
	   Crypt();
	   virtual ~Crypt();

      enum EncryptionType
      {
         ETNone = 0,
         ETBlowFish = 1,
         ETMD5 = 2,
         ETSHA256 = 3,
         ETPHC = 4
      };
      // ETPHC means "the stored string describes itself" - see PasswordHasher.
      // Adding another key derivation function later does not require a new
      // value here, nor a database migration.

      EncryptionType GetHashType(const String &hash);

      String EnCrypt(const String &sInput, EncryptionType iType) const;
      String DeCrypt(const String &sInput, EncryptionType iType) const;

      bool Validate(const String &password, const String &originalHash, EncryptionType iType) const;

   private:

      BlowFishEncryptor *blow_fish_;

   };
}
