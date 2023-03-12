// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "Canonicalization.h"
#include "../../Util/Hashing/HashCreator.h"

namespace HM
{
   class Message;
   class MessageData;
   class MimeHeader;
   class DKIMParameters;
   class MimeField;

   class DKIM
   {
   public:
      DKIM();

      static void Initialize();

      enum Result
      {
         Neutral = 0,
         Pass = 1,
         TempFail = 2,
         PermFail = 3
      };

      enum Settings
      {
         // Limit signing of huge messages, to prevent memory/perforamnce issues.
         MaxFileSize = 1024 * 1024 * 50
      };

      bool Sign(std::shared_ptr<Message> message, 
                const AnsiString &domain,
                const AnsiString &selector, 
                const String &privateKey, 
                HashCreator::HashType algorithm, 
                Canonicalization::CanonicalizeMethod headerMethod, 
                Canonicalization::CanonicalizeMethod bodyMethod);

      Result Verify(const String &messageFile);

   private:

      bool ValidateHeaderContents_(const DKIMParameters &signatureParams);
      bool ValidateBodyHash_(const String &fileName, const DKIMParameters &signatureParams, std::shared_ptr<Canonicalization> canonicalization);
      bool ValidateDNSEntry_(const DKIMParameters &entryParams, const DKIMParameters &headerParams);
      Result VerifyHeaderHash_(AnsiString canonicalizedHeader, const AnsiString &tagA, AnsiString &tagB, const AnsiString &publicKeyString);
      Result VerifySignature_(const String &fileName, const AnsiString &messageHeader, std::pair<AnsiString, AnsiString> signatureField);
      Result RetrievePublicKey_(const DKIMParameters &signatureParams, AnsiString &publicKey, AnsiString &flags);
      AnsiString GetDKIMWithoutSignature_(AnsiString value);
     
      String BuildSignatureHeader_(const String &tagA, const String &tagD, const String &tagS, const String &tagC, const String &tagQ, const String &fieldList, const String &bodyHash, const String &signatureString);
      std::shared_ptr<Canonicalization> CreateCanonicalization_(Canonicalization::CanonicalizeMethod method);
      AnsiString SignHash_(AnsiString &privateKey, AnsiString &canonicalizedHeader, HashCreator::HashType keySize);
      static std::vector<AnsiString> recommendedHeaderFields_;

      std::vector<std::pair<AnsiString, AnsiString> > GetSignatureFields(MimeHeader &mimeHeader);
   };

}