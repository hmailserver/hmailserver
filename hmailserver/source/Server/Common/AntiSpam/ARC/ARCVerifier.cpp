// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "stdafx.h"

#include "ARCVerifier.h"

#include "../DKIM/Canonicalization.h"
#include "../DKIM/DKIMParameters.h"
#include "../DMARC/DMARCTxtLookup.h"

#include "../../Mime/Mime.h"
#include "../../Persistence/PersistentMessage.h"
#include "../../Util/FileUtilities.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   const char *ARCVerifier::AuthenticationResultsFieldName = "ARC-Authentication-Results";
   const char *ARCVerifier::MessageSignatureFieldName = "ARC-Message-Signature";
   const char *ARCVerifier::SealFieldName = "ARC-Seal";

   namespace
   {
      // Positions of the three fields within an ARC set.
      enum SetField
      {
         AuthenticationResultsField = 0,
         MessageSignatureField = 1,
         SealField = 2
      };

      DKIMParameters LoadParameters(AnsiString value)
      {
         MimeField::UnfoldField(value);

         DKIMParameters parameters;
         parameters.Load(value);

         return parameters;
      }
   }

   ARCVerifier::ARCVerifier(std::shared_ptr<DMARCTxtLookup> txtLookup) :
      txt_lookup_(txtLookup),
      highest_instance_(0)
   {

   }

   ARCVerifier::Result
   ARCVerifier::Verify(const String &messageFile)
   {
      highest_instance_ = 0;
      sealer_domain_ = "";
      authentication_results_ = "";
      failure_reason_ = "";

      if (FileUtilities::FileSize(messageFile) > DKIM::MaxFileSize)
      {
         LOG_DEBUG("ARC: The message is larger than the max size for ARC verification.");
         return Result::None;
      }

      AnsiString messageHeader = PersistentMessage::LoadHeader(messageFile);
      MimeHeader mimeHeader;
      mimeHeader.Load(messageHeader.GetBuffer(), messageHeader.GetLength(), false);

      std::vector<std::vector<std::pair<AnsiString, AnsiString> > > sets(MaxInstances);
      int fieldCount = 0;

      for (MimeField field : mimeHeader.Fields())
      {
         AnsiString name = field.GetName();

         int position;
         if (name.CompareNoCase(AuthenticationResultsFieldName) == 0)
            position = AuthenticationResultsField;
         else if (name.CompareNoCase(MessageSignatureFieldName) == 0)
            position = MessageSignatureField;
         else if (name.CompareNoCase(SealFieldName) == 0)
            position = SealField;
         else
            continue;

         fieldCount++;

         // Bounds the work a message can make us do.
         if (fieldCount > MaxInstances * 3)
            return Fail_("too many ARC header fields");

         AnsiString value = field.GetValue();
         int instance = GetInstance(name, value);

         if (instance == 0)
            return Fail_("ARC header field without a valid instance");

         std::vector<std::pair<AnsiString, AnsiString> > &set = sets[instance - 1];

         if (set.empty())
            set.resize(3);

         if (!set[position].first.IsEmpty())
            return Fail_("duplicate ARC header field");

         set[position] = std::make_pair(name, value);

         if (instance > highest_instance_)
            highest_instance_ = instance;
      }

      if (highest_instance_ == 0)
         return Result::None;

      sets.resize(highest_instance_);

      // Every instance from 1 to the highest must have a complete set.
      for (int instance = 1; instance <= highest_instance_; instance++)
      {
         const std::vector<std::pair<AnsiString, AnsiString> > &set = sets[instance - 1];

         if (set.empty() || set[AuthenticationResultsField].first.IsEmpty() ||
             set[MessageSignatureField].first.IsEmpty() || set[SealField].first.IsEmpty())
         {
            return Fail_("incomplete ARC set");
         }

         // The first seal has nothing to vouch for; every later one must say the chain passed.
         AnsiString chainValidation = LoadParameters(set[SealField].second).GetValue("cv");
         AnsiString expected = instance == 1 ? "none" : "pass";

         if (chainValidation.CompareNoCase(expected) != 0)
            return Fail_("unexpected cv= value in ARC-Seal");
      }

      const std::vector<std::pair<AnsiString, AnsiString> > &latestSet = sets[highest_instance_ - 1];

      if (!VerifyMessageSignature_(messageFile, messageHeader, latestSet[MessageSignatureField]))
         return Result::Fail;

      for (int instance = highest_instance_; instance >= 1; instance--)
      {
         if (!VerifySeal_(sets, instance))
            return Result::Fail;
      }

      sealer_domain_ = LoadParameters(latestSet[SealField].second).GetValue("d");

      // The value opens with the i= tag, followed by the authserv-id and the results.
      AnsiString results = latestSet[AuthenticationResultsField].second;
      MimeField::UnfoldField(results);
      int separator = results.Find(";");
      authentication_results_ = separator >= 0 ? results.Mid(separator + 1) : "";
      authentication_results_.Trim();

      LOG_DEBUG("ARC: The chain passed validation.");

      return Result::Pass;
   }

   bool
   ARCVerifier::VerifyMessageSignature_(const String &messageFile, const AnsiString &messageHeader, const std::pair<AnsiString, AnsiString> &field)
   {
      DKIMParameters parameters = LoadParameters(field.second);

      AnsiString tagA = parameters.GetValue("a");

      if (parameters.GetValue("b").IsEmpty() || parameters.GetValue("bh").IsEmpty() ||
          parameters.GetValue("d").IsEmpty() || parameters.GetValue("s").IsEmpty() ||
          parameters.GetValue("h").IsEmpty())
      {
         Fail_("ARC-Message-Signature is missing a required tag");
         return false;
      }

      // RFC 8617 section 4.1.2: rsa-sha256 is the only algorithm ARC allows.
      if (tagA != "rsa-sha256")
      {
         Fail_("unsupported algorithm in ARC-Message-Signature");
         return false;
      }

      // The seals are added after the message signature, so it can't cover them.
      for (AnsiString headerField : StringParser::SplitString(parameters.GetValue("h"), ":"))
      {
         headerField.Trim();

         if (headerField.CompareNoCase(SealFieldName) == 0)
         {
            Fail_("ARC-Message-Signature signs ARC-Seal");
            return false;
         }
      }

      AnsiString publicKey;
      if (RetrievePublicKey_(parameters.GetValue("d"), parameters.GetValue("s"), tagA, publicKey) != DKIM::Pass)
      {
         Fail_("no usable key for ARC-Message-Signature");
         return false;
      }

      if (DKIM::VerifySignatureWithKey(messageFile, messageHeader, field, parameters, publicKey) != DKIM::Pass)
      {
         Fail_("ARC-Message-Signature did not verify");
         return false;
      }

      return true;
   }

   bool
   ARCVerifier::VerifySeal_(const std::vector<std::vector<std::pair<AnsiString, AnsiString> > > &sets, int instance)
   {
      DKIMParameters parameters = LoadParameters(sets[instance - 1][SealField].second);

      AnsiString tagA = parameters.GetValue("a");

      if (parameters.GetValue("b").IsEmpty() || parameters.GetValue("d").IsEmpty() ||
          parameters.GetValue("s").IsEmpty())
      {
         Fail_("ARC-Seal is missing a required tag");
         return false;
      }

      if (tagA != "rsa-sha256")
      {
         Fail_("unsupported algorithm in ARC-Seal");
         return false;
      }

      // RFC 8617 section 4.1.3: a seal covers the ARC sets only, so h= is not allowed.
      if (parameters.GetIsSet("h"))
      {
         Fail_("ARC-Seal has an h= tag");
         return false;
      }

      AnsiString publicKey;
      if (RetrievePublicKey_(parameters.GetValue("d"), parameters.GetValue("s"), tagA, publicKey) != DKIM::Pass)
      {
         Fail_("no usable key for ARC-Seal");
         return false;
      }

      AnsiString canonicalized = CanonicalizeSealInput(sets, instance);

      if (DKIM::VerifyHash(canonicalized, tagA, parameters.GetValue("b"), publicKey) != DKIM::Pass)
      {
         Fail_("ARC-Seal did not verify");
         return false;
      }

      return true;
   }

   AnsiString
   ARCVerifier::CanonicalizeSealInput(const std::vector<std::vector<std::pair<AnsiString, AnsiString> > > &sets, int upToInstance)
   {
      RelaxedCanonicalization relaxed;

      AnsiString result;

      for (int instance = 1; instance <= upToInstance; instance++)
      {
         const std::vector<std::pair<AnsiString, AnsiString> > &set = sets[instance - 1];

         result += relaxed.CanonicalizeHeaderLine(set[AuthenticationResultsField].first, set[AuthenticationResultsField].second) + "\r\n";
         result += relaxed.CanonicalizeHeaderLine(set[MessageSignatureField].first, set[MessageSignatureField].second) + "\r\n";

         if (instance < upToInstance)
         {
            result += relaxed.CanonicalizeHeaderLine(set[SealField].first, set[SealField].second) + "\r\n";
         }
         else
         {
            // The seal being verified goes last, with b= empty and without a trailing CRLF,
            // as a DKIM-Signature does in its own hash.
            std::vector<AnsiString> noFields;
            AnsiString fieldList;
            result += relaxed.CanonicalizeHeader("", set[SealField], noFields, fieldList);
         }
      }

      return result;
   }

   int
   ARCVerifier::GetInstance(const AnsiString &fieldName, const AnsiString &fieldValue)
   {
      AnsiString instanceText;

      if (fieldName.CompareNoCase(AuthenticationResultsFieldName) == 0)
      {
         // Not a tag list: the value is i=, then the authserv-id and the results.
         AnsiString value = fieldValue;
         MimeField::UnfoldField(value);
         value.Trim();

         int separator = value.Find(";");
         AnsiString tag = separator >= 0 ? value.Mid(0, separator) : value;

         int equals = tag.Find("=");
         if (equals < 0)
            return 0;

         AnsiString tagName = tag.Mid(0, equals);
         tagName.Trim();

         if (tagName != "i")
            return 0;

         instanceText = tag.Mid(equals + 1);
      }
      else
      {
         instanceText = LoadParameters(fieldValue).GetValue("i");
      }

      instanceText.Trim();

      if (instanceText.IsEmpty() || instanceText.GetLength() > 2 || !StringParser::IsNumeric(instanceText))
         return 0;

      int instance = atoi(instanceText);

      if (instance < 1 || instance > MaxInstances)
         return 0;

      return instance;
   }

   DKIM::Result
   ARCVerifier::RetrievePublicKey_(const AnsiString &domain, const AnsiString &selector, const AnsiString &tagA, AnsiString &publicKey)
   {
      std::vector<String> records;

      if (!txt_lookup_->GetTXTRecords(String(selector + "._domainkey." + domain), records))
         return DKIM::TempFail;

      // ARC has no signing identity, so there is no i= to check against the key record.
      AnsiString flags;
      return DKIM::ParsePublicKeyRecords(records, domain, tagA, "", publicKey, flags);
   }

   ARCVerifier::Result
   ARCVerifier::Fail_(const String &reason)
   {
      failure_reason_ = reason;

      LOG_DEBUG("ARC: The chain failed validation: " + reason);

      return Result::Fail;
   }

   int
   ARCVerifier::GetHighestInstance() const
   {
      return highest_instance_;
   }

   AnsiString
   ARCVerifier::GetSealerDomain() const
   {
      return sealer_domain_;
   }

   AnsiString
   ARCVerifier::GetAuthenticationResults() const
   {
      return authentication_results_;
   }

   String
   ARCVerifier::GetFailureReason() const
   {
      return failure_reason_;
   }
}
