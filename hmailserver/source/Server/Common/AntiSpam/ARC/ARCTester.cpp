// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "stdafx.h"

#include "ARCTester.h"

#include "ARCAuthenticationResults.h"
#include "ARCVerifier.h"

#include "../DKIM/DKIM.h"
#include "../DMARC/DMARCTxtLookup.h"

#include "../../Util/Encoding/Base64.h"
#include "../../Util/FileUtilities.h"

#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>

#include <stdexcept>

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   namespace
   {
      void Expect(bool condition, const char *description)
      {
         if (!condition)
         {
            assert(0);
            throw std::logic_error(std::string("ARC test failed: ") + description);
         }
      }

      // Returns canned TXT records, so that key lookups don't depend on DNS.
      class TestTxtLookup : public DMARCTxtLookup
      {
      public:

         void AddRecord(const String &domain, const String &record)
         {
            records_[domain].push_back(record);
         }

         virtual bool GetTXTRecords(const String &domain, std::vector<String> &records)
         {
            auto iter = records_.find(domain);

            if (iter != records_.end())
               records = iter->second;

            return true;
         }

      private:

         std::map<String, std::vector<String> > records_;
      };

      // Generated for each run, so that no private key is kept in the source.
      struct TestKey
      {
         AnsiString privateKeyPem;
         AnsiString publicKeyBase64;
      };

      TestKey GenerateKey()
      {
         TestKey result;

         EVP_PKEY *key = EVP_RSA_gen(2048);
         Expect(key != nullptr, "key generation");

         BIO *bio = BIO_new(BIO_s_mem());
         PEM_write_bio_PrivateKey(bio, key, nullptr, nullptr, 0, nullptr, nullptr);

         char *data = nullptr;
         long length = BIO_get_mem_data(bio, &data);
         result.privateKeyPem = AnsiString(std::string(data, static_cast<size_t>(length)).c_str());
         BIO_free(bio);

         unsigned char *der = nullptr;
         int derLength = i2d_PUBKEY(key, &der);
         result.publicKeyBase64 = Base64::Encode((const char*) der, derLength);
         OPENSSL_free(der);

         EVP_PKEY_free(key);

         return result;
      }

      // Builds a message and seals it, one ARC set at a time, as a chain of forwarders would.
      class ChainBuilder
      {
      public:

         ChainBuilder(const TestKey &key, const String &messageFile) :
            key_(key),
            message_file_(messageFile)
         {
            header = "From: Sender <sender@example.com>\r\n"
                     "To: recipient@example.net\r\n"
                     "Subject: ARC test\r\n"
                     "Date: Mon, 28 Sep 2026 10:00:00 +0000\r\n"
                     "Message-ID: <arc-test@example.com>\r\n";

            body = "Hello.\r\n";
         }

         void AddSet(const char *chainValidation)
         {
            int instance = static_cast<int>(sets_.size()) + 1;

            // Built by concatenation: CStdStr::Format sizes its buffer wrongly for narrow strings.
            AnsiString number = std::to_string(instance).c_str();

            AnsiString authenticationResults = "i=" + number + "; mx" + number + ".example.org; dkim=pass header.d=example.com; "
                                               "spf=pass smtp.mailfrom=sender@example.com; dmarc=pass header.from=example.com";

            Write();

            std::vector<AnsiString> signedFields = { "From", "To", "Subject", "Date", "Message-ID" };

            AnsiString leadingTags = "i=" + number;

            AnsiString messageSignature = DKIM::CreateSignature(ARCVerifier::MessageSignatureFieldName, String(leadingTags), header,
                                                                message_file_, "example.org", "arc", key_.privateKeyPem,
                                                                HashCreator::SHA256, Canonicalization::Relaxed,
                                                                Canonicalization::Relaxed, signedFields);
            Expect(!messageSignature.IsEmpty(), "creating the message signature");

            AnsiString unsignedSeal = "i=" + number + "; a=rsa-sha256; cv=" + AnsiString(chainValidation) +
                                      "; d=example.org; s=arc; t=1790000000; b=";

            std::vector<std::pair<AnsiString, AnsiString> > set;
            set.push_back(std::make_pair(AnsiString(ARCVerifier::AuthenticationResultsFieldName), authenticationResults));
            set.push_back(std::make_pair(AnsiString(ARCVerifier::MessageSignatureFieldName), messageSignature));
            set.push_back(std::make_pair(AnsiString(ARCVerifier::SealFieldName), unsignedSeal));
            sets_.push_back(set);

            AnsiString signature = DKIM::SignHash(key_.privateKeyPem, ARCVerifier::CanonicalizeSealInput(sets_, instance), HashCreator::SHA256);
            Expect(!signature.IsEmpty(), "signing the seal");

            AnsiString seal = unsignedSeal + signature;
            sets_.back()[2].second = seal;

            // Each forwarder adds its set on top of the header.
            header = "ARC-Seal: " + seal + "\r\n" +
                     "ARC-Message-Signature: " + messageSignature + "\r\n" +
                     "ARC-Authentication-Results: " + authenticationResults + "\r\n" +
                     header;
         }

         void Write()
         {
            FileUtilities::WriteToFile(message_file_, header + "\r\n" + body);
         }

         AnsiString header;
         AnsiString body;

      private:

         TestKey key_;
         String message_file_;
         std::vector<std::vector<std::pair<AnsiString, AnsiString> > > sets_;
      };
   }

   void
   ARCTester::Test()
   {
      TestInstanceParsing_();
      TestAuthenticationResults_();
      TestChainValidation_();
   }

   void
   ARCTester::TestInstanceParsing_()
   {
      Expect(ARCVerifier::GetInstance("ARC-Authentication-Results", "i=3; mx.example.org; none") == 3, "AAR instance");
      Expect(ARCVerifier::GetInstance("ARC-Authentication-Results", " i = 12 ; mx.example.org; none") == 12, "AAR instance with spaces");
      Expect(ARCVerifier::GetInstance("ARC-Authentication-Results", "mx.example.org; i=1; none") == 0, "AAR without leading i=");
      Expect(ARCVerifier::GetInstance("ARC-Seal", "a=rsa-sha256; i=7; cv=pass; d=example.org") == 7, "seal instance");
      Expect(ARCVerifier::GetInstance("ARC-Seal", "i=0; cv=none") == 0, "instance 0");
      Expect(ARCVerifier::GetInstance("ARC-Seal", "i=51; cv=pass") == 0, "instance above 50");
      Expect(ARCVerifier::GetInstance("ARC-Seal", "i=5x; cv=pass") == 0, "non-numeric instance");
      Expect(ARCVerifier::GetInstance("ARC-Message-Signature", "a=rsa-sha256; d=example.org") == 0, "missing instance");
   }

   void
   ARCTester::TestAuthenticationResults_()
   {
      AnsiString google = "mx.google.com; dkim=pass header.i=@example.com header.s=sel header.b=abc; "
                          "spf=pass (google.com: domain of x@example.com designates 192.0.2.1 as permitted sender) smtp.mailfrom=x@example.com; "
                          "dmarc=pass (p=NONE sp=NONE dis=NONE) header.from=example.com";

      auto relaxed = DMARCRecord::Alignment::Relaxed;
      auto strict = DMARCRecord::Alignment::Strict;

      Expect(ARCAuthenticationResults::ShowsAlignedPass(google, "example.com", relaxed, relaxed), "all methods pass");
      Expect(!ARCAuthenticationResults::ShowsAlignedPass(google, "example.net", relaxed, relaxed), "other From domain");

      Expect(ARCAuthenticationResults::ShowsAlignedPass("mx.example.org; dkim=pass header.d=example.com", "sub.example.com", relaxed, relaxed),
             "relaxed DKIM alignment");
      Expect(!ARCAuthenticationResults::ShowsAlignedPass("mx.example.org; dkim=pass header.d=example.com", "sub.example.com", relaxed, strict),
             "strict DKIM alignment");
      Expect(ARCAuthenticationResults::ShowsAlignedPass("mx.example.org; spf=pass smtp.mailfrom=example.com", "example.com", relaxed, relaxed),
             "SPF domain without local part");
      Expect(!ARCAuthenticationResults::ShowsAlignedPass("mx.example.org; dkim=fail header.d=example.com; spf=softfail smtp.mailfrom=a@example.com",
                                                         "example.com", relaxed, relaxed),
             "failing methods");
      Expect(!ARCAuthenticationResults::ShowsAlignedPass("mx.example.org; spf=fail (dkim=pass header.d=example.com) smtp.mailfrom=a@example.net",
                                                         "example.com", relaxed, relaxed),
             "a pass inside a comment");
      Expect(ARCAuthenticationResults::ShowsAlignedPass("mx.example.org; dmarc=pass header.from=\"example.com\"", "example.com", relaxed, relaxed),
             "quoted value");
      Expect(ARCAuthenticationResults::ShowsAlignedPass("mx.example.org; DKIM=PASS header.d=EXAMPLE.COM", "example.com", strict, strict),
             "case differences");
      Expect(!ARCAuthenticationResults::ShowsAlignedPass("mx.example.org; none", "example.com", relaxed, relaxed), "no results");
   }

   void
   ARCTester::TestChainValidation_()
   {
      TestKey key = GenerateKey();

      auto lookup = std::make_shared<TestTxtLookup>();
      lookup->AddRecord("arc._domainkey.example.org", "v=DKIM1; k=rsa; p=" + String(key.publicKeyBase64));

      String messageFile = FileUtilities::GetTempFileName();

      auto verify = [&](ChainBuilder &builder, std::shared_ptr<DMARCTxtLookup> keys) -> ARCVerifier::Result
      {
         builder.Write();
         ARCVerifier verifier(keys);
         return verifier.Verify(messageFile);
      };

      try
      {
         // A message without ARC header fields has no chain.
         {
            ChainBuilder builder(key, messageFile);
            Expect(verify(builder, lookup) == ARCVerifier::Result::None, "no chain");
         }

         // Chains of one, two and three sets pass.
         for (int hops = 1; hops <= 3; hops++)
         {
            ChainBuilder builder(key, messageFile);

            for (int hop = 1; hop <= hops; hop++)
               builder.AddSet(hop == 1 ? "none" : "pass");

            builder.Write();

            ARCVerifier verifier(lookup);
            Expect(verifier.Verify(messageFile) == ARCVerifier::Result::Pass, "valid chain");
            Expect(verifier.GetHighestInstance() == hops, "highest instance");
            Expect(verifier.GetSealerDomain() == "example.org", "sealer domain");

            AnsiString expectedStart = AnsiString("mx") + std::to_string(hops).c_str() + ".example.org;";
            Expect(verifier.GetAuthenticationResults().Find(expectedStart) == 0, "results of the last sealer");
         }

         // A body changed after sealing breaks the latest message signature.
         {
            ChainBuilder builder(key, messageFile);
            builder.AddSet("none");
            builder.body = "Changed.\r\n";
            Expect(verify(builder, lookup) == ARCVerifier::Result::Fail, "changed body");
         }

         // So does a changed signed header field.
         {
            ChainBuilder builder(key, messageFile);
            builder.AddSet("none");
            builder.header.Replace("Subject: ARC test", "Subject: Changed");
            Expect(verify(builder, lookup) == ARCVerifier::Result::Fail, "changed subject");
         }

         // Unsigned header fields may be added, as forwarders do.
         {
            ChainBuilder builder(key, messageFile);
            builder.AddSet("none");
            builder.header = "Received: from forwarder.example.org\r\n" + builder.header;
            Expect(verify(builder, lookup) == ARCVerifier::Result::Pass, "added unsigned field");
         }

         // Changed results of an earlier sealer break the seals.
         {
            ChainBuilder builder(key, messageFile);
            builder.AddSet("none");
            builder.AddSet("pass");
            builder.header.Replace("mx1.example.org; dkim=pass", "mx1.example.org; dkim=fail");
            Expect(verify(builder, lookup) == ARCVerifier::Result::Fail, "changed earlier results");
         }

         // A missing field makes the set incomplete.
         {
            ChainBuilder builder(key, messageFile);
            builder.AddSet("none");
            builder.AddSet("pass");

            int start = builder.header.Find("ARC-Authentication-Results: i=1;");
            int end = builder.header.Find("\r\n", start);
            builder.header = builder.header.Mid(0, start) + builder.header.Mid(end + 2);

            Expect(verify(builder, lookup) == ARCVerifier::Result::Fail, "incomplete set");
         }

         // The first seal must say cv=none, and later seals cv=pass.
         {
            ChainBuilder builder(key, messageFile);
            builder.AddSet("pass");
            Expect(verify(builder, lookup) == ARCVerifier::Result::Fail, "cv=pass in the first set");
         }

         {
            ChainBuilder builder(key, messageFile);
            builder.AddSet("none");
            builder.AddSet("fail");
            Expect(verify(builder, lookup) == ARCVerifier::Result::Fail, "cv=fail");
         }

         // Two fields of the same kind for one instance.
         {
            ChainBuilder builder(key, messageFile);
            builder.AddSet("none");
            builder.header = "ARC-Seal: i=1; a=rsa-sha256; cv=none; d=example.org; s=arc; b=abc\r\n" + builder.header;
            Expect(verify(builder, lookup) == ARCVerifier::Result::Fail, "duplicate seal");
         }

         // A field with an invalid instance.
         {
            ChainBuilder builder(key, messageFile);
            builder.AddSet("none");
            builder.header = "ARC-Seal: i=0; a=rsa-sha256; cv=none; d=example.org; s=arc; b=abc\r\n" + builder.header;
            Expect(verify(builder, lookup) == ARCVerifier::Result::Fail, "invalid instance");
         }

         // A gap in the instances.
         {
            ChainBuilder builder(key, messageFile);
            builder.AddSet("none");
            builder.header.Replace("i=1;", "i=2;");
            builder.header.Replace("i=1\r\n", "i=2\r\n");
            Expect(verify(builder, lookup) == ARCVerifier::Result::Fail, "missing instance 1");
         }

         // Without a published key, nothing verifies.
         {
            ChainBuilder builder(key, messageFile);
            builder.AddSet("none");
            Expect(verify(builder, std::make_shared<TestTxtLookup>()) == ARCVerifier::Result::Fail, "missing key");
         }
      }
      catch (...)
      {
         FileUtilities::DeleteFile(messageFile);
         throw;
      }

      FileUtilities::DeleteFile(messageFile);
   }
}
