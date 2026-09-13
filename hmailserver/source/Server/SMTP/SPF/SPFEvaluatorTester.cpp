// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "stdafx.h"

#include "SPFEvaluatorTester.h"

#include "SPFAddress.h"
#include "SPFEvaluator.h"
#include "SPFTestLookup.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   namespace
   {
      const char *CLIENT_IP = "192.0.2.1";
      const char *CHECKED_DOMAIN = "example.com";
      const char *SENDER_ADDRESS = "test@example.com";
      const char *HELO_HOST = "mail.example.com";

      const char *ResultName(SPFResult result)
      {
         switch (result)
         {
         case SPFResult::None:
            return "none";

         case SPFResult::Neutral:
            return "neutral";

         case SPFResult::Pass:
            return "pass";

         case SPFResult::Fail:
            return "fail";

         case SPFResult::SoftFail:
            return "softfail";

         case SPFResult::TempError:
            return "temperror";

         case SPFResult::PermError:
            break;
         }

         return "permerror";
      }

      SPFAddress AddressOf(const AnsiString &text)
      {
         SPFAddress address;

         SPFAddress::TryParse(text, address);

         return address;
      }
   }

   std::vector<AnsiString>
   SPFEvaluatorTester::Run()
   {
      failures_.clear();

      TestTemporaryFailures_();
      TestTermCounting_();
      TestTermLimit_();
      TestResolverAnswers_();
      TestExplanationOnlyForFail_();

      return failures_;
   }

   // Sections 5.3 and 5.4: a query which could not be answered makes the check a
   // temperror, so that the same message may be accepted when it is tried again.
   // The suite only covers exists, section 5.7.
   void
   SPFEvaluatorTester::TestTemporaryFailures_()
   {
      struct Case
      {
         const char *record;

         // The name whose queries fail, and which record type it fails for.
         const char *failing;

         const char *what;
      };

      // Every case names a host other than the domain being checked. A failing
      // name fails for every record type, so failing the domain itself would stop
      // its own TXT lookup and the check would be a temperror before it ever
      // reached the mechanism - which looks like a pass and tests nothing.
      const Case cases[] =
      {
         // The TXT query for the record itself, which is the one case that is
         // meant to fail before a mechanism is reached.
         { "v=spf1 a -all", "example.com", "a domain whose own record cannot be read" },

         // The A query behind an a mechanism.
         { "v=spf1 a:host.example.com -all", "host.example.com", "an a mechanism whose address lookup fails" },

         // The MX query behind an mx mechanism.
         { "v=spf1 mx:mxhost.example.com -all", "mxhost.example.com", "an mx mechanism whose MX lookup fails" },

         // The A query for a host an MX record named, which is a second query the
         // mechanism makes and a separate path out of it.
         { "v=spf1 mx -all", "mail.example.com", "an mx mechanism whose host lookup fails" },

         // The A query behind an exists mechanism, which the suite does cover.
         { "v=spf1 exists:host.example.com -all", "host.example.com", "an exists mechanism whose lookup fails" },

         // The TXT query for an included record.
         { "v=spf1 include:other.example.com -all", "other.example.com", "an include whose record cannot be read" },

         // The TXT query for a redirect target.
         { "v=spf1 redirect=other.example.com", "other.example.com", "a redirect whose record cannot be read" }
      };

      for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++)
      {
         auto lookup = std::make_shared<SPFTestLookup>();

         lookup->AddTXT(CHECKED_DOMAIN, cases[i].record);
         lookup->AddMX(CHECKED_DOMAIN, "mail.example.com");
         lookup->AddA("mail.example.com", "10.0.0.1");
         lookup->AddA("host.example.com", "10.0.0.1");
         lookup->AddMX("mxhost.example.com", "mail.example.com");
         lookup->AddTXT("other.example.com", "v=spf1 -all");
         lookup->AddA(CHECKED_DOMAIN, "10.0.0.1");

         lookup->Fail(cases[i].failing);

         SPFEvaluator evaluator(lookup);

         AnsiString explanation;

         SPFResult result = evaluator.Check(AddressOf(CLIENT_IP), CHECKED_DOMAIN, SENDER_ADDRESS, HELO_HOST, explanation);

         if (result == SPFResult::TempError)
            continue;

         failures_.push_back(AnsiString(cases[i].what) + " gave " + ResultName(result) +
                             " rather than temperror");
      }

      // Section 5.5 is the exception: a ptr mechanism whose reverse lookup fails
      // does not end the check, it simply does not match. The names are the
      // client's to publish, so a failure there says nothing about the domain.
      {
         auto lookup = std::make_shared<SPFTestLookup>();

         lookup->AddTXT(CHECKED_DOMAIN, "v=spf1 ptr -all");
         lookup->AddPTR("1.2.0.192.in-addr.arpa", "mail.example.com");
         lookup->AddA("mail.example.com", CLIENT_IP);
         lookup->Fail("1.2.0.192.in-addr.arpa");

         SPFEvaluator evaluator(lookup);

         AnsiString explanation;

         SPFResult result = evaluator.Check(AddressOf(CLIENT_IP), CHECKED_DOMAIN, SENDER_ADDRESS, HELO_HOST, explanation);

         if (result != SPFResult::Fail)
         {
            failures_.push_back(AnsiString("a ptr mechanism whose reverse lookup fails gave ") +
                                ResultName(result) + " rather than falling through to -all");
         }
      }
   }

   // Section 4.6.4 counts terms rather than queries: one mechanism is one term
   // however many names it resolves, and the mechanisms which ask DNS nothing
   // cost nothing.
   void
   SPFEvaluatorTester::TestTermCounting_()
   {
      struct Case
      {
         const char *record;
         int terms;
         const char *what;
      };

      const Case cases[] =
      {
         // The six terms section 4.6.4 names.
         { "v=spf1 a -all", 1, "an a mechanism" },
         { "v=spf1 mx -all", 1, "an mx mechanism" },
         { "v=spf1 ptr -all", 1, "a ptr mechanism" },
         { "v=spf1 exists:nothing.example.com -all", 1, "an exists mechanism" },
         { "v=spf1 include:other.example.com -all", 1, "an include" },
         { "v=spf1 redirect=other.example.com", 1, "a redirect" },

         // And the terms which cost nothing, because they ask DNS nothing.
         { "v=spf1 -all", 0, "an all mechanism" },
         { "v=spf1 ip4:10.0.0.1 -all", 0, "an ip4 mechanism" },
         { "v=spf1 ip6:2001:db8::1 -all", 0, "an ip6 mechanism" },
         { "v=spf1 -all exp=explain.example.com", 0, "an exp modifier" },
         { "v=spf1 default=+ -all", 0, "a modifier nobody knows" },

         // One mx mechanism resolving three hosts is still one term. This is what
         // tells the term limit apart from a query limit.
         { "v=spf1 mx:three.example.com -all", 1, "an mx mechanism resolving three hosts" },

         // A term inside an include counts against the same budget as the record
         // which included it: the limit is per check, not per record.
         { "v=spf1 include:onemore.example.com -all", 2, "an include whose record has a term of its own" }
      };

      for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++)
      {
         auto lookup = std::make_shared<SPFTestLookup>();

         lookup->AddTXT(CHECKED_DOMAIN, cases[i].record);
         lookup->AddA(CHECKED_DOMAIN, "10.0.0.1");
         lookup->AddMX(CHECKED_DOMAIN, "mail.example.com");
         lookup->AddA("mail.example.com", "10.0.0.1");
         lookup->AddPTR("1.2.0.192.in-addr.arpa", "mail.example.com");

         lookup->AddTXT("other.example.com", "v=spf1 -all");
         lookup->AddTXT("onemore.example.com", "v=spf1 a -all");
         lookup->AddA("onemore.example.com", "10.0.0.1");
         lookup->AddTXT("explain.example.com", "Computer says no.");

         lookup->AddMX("three.example.com", "one.example.com");
         lookup->AddMX("three.example.com", "two.example.com");
         lookup->AddMX("three.example.com", "third.example.com");
         lookup->AddA("one.example.com", "10.0.0.1");
         lookup->AddA("two.example.com", "10.0.0.2");
         lookup->AddA("third.example.com", "10.0.0.3");

         SPFEvaluator evaluator(lookup);

         AnsiString explanation;

         evaluator.Check(AddressOf(CLIENT_IP), CHECKED_DOMAIN, SENDER_ADDRESS, HELO_HOST, explanation);

         if (evaluator.GetTermCount() == cases[i].terms)
            continue;

         failures_.push_back(AnsiString(cases[i].what) + " cost " +
                             AnsiString(std::to_string(evaluator.GetTermCount()).c_str()) +
                             " of the ten terms, expected " +
                             AnsiString(std::to_string(cases[i].terms).c_str()));
      }
   }

   // Section 4.6.4's limit of ten terms, over a record whose lookups all answer,
   // so that the void-lookup limit cannot decide it first. The suite's own
   // mech-over-limit spends its budget on names which do not exist, which trips
   // the void limit three terms earlier.
   void
   SPFEvaluatorTester::TestTermLimit_()
   {
      // Each include costs one term and answers, so a chain of them counts up
      // without a single empty answer. The last record in the chain passes, so a
      // chain within the limit is a pass and only the limit can change that.
      //
      // A chain of n links costs n + 1 terms: the record being checked includes
      // the first link, and each link includes the next. The count is asserted
      // alongside the result rather than left to be worked out, because getting
      // that arithmetic wrong looks exactly like the limit being off by one.
      struct Case
      {
         int length;
         SPFResult expected;
         const char *what;
      };

      const Case cases[] =
      {
         { 8, SPFResult::Pass, "a chain of nine terms" },
         { 9, SPFResult::Pass, "a chain of ten terms, which is the limit" },
         { 10, SPFResult::PermError, "a chain of eleven terms, which is one too many" }
      };

      for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++)
      {
         auto lookup = std::make_shared<SPFTestLookup>();

         // link0 includes link1, and so on; the last one passes on the client.
         for (int link = 0; link < cases[i].length; link++)
         {
            AnsiString name = AnsiString("link") + AnsiString(std::to_string(link).c_str()) + ".example.com";
            AnsiString next = AnsiString("link") + AnsiString(std::to_string(link + 1).c_str()) + ".example.com";

            lookup->AddTXT(name, AnsiString("v=spf1 include:") + next + " -all");
         }

         AnsiString last = AnsiString("link") + AnsiString(std::to_string(cases[i].length).c_str()) + ".example.com";

         lookup->AddTXT(last, AnsiString("v=spf1 ip4:") + CLIENT_IP + " -all");
         lookup->AddTXT(CHECKED_DOMAIN, "v=spf1 include:link0.example.com -all");

         SPFEvaluator evaluator(lookup);

         AnsiString explanation;

         SPFResult result = evaluator.Check(AddressOf(CLIENT_IP), CHECKED_DOMAIN, SENDER_ADDRESS, HELO_HOST, explanation);

         if (result != cases[i].expected)
         {
            failures_.push_back(AnsiString(cases[i].what) + " gave " + ResultName(result) +
                                ", expected " + ResultName(cases[i].expected));
            continue;
         }

         // A chain which passed spent every term it has; one which was cut off
         // stopped at the limit.
         int expectedTerms = cases[i].length + 1;

         if (expectedTerms > SPFEvaluator::MaximumTerms)
            expectedTerms = SPFEvaluator::MaximumTerms + 1;

         if (evaluator.GetTermCount() == expectedTerms)
            continue;

         failures_.push_back(AnsiString(cases[i].what) + " cost " +
                             AnsiString(std::to_string(evaluator.GetTermCount()).c_str()) +
                             " terms, expected " +
                             AnsiString(std::to_string(expectedTerms).c_str()));
      }
   }

   // What a resolver hands back is not always an address. Every address in the
   // suite's zones is one, so nothing there says what happens to something that
   // is not.
   void
   SPFEvaluatorTester::TestResolverAnswers_()
   {
      struct Case
      {
         const char *address;
         SPFResult expected;
         const char *what;
      };

      const Case cases[] =
      {
         { CLIENT_IP, SPFResult::Pass, "the client's own address" },

         // Not addresses at all. A mechanism cannot match against one, and
         // section 5.3 gives no way to report it, so it simply does not match.
         { "", SPFResult::Fail, "an empty answer" },
         { "not an address", SPFResult::Fail, "an answer which is not an address" },
         { "192.0.2", SPFResult::Fail, "a truncated address" },
         { "192.0.2.1.1", SPFResult::Fail, "an address with an octet too many" },
         { "192.0.2.256", SPFResult::Fail, "an octet out of range" },

         // An address of the other family. Section 5.6 never matches across
         // families, whatever the addresses look like.
         { "::ffff:192.0.2.1", SPFResult::Pass, "the client's address written as IPv4-mapped" },
         { "2001:db8::1", SPFResult::Fail, "an address of the other family" }
      };

      for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++)
      {
         auto lookup = std::make_shared<SPFTestLookup>();

         lookup->AddTXT(CHECKED_DOMAIN, "v=spf1 a -all");
         lookup->AddA(CHECKED_DOMAIN, cases[i].address);

         SPFEvaluator evaluator(lookup);

         AnsiString explanation;

         SPFResult result = evaluator.Check(AddressOf(CLIENT_IP), CHECKED_DOMAIN, SENDER_ADDRESS, HELO_HOST, explanation);

         if (result == cases[i].expected)
            continue;

         failures_.push_back(AnsiString("an a mechanism against ") + cases[i].what + " gave " +
                             ResultName(result) + ", expected " + ResultName(cases[i].expected));
      }
   }

   // Section 6.2: an exp modifier explains a fail, and only a fail. The suite
   // asserts explanations for 22 cases and every one of them is a fail, so
   // explaining a softfail or a neutral as well goes unnoticed there.
   void
   SPFEvaluatorTester::TestExplanationOnlyForFail_()
   {
      struct Case
      {
         const char *record;
         SPFResult expected;
         bool explained;
         const char *what;
      };

      const Case cases[] =
      {
         { "v=spf1 -all exp=explain.example.com", SPFResult::Fail, true, "a fail" },
         { "v=spf1 ~all exp=explain.example.com", SPFResult::SoftFail, false, "a softfail" },
         { "v=spf1 ?all exp=explain.example.com", SPFResult::Neutral, false, "a neutral" },
         { "v=spf1 +all exp=explain.example.com", SPFResult::Pass, false, "a pass" },

         // Nothing matched, so the record's own default applies and there is
         // nothing to explain.
         { "v=spf1 exp=explain.example.com", SPFResult::Neutral, false, "a record which matched nothing" }
      };

      for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++)
      {
         auto lookup = std::make_shared<SPFTestLookup>();

         lookup->AddTXT(CHECKED_DOMAIN, cases[i].record);
         lookup->AddTXT("explain.example.com", "Computer says no.");

         SPFEvaluator evaluator(lookup);

         AnsiString explanation;

         SPFResult result = evaluator.Check(AddressOf(CLIENT_IP), CHECKED_DOMAIN, SENDER_ADDRESS, HELO_HOST, explanation);

         if (result != cases[i].expected)
         {
            failures_.push_back(AnsiString(cases[i].what) + " gave " + ResultName(result) +
                                ", expected " + ResultName(cases[i].expected));
            continue;
         }

         bool explained = !explanation.IsEmpty();

         if (explained == cases[i].explained)
            continue;

         failures_.push_back(AnsiString(cases[i].what) +
                             (explained ? " was explained" : " was not explained") +
                             " when it should " + (cases[i].explained ? "have been" : "not have been"));
      }

      // Section 6.2: where the explanation cannot be read the receiver uses its
      // own words, and none of these is an error.
      struct Unreadable
      {
         const char *target;
         const char *what;
      };

      const Unreadable unreadable[] =
      {
         { "missing.example.com", "an exp naming a domain with no TXT record" },
         { "two.example.com", "an exp naming a domain with two TXT records" },
         { "broken.example.com", "an exp whose lookup fails" },
         { "invalid.example.com", "an exp whose text does not parse" }
      };

      for (size_t i = 0; i < sizeof(unreadable) / sizeof(unreadable[0]); i++)
      {
         auto lookup = std::make_shared<SPFTestLookup>();

         lookup->AddTXT(CHECKED_DOMAIN, AnsiString("v=spf1 -all exp=") + unreadable[i].target);

         lookup->AddTXT("two.example.com", "One explanation.");
         lookup->AddTXT("two.example.com", "And another.");
         lookup->AddTXT("broken.example.com", "Never read.");
         lookup->Fail("broken.example.com");

         // A macro nobody knows, which section 7.1 does not allow and which
         // section 6.2 turns into the receiver's own explanation rather than an
         // error.
         lookup->AddTXT("invalid.example.com", "The %{x}-files.");

         SPFEvaluator evaluator(lookup);

         AnsiString explanation;

         SPFResult result = evaluator.Check(AddressOf(CLIENT_IP), CHECKED_DOMAIN, SENDER_ADDRESS, HELO_HOST, explanation);

         if (result != SPFResult::Fail)
         {
            failures_.push_back(AnsiString(unreadable[i].what) + " gave " + ResultName(result) +
                                " rather than the fail the record asked for");
            continue;
         }

         if (explanation.IsEmpty())
            continue;

         failures_.push_back(AnsiString(unreadable[i].what) + " produced the explanation \"" +
                             explanation + "\" rather than none");
      }
   }
}
