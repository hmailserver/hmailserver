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
         // Limit signing of huge messages, to prevent memory/performance issues.
         MaxFileSize = 1024 * 1024 * 50,
         // Limit the number of signatures we verify in a single message.
         MaxSignatureCount = 5
      };

      bool Sign(std::shared_ptr<Message> message,
                const AnsiString &header,
                const AnsiString &domain,
                const AnsiString &selector,
                const String &privateKey,
                HashCreator::HashType algorithm,
                Canonicalization::CanonicalizeMethod headerMethod,
                Canonicalization::CanonicalizeMethod bodyMethod);

      Result Verify(const String &messageFile);

      // Verifies every signature in the message. signatureResults is filled with the
      // d= domain and result of each signature, which DMARC needs for alignment.
      Result Verify(const String &messageFile, std::vector<std::pair<AnsiString, Result> > &signatureResults);

      // Building blocks shared with ARC (RFC 8617). ARC-Message-Signature and ARC-Seal
      // use the same canonicalization, key records and signatures as DKIM-Signature,
      // but a different field name, i= in place of v=, and i= is not an identity.

      // Builds a signed field value for fieldName. leadingTags opens the value, for
      // example v=1. Returns an empty string on failure.
      static String CreateSignature(const AnsiString &fieldName,
                                    const String &leadingTags,
                                    const AnsiString &header,
                                    const String &messageFile,
                                    const AnsiString &domain,
                                    const AnsiString &selector,
                                    const AnsiString &privateKeyContent,
                                    HashCreator::HashType algorithm,
                                    Canonicalization::CanonicalizeMethod headerMethod,
                                    Canonicalization::CanonicalizeMethod bodyMethod,
                                    const std::vector<AnsiString> &headerFields);

      // Verifies one signature field: key lookup, body hash and header signature.
      // Checks of tags specific to the field type are left to the caller. auid is the
      // signing identity (DKIM's i=), empty if there is none.
      static Result VerifySignatureField(const String &messageFile,
                                         const AnsiString &messageHeader,
                                         const std::pair<AnsiString, AnsiString> &signatureField,
                                         const DKIMParameters &signatureParams,
                                         const AnsiString &auid,
                                         bool &testMode);

      static std::shared_ptr<Canonicalization> CreateCanonicalization(Canonicalization::CanonicalizeMethod method);

      // Parses a c= tag such as relaxed/simple. A missing part defaults to simple.
      static void ParseCanonicalizationTag(const AnsiString &tagC,
                                           std::shared_ptr<Canonicalization> &headerCanonicalization,
                                           std::shared_ptr<Canonicalization> &bodyCanonicalization);

      static AnsiString SignHash(const AnsiString &privateKey, const AnsiString &canonicalizedHeader, HashCreator::HashType hashType);
      static Result VerifyHash(const AnsiString &canonicalizedHeader, const AnsiString &tagA, const AnsiString &tagB, const AnsiString &publicKeyString);
      static Result RetrievePublicKey(const AnsiString &domain, const AnsiString &selector, const AnsiString &tagA, const AnsiString &auid, AnsiString &publicKey, AnsiString &flags);

      // Returns up to maxCount fields named fieldName, in header order.
      static std::vector<std::pair<AnsiString, AnsiString> > GetSignatureFields(MimeHeader &mimeHeader, const AnsiString &fieldName, size_t maxCount);

      static String BuildSignatureHeader(const String &leadingTags, const String &tagA, const String &tagD, const String &tagS, const String &tagC, const String &tagQ, const String &fieldList, const String &bodyHash, const String &signatureString);

   private:

      bool ValidateHeaderContents_(const DKIMParameters &signatureParams);
      static bool ValidateBodyHash_(const String &fileName, const DKIMParameters &signatureParams, std::shared_ptr<Canonicalization> canonicalization);
      static bool ValidateDNSEntry_(const DKIMParameters &entryParams, const AnsiString &tagA, const AnsiString &auid);
      Result VerifySignature_(const String &fileName, const AnsiString &messageHeader, std::pair<AnsiString, AnsiString> signatureField);
      AnsiString GetSignatureDomain_(AnsiString headerValue);

      bool HasSignatureForDomain_(MimeHeader &mimeHeader, const AnsiString &domain);
      static std::vector<AnsiString> recommendedHeaderFields_;
   };

}