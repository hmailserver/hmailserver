// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

// The openspf.org test suite for RFC 7208, as a C++ table.
//
// The suite is vendored in this directory as rfc7208-tests.yml, and
// Generate-SPFConformanceSuite.py transcribes it into SPFConformanceSuite.cpp.
// The generated file is committed, so that building hMailServer needs neither
// Python nor a YAML parser. README.md says where the suite comes from.
//
// The table is data only. Nothing here knows what hMailServer's SPF evaluator
// looks like, on purpose: a transcription of someone else's file should say
// what that file says. The runner is where the two meet.

#pragma once

namespace HM
{
   namespace SPFConformance
   {
      // The results of RFC 7208 section 2.6, as the suite spells them.
      // Deliberately not HM::SPF::Result: this is what the suite asks for, and
      // the runner is what maps it onto what hMailServer answers.
      enum class Result
      {
         None,
         Neutral,
         Pass,
         Fail,
         SoftFail,
         TempError,
         PermError
      };

      // The suite's name for a result, for failure messages.
      const char *ResultName(Result result);

      // The record types a zone in the suite can hold. Type SPF, the
      // deprecated RR type 99 that most of the suite's policies are written
      // as, is absent: RFC 7208 section 3.1 removed it, and the generator has
      // already turned those records into the TXT records an evaluator
      // following RFC 7208 would find. See the generator for the rule.
      enum class RecordType
      {
         A,
         AAAA,
         CNAME,
         MX,
         PTR,
         TXT
      };

      // One character-string of a record. Bytes and a length rather than a C
      // string: two of the suite's policies end in a NUL octet, which RFC 7208
      // section 7.1 does not allow in a domain name and which an evaluator has
      // to be handed in order to reject. Several others hold bytes above 0x7f,
      // for the same reason - section 3.1 restricts a policy to 7-bit ASCII.
      // This is a record as it came off the wire, not text.
      struct CharacterString
      {
         const char *bytes;
         int length;
      };

      struct Record
      {
         RecordType type;

         // A TXT record is a sequence of character-strings, which RFC 7208
         // section 3.3 joins with nothing between them; every other type
         // carries exactly one value.
         const CharacterString *strings;
         int stringCount;
      };

      struct Zone
      {
         // Written as the suite writes it, which is not always in lower case.
         // Comparisons against it are case insensitive, as DNS is.
         const char *name;

         const Record *records;
         int recordCount;

         // The zone answers a query for a type it holds no record of with a
         // timeout rather than with an empty answer.
         bool timesOut;
      };

      struct Case
      {
         // The key the case is written under in rfc7208-tests.yml. Its
         // description and the commentary on what it is for are there; a
         // failure message names the case and leaves the reader to look.
         const char *name;

         // The part of RFC 7208 under test, in the suite's own notation.
         const char *spec;

         // The arguments to an evaluation: the client address, the sender
         // (which may be empty, or have no local part), and the argument of
         // the HELO or EHLO command.
         const char *clientIp;
         const char *mailFrom;
         const char *heloHost;

         // The results the suite accepts. More than one where RFC 7208 leaves
         // the answer to the implementation; the first is the one the suite
         // prefers.
         const Result *acceptedResults;
         int acceptedResultCount;

         // The explanation the evaluation must produce, or 0 where the case
         // asserts nothing about it. An empty string means the record supplied
         // no explanation and the receiver is left to use its own, which
         // hMailServer reports by producing no explanation at all.
         const char *explanation;
      };

      // A section of the suite. Each has a zone of its own: the same host name
      // means different things in different sections.
      struct Section
      {
         const char *description;

         const Zone *zones;
         int zoneCount;

         const Case *cases;
         int caseCount;
      };

      extern const Section Sections[];
      extern const int SectionCount;

      // The number of cases across all sections, so that a runner can say it
      // ran them all.
      extern const int CaseCount;
   }
}
