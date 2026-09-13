// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

// Runs the openspf.org conformance suite for RFC 7208.
//
// What it runs today is the harness, not an evaluation: it checks that the
// table transcribed from rfc7208-tests.yml is whole, and that the stub resolver
// answers out of it the way the suite means its zones to be read. There is no
// SPF evaluator to hand the cases to yet - the parser, the macro expander and
// the evaluator arrive after this - so no case is decided here, and the runner
// says as much when it finishes. RunCases_ is where they will be driven from.
//
// The resolver is worth pinning down on its own, ahead of the evaluator, for
// two reasons. Its rules are not obvious - a zone that times out for one record
// type and answers for another, host names which differ only in case, an alias
// for itself - and if any of them is wrong, every case that leans on it fails
// for a reason that has nothing to do with the evaluator being written against
// it.

#include "stdafx.h"

#include "SPFConformanceLookup.h"
#include "SPFConformanceSuite.h"

#include <cstdio>

namespace
{
   using namespace HM;
   using namespace HM::SPFConformance;

   int failures = 0;

   void Check(bool condition, const char *what)
   {
      if (condition)
         return;

      printf("FAILED: %s\n", what);
      failures++;
   }

   void CheckEqual(const AnsiString &expected, const AnsiString &actual, const char *what)
   {
      if (expected == actual)
         return;

      // With the lengths, because some of the suite's records differ from what
      // they look like by an invisible byte - a NUL, a control character - and
      // a message showing only the text would read as though the two were the
      // same.
      printf("FAILED: %s: expected \"%s\" (%d bytes), got \"%s\" (%d bytes)\n",
             what, expected.c_str(), expected.GetLength(),
             actual.c_str(), actual.GetLength());
      failures++;
   }

   const Section &FindSection(const char *description)
   {
      for (int i = 0; i < SectionCount; i++)
      {
         if (AnsiString(Sections[i].description) == description)
            return Sections[i];
      }

      printf("FAILED: no section called \"%s\"\n", description);
      failures++;

      return Sections[0];
   }

   // Walks every byte of the table. Nothing here can fail on well-formed data;
   // the point is that a table which points somewhere it should not, or counts
   // something it does not hold, is found by the sanitizers rather than by a
   // case failing later for an unrelated-looking reason.
   void TestTableIsWhole()
   {
      Check(SectionCount == 16, "the suite has sixteen sections");
      Check(CaseCount == 203, "the suite has 203 cases");

      int cases = 0;
      int zones = 0;
      int records = 0;

      for (int s = 0; s < SectionCount; s++)
      {
         const Section &section = Sections[s];

         Check(section.description != 0 && *section.description != 0,
               "every section is described");
         Check(section.zoneCount > 0, "every section has a zone");
         Check(section.caseCount > 0, "every section has cases");

         zones += section.zoneCount;
         cases += section.caseCount;

         for (int z = 0; z < section.zoneCount; z++)
         {
            const Zone &zone = section.zones[z];

            Check(zone.name != 0 && *zone.name != 0, "every zone is named");
            Check(zone.recordCount >= 0, "no zone holds a negative record count");

            records += zone.recordCount;

            for (int r = 0; r < zone.recordCount; r++)
            {
               const Record &record = zone.records[r];

               Check(record.stringCount >= 0, "no record holds a negative string count");
               Check(record.stringCount == 0 || record.strings != 0,
                     "a record with strings has somewhere to keep them");

               for (int i = 0; i < record.stringCount; i++)
               {
                  Check(record.strings[i].bytes != 0, "every character-string is there");
                  Check(record.strings[i].length >= 0,
                        "no character-string is of negative length");
               }
            }
         }

         for (int c = 0; c < section.caseCount; c++)
         {
            const Case &testCase = section.cases[c];

            Check(testCase.name != 0 && *testCase.name != 0, "every case is named");
            Check(testCase.spec != 0, "every case names the part of the RFC it tests");
            Check(testCase.clientIp != 0 && *testCase.clientIp != 0,
                  "every case has a client address");
            // The sender is empty in the cases which test what happens without
            // one, and the HELO host is what stands in for it there.
            Check(testCase.mailFrom != 0, "every case has a sender");
            Check(testCase.heloHost != 0 && *testCase.heloHost != 0,
                  "every case has a HELO host");
            Check(testCase.acceptedResultCount >= 1 && testCase.acceptedResultCount <= 2,
                  "every case accepts one result or two");
            Check(testCase.acceptedResults != 0, "every case says which results it accepts");

            for (int r = 0; r < testCase.acceptedResultCount; r++)
               Check(*ResultName(testCase.acceptedResults[r]) != '?',
                     "every accepted result is one of the seven");
         }
      }

      Check(cases == CaseCount, "the sections hold every case the table counts");

      // Recorded so that a change to the vendored suite which silently drops
      // half of it is visible here rather than in a smaller pass rate.
      Check(zones == 256, "the suite describes 256 zones");
      Check(records == 352, "the suite describes 352 records");
   }

   void TestResultNames()
   {
      CheckEqual("none", ResultName(Result::None), "none");
      CheckEqual("neutral", ResultName(Result::Neutral), "neutral");
      CheckEqual("pass", ResultName(Result::Pass), "pass");
      CheckEqual("fail", ResultName(Result::Fail), "fail");
      CheckEqual("softfail", ResultName(Result::SoftFail), "softfail");
      CheckEqual("temperror", ResultName(Result::TempError), "temperror");
      CheckEqual("permerror", ResultName(Result::PermError), "permerror");
   }

   // The suite publishes most of its policies as records of the deprecated type
   // SPF rather than as TXT records, and expects a driver to present them as
   // the TXT records an RFC 7208 evaluator would find - except where a host
   // publishes a TXT record of its own, in which case the type-SPF records are
   // what the evaluator is supposed to ignore. The generator applies that rule;
   // these are the cases from the "Record lookup" and "Selecting records"
   // sections which exist to catch getting it wrong.
   void TestTypeSpfBecomesTxt()
   {
      const Section &lookup = FindSection("Record lookup");

      {
         // A TXT record and a type-SPF record saying the same thing. Only the
         // TXT record is there, otherwise there would appear to be two.
         ConformanceLookup resolver(lookup);
         std::vector<AnsiString> records;
         Check(resolver.GetTXTRecords("both.example.net", records), "both.example.net answers");
         Check(records.size() == 1, "both.example.net publishes one TXT record");
         if (records.size() == 1)
            CheckEqual("v=spf1 -all", records[0], "both.example.net");
      }

      {
         // A type-SPF record and "TXT: NONE", which is how the suite says the
         // host has no TXT record. An evaluator querying TXT learns nothing,
         // and RFC 7208 section 4.3 makes that none rather than an error.
         ConformanceLookup resolver(lookup);
         std::vector<AnsiString> records;
         Check(resolver.GetTXTRecords("spfonly.example.net", records), "spfonly.example.net answers");
         Check(records.empty(), "spfonly.example.net publishes no TXT record");
      }

      const Section &selecting = FindSection("Selecting records");

      {
         // Only a type-SPF record, so it stands in as the TXT record.
         ConformanceLookup resolver(selecting);
         std::vector<AnsiString> records;
         Check(resolver.GetTXTRecords("example1.com", records), "example1.com answers");
         Check(records.size() == 1, "example1.com publishes one TXT record");
         if (records.size() == 1)
            CheckEqual("v=spf1", records[0], "example1.com");
      }

      {
         // Two type-SPF records and a TXT record. The TXT record wins outright:
         // if the type-SPF records were carried over as well there would be
         // three, and the case expects the one.
         ConformanceLookup resolver(selecting);
         std::vector<AnsiString> records;
         Check(resolver.GetTXTRecords("example8.com", records), "example8.com answers");
         Check(records.size() == 1, "example8.com publishes one TXT record");
         if (records.size() == 1)
            CheckEqual("v=spf1 +all", records[0], "example8.com");
      }

      {
         // Two type-SPF records and no TXT record, so both stand in, and an
         // evaluator is supposed to call that a permerror.
         ConformanceLookup resolver(selecting);
         std::vector<AnsiString> records;
         Check(resolver.GetTXTRecords("example6.com", records), "example6.com answers");
         Check(records.size() == 2, "example6.com publishes two TXT records");
      }
   }

   // RFC 7208 section 3.3: a TXT record made of several character-strings is
   // read as those strings joined with nothing between them. The suite has a
   // case for each way that can go wrong.
   void TestCharacterStringsAreJoined()
   {
      const Section &selecting = FindSection("Selecting records");

      {
         // "v=spf1" and "mx", which join into "v=spf1mx". The version is not
         // terminated, so this is not an SPF record at all.
         ConformanceLookup resolver(selecting);
         std::vector<AnsiString> records;
         Check(resolver.GetTXTRecords("example2.com", records), "example2.com answers");
         Check(records.size() == 1, "example2.com publishes one TXT record");
         if (records.size() == 1)
            CheckEqual("v=spf1mx", records[0], "example2.com");
      }

      const Section &initial = FindSection("Initial processing");

      {
         // Two records: "v=spf1 ip4:" and "192.0.2.5 -all" joined, and one made
         // of no character-strings at all.
         ConformanceLookup resolver(initial);
         std::vector<AnsiString> records;
         Check(resolver.GetTXTRecords("null.example.com", records), "null.example.com answers");
         Check(records.size() == 2, "null.example.com publishes two TXT records");
         if (records.size() == 2)
         {
            CheckEqual("v=spf1 ip4:192.0.2.5 -all", records[0], "null.example.com, first");
            CheckEqual("", records[1], "null.example.com, second");
         }
      }
   }

   // RFC 7208 section 7.1 does not allow a NUL octet in a domain name, and the
   // suite has two policies which end in one, expecting a permerror. The octet
   // only reaches an evaluator because a character-string carries its length: a
   // record read as a C string would stop at the NUL, the policy would look
   // perfectly well formed, and the two cases would be decided on a record the
   // suite did not publish.
   void TestNulOctetsSurvive()
   {
      const char *sections[] = { "A mechanism syntax", "MX mechanism syntax" };
      const char *policies[] = { "v=spf1 a:foo.example.com", "v=spf1 mx:foo.example.com" };

      for (int i = 0; i < 2; i++)
      {
         ConformanceLookup resolver(FindSection(sections[i]));
         std::vector<AnsiString> records;
         Check(resolver.GetTXTRecords("e3.example.com", records),
               "e3.example.com answers");
         Check(records.size() == 1, "e3.example.com publishes one TXT record");

         if (records.size() != 1)
            continue;

         AnsiString expected = policies[i];
         expected += '\0';

         Check(records[0].GetLength() == expected.GetLength(),
               "the NUL octet is still in the policy");
         CheckEqual(expected, records[0], "the policy with its NUL octet");
      }
   }

   // A bare TIMEOUT in a zone stands for a DNS server which does not answer.
   // RFC 7208 section 4.4 makes that a temperror, and the suite has cases which
   // turn on a zone timing out for one record type while answering for another.
   void TestTimeouts()
   {
      const Section &lookup = FindSection("Record lookup");

      {
         // Nothing but a timeout.
         ConformanceLookup resolver(lookup);
         std::vector<AnsiString> records;
         Check(!resolver.GetTXTRecords("alltimeout.example.net", records),
               "alltimeout.example.net does not answer");
      }

      {
         // A type-SPF record, "TXT: NONE", and a timeout. The TXT query is the
         // one an RFC 7208 evaluator makes, and it is the one that times out.
         ConformanceLookup resolver(lookup);
         std::vector<AnsiString> records;
         Check(!resolver.GetTXTRecords("txttimeout.example.net", records),
               "txttimeout.example.net does not answer a TXT query");
      }

      {
         // A TXT record and a timeout. The TXT query is answered from the
         // record; a query for anything else is not answered at all. This is
         // the pair that a single flag for the whole zone would get wrong.
         ConformanceLookup resolver(lookup);
         std::vector<AnsiString> records;
         Check(resolver.GetTXTRecords("spftimeout.example.net", records),
               "spftimeout.example.net answers a TXT query");
         Check(records.size() == 1, "spftimeout.example.net publishes one TXT record");

         std::vector<AnsiString> addresses;
         Check(!resolver.GetARecords("spftimeout.example.net", addresses),
               "spftimeout.example.net does not answer an A query");
      }

      {
         // A name the section says nothing about. An empty answer, not a
         // failure: the host does not exist, which is not the same as not
         // knowing whether it does.
         ConformanceLookup resolver(lookup);
         std::vector<AnsiString> records;
         Check(resolver.GetTXTRecords("nothing.example.net", records),
               "a name outside the zone answers");
         Check(records.empty(), "a name outside the zone publishes nothing");
      }
   }

   // DNS ignores the case of ASCII letters in a name, and a trailing dot says
   // only that the name is already absolute. The suite leans on both: it writes
   // some zone names in mixed case, and some records point at a name which
   // differs from the zone it names in nothing but case and a dot.
   void TestNamesAreMatchedAsDnsMatchesThem()
   {
      const Section &selecting = FindSection("Selecting records");

      {
         ConformanceLookup resolver(selecting);
         std::vector<AnsiString> addresses;
         Check(resolver.GetARecords("MAIL.EXAMPLE1.COM", addresses),
               "a name in upper case answers");
         Check(addresses.size() == 1, "mail.example1.com has one address");
         if (addresses.size() == 1)
            CheckEqual("1.2.3.4", addresses[0], "mail.example1.com");
      }

      {
         ConformanceLookup resolver(selecting);
         std::vector<AnsiString> addresses;
         Check(resolver.GetARecords("mail.example1.com.", addresses),
               "a name with a trailing dot answers");
         Check(addresses.size() == 1, "a trailing dot names the same host");
      }

      const Section &ptr = FindSection("PTR mechanism syntax");

      {
         // The suite writes this reverse-mapping name with its hex nibbles in
         // upper case; an evaluator building it from an address would produce
         // lower case, or the other way round.
         ConformanceLookup resolver(ptr);
         std::vector<AnsiString> hostNames;
         Check(resolver.GetPTRRecords("1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.e.b.a.b.e.f.a.c.ip6.arpa", hostNames),
               "a reverse-mapping name in lower case answers");
         Check(hostNames.size() == 1, "the address maps back to one host");
         if (hostNames.size() == 1)
            CheckEqual("e3.example.com", hostNames[0], "the reverse mapping");
      }
   }

   // A reverse-mapping name can point at several hosts, and RFC 7208 section
   // 5.5 has the ptr mechanism check each of them.
   void TestReverseMappings()
   {
      const Section &ptr = FindSection("PTR mechanism syntax");

      ConformanceLookup resolver(ptr);
      std::vector<AnsiString> hostNames;
      Check(resolver.GetPTRRecords("4.3.2.1.in-addr.arpa", hostNames),
            "1.2.3.4 maps back");
      Check(hostNames.size() == 3, "1.2.3.4 maps back to three hosts");
   }

   // RFC 7208 section 5.4 walks a domain's MX records; DNSResolver hands its
   // callers the host names in preference order, and the table is built that
   // way, which matters where the number of names is over the section 4.6.4
   // limit and only the first ten are looked at.
   void TestMxPreferenceOrder()
   {
      const Section &limits = FindSection("Processing limits");

      ConformanceLookup resolver(limits);
      std::vector<AnsiString> hostNames;
      Check(resolver.GetMXRecords("e4.example.com", hostNames), "e4.example.com answers");
      Check(hostNames.size() == 11, "e4.example.com names eleven mail exchangers");

      if (hostNames.size() == 11)
      {
         // Preferences nought to nine, then the domain itself at ten.
         for (int i = 0; i < 10; i++)
            CheckEqual("mail.example.com", hostNames[i], "a mail exchanger of e4.example.com");

         CheckEqual("e4.example.com", hostNames[10], "the last mail exchanger of e4.example.com");
      }
   }

   // A CNAME is followed one step, which is what the suite's own driver does.
   void TestAliases()
   {
      const Section &bugs = FindSection("Test cases from implementation bugs");

      {
         // b.example.org is an alias for a.example.org, which publishes four
         // TXT records, one of them a policy. The case exists because an
         // implementation which followed the alias twice found the policy twice
         // and called it a permerror.
         ConformanceLookup resolver(bugs);
         std::vector<AnsiString> records;
         Check(resolver.GetTXTRecords("b.example.org", records), "b.example.org answers");
         Check(records.size() == 4, "an alias answers with what it points at");
      }

      const Section &ptr = FindSection("PTR mechanism syntax");

      {
         // cname.example.com is an alias for itself, spelled with a trailing
         // dot and a different case. Following it even once more would not
         // terminate.
         ConformanceLookup resolver(ptr);
         std::vector<AnsiString> addresses;
         Check(resolver.GetARecords("cname.example.com", addresses),
               "an alias for itself answers");
         Check(addresses.empty(), "an alias for itself has no address");
      }

      {
         // And an alias for that alias.
         ConformanceLookup resolver(ptr);
         std::vector<AnsiString> addresses;
         Check(resolver.GetARecords("loop4.example.com", addresses),
               "an alias for an alias for itself answers");
         Check(addresses.empty(), "an alias for an alias for itself has no address");
      }
   }

   // Every query is counted, so that the lookup limits of RFC 7208 section
   // 4.6.4 can be checked against the resolver rather than inferred from a
   // result.
   void TestQueriesAreCounted()
   {
      const Section &selecting = FindSection("Selecting records");

      ConformanceLookup resolver(selecting);
      Check(resolver.GetQueryCount() == 0, "a fresh resolver has answered nothing");

      std::vector<AnsiString> records;
      Check(resolver.GetTXTRecords("example1.com", records), "example1.com answers");
      Check(resolver.GetQueryCount() == 1, "one query has been answered");

      Check(resolver.GetTXTRecords("nothing.example.com", records),
            "a name outside the zone answers");
      Check(resolver.GetQueryCount() == 2, "a name outside the zone is still a query");
   }

   // Where the cases will be run once there is something to run them against.
   // Each is an evaluation of Case::mailFrom from Case::clientIp, announced as
   // Case::heloHost, against a ConformanceLookup over the case's own section;
   // it passes if the result is one of Case::acceptedResults and, where
   // Case::explanation is set, the explanation matches it.
   int RunCases_()
   {
      return 0;
   }
}

int main()
{
   TestTableIsWhole();
   TestResultNames();
   TestTypeSpfBecomesTxt();
   TestCharacterStringsAreJoined();
   TestNulOctetsSurvive();
   TestTimeouts();
   TestNamesAreMatchedAsDnsMatchesThem();
   TestReverseMappings();
   TestMxPreferenceOrder();
   TestAliases();
   TestQueriesAreCounted();

   int ran = RunCases_();

   if (failures > 0)
   {
      printf("%d check(s) failed.\n", failures);
      return 1;
   }

   printf("The harness is sound. %d of %d conformance cases were run; the rest\n"
          "wait for an evaluator to run them against.\n", ran, CaseCount);

   return 0;
}
