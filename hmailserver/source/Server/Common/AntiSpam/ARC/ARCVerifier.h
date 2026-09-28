// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

#include "../DKIM/DKIM.h"

namespace HM
{
   class DMARCTxtLookup;

   // Validates the Authenticated Received Chain of a message. See RFC 8617, section 5.2.
   class ARCVerifier
   {
   public:

      enum class Result
      {
         None = 0,
         Pass = 1,
         Fail = 2
      };

      enum Settings
      {
         // RFC 8617 section 4.2.1: instance values run from 1 to 50.
         MaxInstances = 50
      };

      // The lookup is used for the public keys, so that tests don't depend on DNS.
      explicit ARCVerifier(std::shared_ptr<DMARCTxtLookup> txtLookup);

      Result Verify(const String &messageFile);

      // The values below describe the last Verify.

      // The number of ARC sets on the message, 0 if there were none.
      int GetHighestInstance() const;

      // The d= of the ARC-Seal with the highest instance, i.e. the last sealer.
      AnsiString GetSealerDomain() const;

      // The ARC-Authentication-Results of the highest instance, without its i= tag.
      AnsiString GetAuthenticationResults() const;

      // Why the chain failed, for the log and the Authentication-Results comment.
      String GetFailureReason() const;

      static const char *AuthenticationResultsFieldName;
      static const char *MessageSignatureFieldName;
      static const char *SealFieldName;

      // The instance in an ARC field value, or 0 if it has no valid one.
      static int GetInstance(const AnsiString &fieldName, const AnsiString &fieldValue);

      // The input the ARC-Seal of instance upToInstance signs: every ARC set up to and
      // including that one, in instance order, relaxed, with that seal's b= left empty.
      // sets holds the AAR, AMS and seal of each instance, indexed by instance - 1.
      static AnsiString CanonicalizeSealInput(const std::vector<std::vector<std::pair<AnsiString, AnsiString> > > &sets, int upToInstance);

   private:

      Result Fail_(const String &reason);
      DKIM::Result RetrievePublicKey_(const AnsiString &domain, const AnsiString &selector, const AnsiString &tagA, AnsiString &publicKey);
      bool VerifyMessageSignature_(const String &messageFile, const AnsiString &messageHeader, const std::pair<AnsiString, AnsiString> &field);
      bool VerifySeal_(const std::vector<std::vector<std::pair<AnsiString, AnsiString> > > &sets, int instance);

      std::shared_ptr<DMARCTxtLookup> txt_lookup_;

      int highest_instance_;
      AnsiString sealer_domain_;
      AnsiString authentication_results_;
      String failure_reason_;
   };
}
