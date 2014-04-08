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
         MaxFileSize = 1024 * 1024 * 10
      };

      bool Sign(shared_ptr<Message> message, 
                const AnsiString &domain,
                const AnsiString &selector, 
                const String &privateKey, 
                HashCreator::HashType algorithm, 
                Canonicalization::CanonicalizeMethod headerMethod, 
                Canonicalization::CanonicalizeMethod bodyMethod);

      Result Verify(const String &messageFile);

   private:

      bool _ValidateHeaderContents(const DKIMParameters &signatureParams);
      bool _ValidateBodyHash(const String &fileName, const DKIMParameters &signatureParams, shared_ptr<Canonicalization> canonicalization);
      bool _ValidateDNSEntry(const DKIMParameters &entryParams, const DKIMParameters &headerParams);
      Result _VerifyHeaderHash(AnsiString canonicalizedHeader, const AnsiString &tagA, AnsiString &tagB, const AnsiString &publicKeyString);
      Result _VerifySignature(const String &fileName, const AnsiString &messageHeader, pair<AnsiString, AnsiString> signatureField);
      Result _RetrievePublicKey(const DKIMParameters &signatureParams, AnsiString &publicKey, AnsiString &flags);
      AnsiString _GetDKIMWithoutSignature(AnsiString value);
     
      String _BuildSignatureHeader(const String &tagA, const String &tagD, const String &tagS, const String &tagC, const String &tagQ, const String &fieldList, const String &bodyHash, const String &signatureString);
      shared_ptr<Canonicalization> _CreateCanonicalization(Canonicalization::CanonicalizeMethod method);
      AnsiString _SignHash(AnsiString &privateKey, AnsiString &canonicalizedHeader, HashCreator::HashType keySize);
      static vector<AnsiString> _recommendedHeaderFields;

      vector<pair<AnsiString, AnsiString> > GetSignatureFields(MimeHeader &mimeHeader);
   };

}