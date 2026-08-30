// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "StdAfx.h"

#include "DMARCTester.h"

#include "DMARCEvaluator.h"
#include "DMARCPolicyLocator.h"
#include "DMARCTxtLookup.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   namespace
   {
      // Returns canned TXT records, so that the policy lookup can be tested without DNS.
      class TestTxtLookup : public DMARCTxtLookup
      {
      public:

         TestTxtLookup() :
            fail_(false)
         {

         }

         void AddRecord(const String &domain, const String &record)
         {
            records_[domain].push_back(record);
         }

         void SetFail(bool fail)
         {
            fail_ = fail;
         }

         virtual bool GetTXTRecords(const String &domain, std::vector<String> &records)
         {
            if (fail_)
               return false;

            auto iter = records_.find(domain);

            if (iter != records_.end())
               records = iter->second;

            return true;
         }

      private:

         std::map<String, std::vector<String> > records_;
         bool fail_;
      };
   }

   void
   DMARCTester::Test()
   {
      TestRecordParsing_();
      TestOrganizationalDomain_();
      TestAlignment_();
      TestApplicablePolicy_();
      TestPolicyLocator_();
   }

   void
   DMARCTester::TestRecordParsing_()
   {
      {
         DMARCRecord record = ParseValidRecord_("v=DMARC1; p=none");

         if (record.GetPolicy() != DMARCRecord::Policy::None ||
             record.GetHasSubdomainPolicy() ||
             record.GetDKIMAlignment() != DMARCRecord::Alignment::Relaxed ||
             record.GetSPFAlignment() != DMARCRecord::Alignment::Relaxed ||
             record.GetPercent() != 100)
         {
            assert(0);
            throw;
         }
      }

      {
         DMARCRecord record = ParseValidRecord_("v=DMARC1;p=reject;sp=quarantine;adkim=s;aspf=s;pct=50;rua=mailto:a@example.com");

         if (record.GetPolicy() != DMARCRecord::Policy::Reject ||
             !record.GetHasSubdomainPolicy() ||
             record.GetSubdomainPolicy() != DMARCRecord::Policy::Quarantine ||
             record.GetDKIMAlignment() != DMARCRecord::Alignment::Strict ||
             record.GetSPFAlignment() != DMARCRecord::Alignment::Strict ||
             record.GetPercent() != 50)
         {
            assert(0);
            throw;
         }
      }

      // Whitespace is permitted, tag names and policy values are case-insensitive
      // and unknown tags are ignored.
      {
         DMARCRecord record = ParseValidRecord_("v = DMARC1 ; P = Reject ; unknown=value");

         if (record.GetPolicy() != DMARCRecord::Policy::Reject)
         {
            assert(0);
            throw;
         }
      }

      // An invalid percentage falls back to the default.
      if (ParseValidRecord_("v=DMARC1; p=none; pct=abc").GetPercent() != 100 ||
          ParseValidRecord_("v=DMARC1; p=none; pct=150").GetPercent() != 100 ||
          ParseValidRecord_("v=DMARC1; p=none; pct=-1").GetPercent() != 100 ||
          ParseValidRecord_("v=DMARC1; p=none; pct=0").GetPercent() != 0)
      {
         assert(0);
         throw;
      }

      // An invalid subdomain policy is ignored, the main policy still applies.
      {
         DMARCRecord record = ParseValidRecord_("v=DMARC1; p=reject; sp=bogus");

         if (record.GetHasSubdomainPolicy() || record.GetPolicy() != DMARCRecord::Policy::Reject)
         {
            assert(0);
            throw;
         }
      }

      AssertInvalidRecord_("");
      AssertInvalidRecord_("v=DMARC1");                  // no policy
      AssertInvalidRecord_("p=none; v=DMARC1");          // version tag isn't first
      AssertInvalidRecord_("v=dmarc1; p=none");          // version value is case-sensitive
      AssertInvalidRecord_("v=DMARC1; p=bogus");         // unknown policy
      AssertInvalidRecord_("v=spf1 -all");
   }

   void
   DMARCTester::TestOrganizationalDomain_()
   {
      if (DMARCEvaluator::GetOrganizationalDomain(_T("example.com")) != _T("example.com") ||
          DMARCEvaluator::GetOrganizationalDomain(_T("EXAMPLE.COM")) != _T("example.com") ||
          DMARCEvaluator::GetOrganizationalDomain(_T("www.mail.example.com")) != _T("example.com") ||
          DMARCEvaluator::GetOrganizationalDomain(_T("example.co.uk")) != _T("example.co.uk") ||
          DMARCEvaluator::GetOrganizationalDomain(_T("a.b.example.co.uk")) != _T("example.co.uk") ||
          DMARCEvaluator::GetOrganizationalDomain(_T("localhost")) != _T("localhost"))
      {
         assert(0);
         throw;
      }
   }

   void
   DMARCTester::TestAlignment_()
   {
      const DMARCRecord::Alignment strict = DMARCRecord::Alignment::Strict;
      const DMARCRecord::Alignment relaxed = DMARCRecord::Alignment::Relaxed;

      if (!DMARCEvaluator::IsAligned("example.com", "example.com", strict) ||
          !DMARCEvaluator::IsAligned("EXAMPLE.com", "example.COM", strict) ||
          !DMARCEvaluator::IsAligned("mail.example.com", "example.com", relaxed) ||
          !DMARCEvaluator::IsAligned("example.com", "mail.example.com", relaxed) ||
          !DMARCEvaluator::IsAligned("a.example.co.uk", "b.example.co.uk", relaxed))
      {
         assert(0);
         throw;
      }

      if (DMARCEvaluator::IsAligned("mail.example.com", "example.com", strict) ||
          DMARCEvaluator::IsAligned("example.com", "example.net", relaxed) ||
          DMARCEvaluator::IsAligned("example.co.uk", "other.co.uk", relaxed) ||
          DMARCEvaluator::IsAligned("", "example.com", relaxed) ||
          DMARCEvaluator::IsAligned("example.com", "", relaxed))
      {
         assert(0);
         throw;
      }
   }

   void
   DMARCTester::TestApplicablePolicy_()
   {
      DMARCRecord withoutSubdomainPolicy = ParseValidRecord_("v=DMARC1; p=reject");
      DMARCRecord withSubdomainPolicy = ParseValidRecord_("v=DMARC1; p=none; sp=reject");

      // Without a subdomain policy the main policy always applies.
      if (DMARCEvaluator::GetApplicablePolicy(withoutSubdomainPolicy, "sub.example.com", "example.com") !=
          DMARCRecord::Policy::Reject)
      {
         assert(0);
         throw;
      }

      // The subdomain policy only applies when the record was found at another domain.
      if (DMARCEvaluator::GetApplicablePolicy(withSubdomainPolicy, "sub.example.com", "example.com") !=
          DMARCRecord::Policy::Reject ||
          DMARCEvaluator::GetApplicablePolicy(withSubdomainPolicy, "example.com", "example.com") !=
          DMARCRecord::Policy::None)
      {
         assert(0);
         throw;
      }
   }

   void
   DMARCTester::TestPolicyLocator_()
   {
      DMARCRecord record;
      String policyDomain;

      // The record published for the domain itself is used, and other TXT records are ignored.
      {
         auto lookup = std::make_shared<TestTxtLookup>();
         lookup->AddRecord("_dmarc.sub.example.com", "v=spf1 -all");
         lookup->AddRecord("_dmarc.sub.example.com", "v=DMARC1; p=quarantine");
         lookup->AddRecord("_dmarc.example.com", "v=DMARC1; p=reject");

         DMARCPolicyLocator locator(lookup);

         if (locator.Locate("sub.example.com", record, policyDomain) != DMARCPolicyLocator::Result::Found ||
             policyDomain != _T("sub.example.com") ||
             record.GetPolicy() != DMARCRecord::Policy::Quarantine)
         {
            assert(0);
            throw;
         }
      }

      // Without a record at the domain, the organizational domain is used.
      {
         auto lookup = std::make_shared<TestTxtLookup>();
         lookup->AddRecord("_dmarc.example.com", "v=DMARC1; p=none; sp=reject");

         DMARCPolicyLocator locator(lookup);

         if (locator.Locate("SUB.EXAMPLE.COM", record, policyDomain) != DMARCPolicyLocator::Result::Found ||
             policyDomain != _T("example.com") ||
             DMARCEvaluator::GetApplicablePolicy(record, "sub.example.com", policyDomain) !=
             DMARCRecord::Policy::Reject)
         {
            assert(0);
            throw;
         }
      }

      // More than one DMARC record means the domain has no usable policy.
      {
         auto lookup = std::make_shared<TestTxtLookup>();
         lookup->AddRecord("_dmarc.example.com", "v=DMARC1; p=none");
         lookup->AddRecord("_dmarc.example.com", "v=DMARC1; p=reject");

         DMARCPolicyLocator locator(lookup);

         if (locator.Locate("example.com", record, policyDomain) != DMARCPolicyLocator::Result::NoRecord)
         {
            assert(0);
            throw;
         }
      }

      // A record that doesn't parse is the same as no record at all.
      {
         auto lookup = std::make_shared<TestTxtLookup>();
         lookup->AddRecord("_dmarc.example.com", "v=DMARC1; p=bogus");

         DMARCPolicyLocator locator(lookup);

         if (locator.Locate("example.com", record, policyDomain) != DMARCPolicyLocator::Result::NoRecord)
         {
            assert(0);
            throw;
         }
      }

      {
         auto lookup = std::make_shared<TestTxtLookup>();
         DMARCPolicyLocator locator(lookup);

         if (locator.Locate("example.com", record, policyDomain) != DMARCPolicyLocator::Result::NoRecord ||
             !policyDomain.IsEmpty())
         {
            assert(0);
            throw;
         }
      }

      // A failing lookup must not be reported as an absent policy.
      {
         auto lookup = std::make_shared<TestTxtLookup>();
         lookup->SetFail(true);

         DMARCPolicyLocator locator(lookup);

         if (locator.Locate("example.com", record, policyDomain) != DMARCPolicyLocator::Result::TemporaryError)
         {
            assert(0);
            throw;
         }
      }
   }

   void
   DMARCTester::AssertInvalidRecord_(const AnsiString &textRecord)
   {
      DMARCRecord record;

      if (DMARCRecord::Parse(textRecord, record))
      {
         assert(0);
         throw;
      }
   }

   DMARCRecord
   DMARCTester::ParseValidRecord_(const AnsiString &textRecord)
   {
      DMARCRecord record;

      if (!DMARCRecord::Parse(textRecord, record))
      {
         assert(0);
         throw;
      }

      return record;
   }
}
