// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "stdafx.h"

#include "SPFConformanceTester.h"

#include "SPFConformanceLookup.h"
#include "SPFConformanceSuite.h"

#include "../SPFAddress.h"
#include "../SPFEvaluator.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   namespace SPFConformance
   {
      namespace
      {
         // Failure messages are assembled by hand rather than through Formatter,
         // so that this file needs nothing from the parts of Common which the
         // portable build does not have.
         AnsiString Count(int value)
         {
            return AnsiString(std::to_string(value).c_str());
         }
      }

      SPFConformanceTester::SPFConformanceTester() :
         casesRun_(0)
      {

      }

      int
      SPFConformanceTester::GetCaseCount()
      {
         return CaseCount;
      }

      void
      SPFConformanceTester::Check_(bool condition, const AnsiString &what)
      {
         if (condition)
            return;

         failures_.push_back(what);
      }

      void
      SPFConformanceTester::CheckEqual_(const AnsiString &expected, const AnsiString &actual, const AnsiString &what)
      {
         if (expected == actual)
            return;

         // With the lengths, because some of the suite's records differ from what
         // they look like by an invisible byte - a NUL, a control character - and
         // a message showing only the text would read as though the two were the
         // same.
         failures_.push_back(what + ": expected \"" + expected + "\" (" +
                             Count(expected.GetLength()) + " bytes), got \"" +
                             actual + "\" (" + Count(actual.GetLength()) + " bytes)");
      }

      const Section &
      SPFConformanceTester::FindSection_(const AnsiString &description)
      {
         for (int i = 0; i < SectionCount; i++)
         {
            if (AnsiString(Sections[i].description) == description)
               return Sections[i];
         }

         failures_.push_back("there is no section called \"" + description + "\"");

         return Sections[0];
      }

      // Walks every byte of the table. Nothing here can fail on well-formed data;
      // the point is that a table which points somewhere it should not, or counts
      // something it does not hold, is found by the sanitizers rather than by a
      // case failing later for an unrelated-looking reason.
      void
      SPFConformanceTester::TestTableIsWhole_()
      {
         Check_(SectionCount == 16, "the suite has sixteen sections");
         Check_(CaseCount == 203, "the suite has 203 cases");

         int cases = 0;
         int zones = 0;
         int records = 0;

         for (int s = 0; s < SectionCount; s++)
         {
            const Section &section = Sections[s];

            Check_(section.description != 0 && *section.description != 0,
                   "every section is described");
            Check_(section.zoneCount > 0, "every section has a zone");
            Check_(section.caseCount > 0, "every section has cases");

            zones += section.zoneCount;
            cases += section.caseCount;

            for (int z = 0; z < section.zoneCount; z++)
            {
               const Zone &zone = section.zones[z];

               Check_(zone.name != 0 && *zone.name != 0, "every zone is named");
               Check_(zone.recordCount >= 0, "no zone holds a negative record count");

               records += zone.recordCount;

               for (int r = 0; r < zone.recordCount; r++)
               {
                  const Record &record = zone.records[r];

                  Check_(record.stringCount >= 0, "no record holds a negative string count");
                  Check_(record.stringCount == 0 || record.strings != 0,
                         "a record with strings has somewhere to keep them");

                  for (int i = 0; i < record.stringCount; i++)
                  {
                     Check_(record.strings[i].bytes != 0, "every character-string is there");
                     Check_(record.strings[i].length >= 0,
                            "no character-string is of negative length");
                  }
               }
            }

            for (int c = 0; c < section.caseCount; c++)
            {
               const Case &testCase = section.cases[c];

               Check_(testCase.name != 0 && *testCase.name != 0, "every case is named");
               Check_(testCase.spec != 0, "every case names the part of the RFC it tests");
               Check_(testCase.clientIp != 0 && *testCase.clientIp != 0,
                      "every case has a client address");
               // The sender is empty in the cases which test what happens without
               // one, and the HELO host is what stands in for it there.
               Check_(testCase.mailFrom != 0, "every case has a sender");
               Check_(testCase.heloHost != 0 && *testCase.heloHost != 0,
                      "every case has a HELO host");
               Check_(testCase.acceptedResultCount >= 1 && testCase.acceptedResultCount <= 2,
                      "every case accepts one result or two");
               Check_(testCase.acceptedResults != 0, "every case says which results it accepts");

               for (int r = 0; r < testCase.acceptedResultCount; r++)
                  Check_(*ResultName(testCase.acceptedResults[r]) != '?',
                         "every accepted result is one of the seven");
            }
         }

         Check_(cases == CaseCount, "the sections hold every case the table counts");

         // Recorded so that a change to the vendored suite which silently drops
         // half of it is visible here rather than in a smaller pass rate.
         Check_(zones == 256, "the suite describes 256 zones");
         Check_(records == 352, "the suite describes 352 records");
      }

      void
      SPFConformanceTester::TestResultNames_()
      {
         CheckEqual_("none", ResultName(Result::None), "none");
         CheckEqual_("neutral", ResultName(Result::Neutral), "neutral");
         CheckEqual_("pass", ResultName(Result::Pass), "pass");
         CheckEqual_("fail", ResultName(Result::Fail), "fail");
         CheckEqual_("softfail", ResultName(Result::SoftFail), "softfail");
         CheckEqual_("temperror", ResultName(Result::TempError), "temperror");
         CheckEqual_("permerror", ResultName(Result::PermError), "permerror");
      }

      // The suite publishes most of its policies as records of the deprecated type
      // SPF rather than as TXT records, and expects a driver to present them as
      // the TXT records an RFC 7208 evaluator would find - except where a host
      // publishes a TXT record of its own, in which case the type-SPF records are
      // what the evaluator is supposed to ignore. The generator applies that rule;
      // these are the cases from the "Record lookup" and "Selecting records"
      // sections which exist to catch getting it wrong.
      void
      SPFConformanceTester::TestTypeSpfBecomesTxt_()
      {
         const Section &lookup = FindSection_("Record lookup");

         {
            // A TXT record and a type-SPF record saying the same thing. Only the
            // TXT record is there, otherwise there would appear to be two.
            ConformanceLookup resolver(lookup);
            std::vector<AnsiString> records;
            Check_(resolver.GetTXTRecords("both.example.net", records), "both.example.net answers");
            Check_(records.size() == 1, "both.example.net publishes one TXT record");
            if (records.size() == 1)
               CheckEqual_("v=spf1 -all", records[0], "both.example.net");
         }

         {
            // A type-SPF record and "TXT: NONE", which is how the suite says the
            // host has no TXT record. An evaluator querying TXT learns nothing,
            // and RFC 7208 section 4.3 makes that none rather than an error.
            ConformanceLookup resolver(lookup);
            std::vector<AnsiString> records;
            Check_(resolver.GetTXTRecords("spfonly.example.net", records), "spfonly.example.net answers");
            Check_(records.empty(), "spfonly.example.net publishes no TXT record");
         }

         const Section &selecting = FindSection_("Selecting records");

         {
            // Only a type-SPF record, so it stands in as the TXT record.
            ConformanceLookup resolver(selecting);
            std::vector<AnsiString> records;
            Check_(resolver.GetTXTRecords("example1.com", records), "example1.com answers");
            Check_(records.size() == 1, "example1.com publishes one TXT record");
            if (records.size() == 1)
               CheckEqual_("v=spf1", records[0], "example1.com");
         }

         {
            // Two type-SPF records and a TXT record. The TXT record wins outright:
            // if the type-SPF records were carried over as well there would be
            // three, and the case expects the one.
            ConformanceLookup resolver(selecting);
            std::vector<AnsiString> records;
            Check_(resolver.GetTXTRecords("example8.com", records), "example8.com answers");
            Check_(records.size() == 1, "example8.com publishes one TXT record");
            if (records.size() == 1)
               CheckEqual_("v=spf1 +all", records[0], "example8.com");
         }

         {
            // Two type-SPF records and no TXT record, so both stand in, and an
            // evaluator is supposed to call that a permerror.
            ConformanceLookup resolver(selecting);
            std::vector<AnsiString> records;
            Check_(resolver.GetTXTRecords("example6.com", records), "example6.com answers");
            Check_(records.size() == 2, "example6.com publishes two TXT records");
         }
      }

      // RFC 7208 section 3.3: a TXT record made of several character-strings is
      // read as those strings joined with nothing between them. The suite has a
      // case for each way that can go wrong.
      void
      SPFConformanceTester::TestCharacterStringsAreJoined_()
      {
         const Section &selecting = FindSection_("Selecting records");

         {
            // "v=spf1" and "mx", which join into "v=spf1mx". The version is not
            // terminated, so this is not an SPF record at all.
            ConformanceLookup resolver(selecting);
            std::vector<AnsiString> records;
            Check_(resolver.GetTXTRecords("example2.com", records), "example2.com answers");
            Check_(records.size() == 1, "example2.com publishes one TXT record");
            if (records.size() == 1)
               CheckEqual_("v=spf1mx", records[0], "example2.com");
         }

         const Section &initial = FindSection_("Initial processing");

         {
            // Two records: "v=spf1 ip4:" and "192.0.2.5 -all" joined, and one made
            // of no character-strings at all.
            ConformanceLookup resolver(initial);
            std::vector<AnsiString> records;
            Check_(resolver.GetTXTRecords("null.example.com", records), "null.example.com answers");
            Check_(records.size() == 2, "null.example.com publishes two TXT records");
            if (records.size() == 2)
            {
               CheckEqual_("v=spf1 ip4:192.0.2.5 -all", records[0], "null.example.com, first");
               CheckEqual_("", records[1], "null.example.com, second");
            }
         }
      }

      // RFC 7208 section 7.1 does not allow a NUL octet in a domain name, and the
      // suite has two policies which end in one, expecting a permerror. The octet
      // only reaches an evaluator because a character-string carries its length: a
      // record read as a C string would stop at the NUL, the policy would look
      // perfectly well formed, and the two cases would be decided on a record the
      // suite did not publish.
      void
      SPFConformanceTester::TestNulOctetsSurvive_()
      {
         const char *sections[] = { "A mechanism syntax", "MX mechanism syntax" };
         const char *policies[] = { "v=spf1 a:foo.example.com", "v=spf1 mx:foo.example.com" };

         for (int i = 0; i < 2; i++)
         {
            ConformanceLookup resolver(FindSection_(sections[i]));
            std::vector<AnsiString> records;
            Check_(resolver.GetTXTRecords("e3.example.com", records),
                   "e3.example.com answers");
            Check_(records.size() == 1, "e3.example.com publishes one TXT record");

            if (records.size() != 1)
               continue;

            AnsiString expected = policies[i];
            expected += '\0';

            Check_(records[0].GetLength() == expected.GetLength(),
                   "the NUL octet is still in the policy");
            CheckEqual_(expected, records[0], "the policy with its NUL octet");
         }
      }

      // A bare TIMEOUT in a zone stands for a DNS server which does not answer.
      // RFC 7208 section 4.4 makes that a temperror, and the suite has cases which
      // turn on a zone timing out for one record type while answering for another.
      void
      SPFConformanceTester::TestTimeouts_()
      {
         const Section &lookup = FindSection_("Record lookup");

         {
            // Nothing but a timeout.
            ConformanceLookup resolver(lookup);
            std::vector<AnsiString> records;
            Check_(!resolver.GetTXTRecords("alltimeout.example.net", records),
                   "alltimeout.example.net does not answer");
         }

         {
            // A type-SPF record, "TXT: NONE", and a timeout. The TXT query is the
            // one an RFC 7208 evaluator makes, and it is the one that times out.
            ConformanceLookup resolver(lookup);
            std::vector<AnsiString> records;
            Check_(!resolver.GetTXTRecords("txttimeout.example.net", records),
                   "txttimeout.example.net does not answer a TXT query");
         }

         {
            // A TXT record and a timeout. The TXT query is answered from the
            // record; a query for anything else is not answered at all. This is
            // the pair that a single flag for the whole zone would get wrong.
            ConformanceLookup resolver(lookup);
            std::vector<AnsiString> records;
            Check_(resolver.GetTXTRecords("spftimeout.example.net", records),
                   "spftimeout.example.net answers a TXT query");
            Check_(records.size() == 1, "spftimeout.example.net publishes one TXT record");

            std::vector<AnsiString> addresses;
            Check_(!resolver.GetARecords("spftimeout.example.net", addresses),
                   "spftimeout.example.net does not answer an A query");
         }

         {
            // A name the section says nothing about. An empty answer, not a
            // failure: the host does not exist, which is not the same as not
            // knowing whether it does.
            ConformanceLookup resolver(lookup);
            std::vector<AnsiString> records;
            Check_(resolver.GetTXTRecords("nothing.example.net", records),
                   "a name outside the zone answers");
            Check_(records.empty(), "a name outside the zone publishes nothing");
         }
      }

      // DNS ignores the case of ASCII letters in a name, and a trailing dot says
      // only that the name is already absolute. The suite leans on both: it writes
      // some zone names in mixed case, and some records point at a name which
      // differs from the zone it names in nothing but case and a dot.
      void
      SPFConformanceTester::TestNamesAreMatchedAsDnsMatchesThem_()
      {
         const Section &selecting = FindSection_("Selecting records");

         {
            ConformanceLookup resolver(selecting);
            std::vector<AnsiString> addresses;
            Check_(resolver.GetARecords("MAIL.EXAMPLE1.COM", addresses),
                   "a name in upper case answers");
            Check_(addresses.size() == 1, "mail.example1.com has one address");
            if (addresses.size() == 1)
               CheckEqual_("1.2.3.4", addresses[0], "mail.example1.com");
         }

         {
            ConformanceLookup resolver(selecting);
            std::vector<AnsiString> addresses;
            Check_(resolver.GetARecords("mail.example1.com.", addresses),
                   "a name with a trailing dot answers");
            Check_(addresses.size() == 1, "a trailing dot names the same host");
         }

         const Section &ptr = FindSection_("PTR mechanism syntax");

         {
            // The suite writes this reverse-mapping name with its hex nibbles in
            // upper case; an evaluator building it from an address would produce
            // lower case, or the other way round.
            ConformanceLookup resolver(ptr);
            std::vector<AnsiString> hostNames;
            Check_(resolver.GetPTRRecords("1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.e.b.a.b.e.f.a.c.ip6.arpa", hostNames),
                   "a reverse-mapping name in lower case answers");
            Check_(hostNames.size() == 1, "the address maps back to one host");
            if (hostNames.size() == 1)
               CheckEqual_("e3.example.com", hostNames[0], "the reverse mapping");
         }
      }

      // A reverse-mapping name can point at several hosts, and RFC 7208 section
      // 5.5 has the ptr mechanism check each of them.
      void
      SPFConformanceTester::TestReverseMappings_()
      {
         const Section &ptr = FindSection_("PTR mechanism syntax");

         ConformanceLookup resolver(ptr);
         std::vector<AnsiString> hostNames;
         Check_(resolver.GetPTRRecords("4.3.2.1.in-addr.arpa", hostNames),
                "1.2.3.4 maps back");
         Check_(hostNames.size() == 3, "1.2.3.4 maps back to three hosts");
      }

      // RFC 7208 section 5.4 walks a domain's MX records; DNSResolver hands its
      // callers the host names in preference order, and the table is built that
      // way, which matters where the number of names is over the section 4.6.4
      // limit and only the first ten are looked at.
      void
      SPFConformanceTester::TestMxPreferenceOrder_()
      {
         const Section &limits = FindSection_("Processing limits");

         ConformanceLookup resolver(limits);
         std::vector<AnsiString> hostNames;
         Check_(resolver.GetMXRecords("e4.example.com", hostNames), "e4.example.com answers");
         Check_(hostNames.size() == 11, "e4.example.com names eleven mail exchangers");

         if (hostNames.size() == 11)
         {
            // Preferences nought to nine, then the domain itself at ten.
            for (int i = 0; i < 10; i++)
               CheckEqual_("mail.example.com", hostNames[i], "a mail exchanger of e4.example.com");

            CheckEqual_("e4.example.com", hostNames[10], "the last mail exchanger of e4.example.com");
         }
      }

      // A CNAME is followed one step, which is what the suite's own driver does.
      void
      SPFConformanceTester::TestAliases_()
      {
         const Section &bugs = FindSection_("Test cases from implementation bugs");

         {
            // b.example.org is an alias for a.example.org, which publishes four
            // TXT records, one of them a policy. The case exists because an
            // implementation which followed the alias twice found the policy twice
            // and called it a permerror.
            ConformanceLookup resolver(bugs);
            std::vector<AnsiString> records;
            Check_(resolver.GetTXTRecords("b.example.org", records), "b.example.org answers");
            Check_(records.size() == 4, "an alias answers with what it points at");
         }

         const Section &ptr = FindSection_("PTR mechanism syntax");

         {
            // cname.example.com is an alias for itself, spelled with a trailing
            // dot and a different case. Following it even once more would not
            // terminate.
            ConformanceLookup resolver(ptr);
            std::vector<AnsiString> addresses;
            Check_(resolver.GetARecords("cname.example.com", addresses),
                   "an alias for itself answers");
            Check_(addresses.empty(), "an alias for itself has no address");
         }

         {
            // And an alias for that alias.
            ConformanceLookup resolver(ptr);
            std::vector<AnsiString> addresses;
            Check_(resolver.GetARecords("loop4.example.com", addresses),
                   "an alias for an alias for itself answers");
            Check_(addresses.empty(), "an alias for an alias for itself has no address");
         }
      }

      // Every query is counted, so that the lookup limits of RFC 7208 section
      // 4.6.4 can be checked against the resolver rather than inferred from a
      // result.
      void
      SPFConformanceTester::TestQueriesAreCounted_()
      {
         const Section &selecting = FindSection_("Selecting records");

         ConformanceLookup resolver(selecting);
         Check_(resolver.GetQueryCount() == 0, "a fresh resolver has answered nothing");

         std::vector<AnsiString> records;
         Check_(resolver.GetTXTRecords("example1.com", records), "example1.com answers");
         Check_(resolver.GetQueryCount() == 1, "one query has been answered");

         Check_(resolver.GetTXTRecords("nothing.example.com", records),
                "a name outside the zone answers");
         Check_(resolver.GetQueryCount() == 2, "a name outside the zone is still a query");
      }

      namespace
      {
         // The domain an evaluation is done against, which the caller picks
         // rather than the evaluation: the sender's domain, or the HELO argument
         // when there is no sender. This is what SPF::Test and
         // SenderAuthentication do in the server, and what the suite means by a
         // case with an empty mailfrom.
         AnsiString DomainOf(const Case &testCase)
         {
            AnsiString sender = testCase.mailFrom;

            int at = sender.ReverseFind('@');

            if (at >= 0)
               return sender.Mid(at + 1);

            if (!sender.IsEmpty())
            {
               // A sender with no "@" at all is not an address; there is no
               // domain in it to check.
               return "";
            }

            return testCase.heloHost;
         }

         bool Accepts(const Case &testCase, Result result)
         {
            for (int i = 0; i < testCase.acceptedResultCount; i++)
            {
               if (testCase.acceptedResults[i] == result)
                  return true;
            }

            return false;
         }

         AnsiString AcceptedResultsOf(const Case &testCase)
         {
            AnsiString text;

            for (int i = 0; i < testCase.acceptedResultCount; i++)
            {
               if (i > 0)
                  text += " or ";

               text += ResultName(testCase.acceptedResults[i]);
            }

            return text;
         }
      }

      namespace
      {
         // The suite's name for one of hMailServer's results, so that a failure
         // message can be read against rfc7208-tests.yml.
         Result ToSuiteResult(SPFResult result)
         {
            switch (result)
            {
            case SPFResult::None:
               return Result::None;

            case SPFResult::Neutral:
               return Result::Neutral;

            case SPFResult::Pass:
               return Result::Pass;

            case SPFResult::Fail:
               return Result::Fail;

            case SPFResult::SoftFail:
               return Result::SoftFail;

            case SPFResult::TempError:
               return Result::TempError;

            case SPFResult::PermError:
               break;
            }

            return Result::PermError;
         }
      }

      // Runs every case of the suite through the evaluator and compares what
      // comes back with what rfc7208-tests.yml expects.
      //
      // A case names the client address, the sender and the HELO argument; the
      // domain to check is the caller's to pick, and DomainOf picks it the way
      // the server does. Some cases accept more than one result, because RFC 7208
      // leaves the answer to the implementation; the suite's first choice is its
      // preference and any of them passes.
      int
      SPFConformanceTester::RunCases_()
      {
         int decided = 0;

         for (int s = 0; s < SectionCount; s++)
         {
            const Section &section = Sections[s];

            for (int c = 0; c < section.caseCount; c++)
            {
               const Case &testCase = section.cases[c];

               auto lookup = std::make_shared<ConformanceLookup>(section);

               SPFEvaluator evaluator(lookup);

               // The suite asserts the text of an explanation, and two of them
               // name the receiving host and none the time, so both are fixed
               // here rather than left to the clock.
               evaluator.SetReceivingHost("receiver.example.com");
               evaluator.SetTimestamp(0);

               SPFAddress clientAddress;
               AnsiString explanation;

               Result result;

               if (!SPFAddress::TryParse(testCase.clientIp, clientAddress))
               {
                  failures_.push_back(AnsiString(testCase.name) + " (" + testCase.spec +
                                      "): the client address \"" + testCase.clientIp +
                                      "\" could not be read");
                  continue;
               }

               result = ToSuiteResult(evaluator.Check(clientAddress, DomainOf(testCase),
                                                      testCase.mailFrom, testCase.heloHost,
                                                      explanation));

               decided++;

               if (!Accepts(testCase, result))
               {
                  failures_.push_back(AnsiString(testCase.name) + " (" + testCase.spec + "): expected " +
                                      AcceptedResultsOf(testCase) + ", got " + ResultName(result));
                  continue;
               }

               if (testCase.explanation == 0)
               {
                  // The case asserts nothing about the explanation.
                  continue;
               }

               // An empty string in the table is the suite's "use your own
               // default", which hMailServer reports by producing no explanation
               // at all.
               if (explanation == AnsiString(testCase.explanation))
                  continue;

               failures_.push_back(AnsiString(testCase.name) + " (" + testCase.spec +
                                   "): expected the explanation \"" + testCase.explanation +
                                   "\", got \"" + explanation + "\"");
            }
         }

         // Every case is decided now, so the floor is the whole suite. It stays a
         // floor rather than an equality because the count is what a case is
         // dropped from silently: an evaluation which stopped answering would
         // otherwise look like nothing had changed.
         Check_(decided >= GetCaseCount(), "every case of the suite is decided (" +
                                           Count(decided) + " of " + Count(GetCaseCount()) + ")");

         return decided;
      }

      std::vector<AnsiString>
      SPFConformanceTester::Run()
      {
         failures_.clear();
         casesRun_ = 0;

         TestTableIsWhole_();
         TestResultNames_();
         TestTypeSpfBecomesTxt_();
         TestCharacterStringsAreJoined_();
         TestNulOctetsSurvive_();
         TestTimeouts_();
         TestNamesAreMatchedAsDnsMatchesThem_();
         TestReverseMappings_();
         TestMxPreferenceOrder_();
         TestAliases_();
         TestQueriesAreCounted_();

         casesRun_ = RunCases_();

         return failures_;
      }
   }
}
