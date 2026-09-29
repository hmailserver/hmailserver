// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

// The openspf.org test suite for RFC 7208, as a C++ table, transcribed from
// rfc7208-tests.yml by Generate-SPFConformanceSuite.py. Data only: nothing here
// knows what hMailServer's evaluator looks like. README.md says where it is from.

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

      // The record types a zone in the suite can hold. Type SPF, the deprecated RR type
      // 99 most of the suite's policies are written as, is absent: section 3.1 removed
      // it, and the generator has already turned those into TXT records.
      enum class RecordType
      {
         A,
         AAAA,
         CNAME,
         MX,
         PTR,
         TXT
      };

      // One character-string of a record as it came off the wire, not text. Two of the
      // suite's policies end in a NUL and several hold bytes above 0x7f, which an
      // evaluator has to be handed in order to reject - sections 3.1 and 7.1.
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

         // The explanation the evaluation must produce, or 0 where the case asserts nothing
         // about it. Empty means the record supplied none and the receiver uses its own,
         // which hMailServer reports by producing no explanation at all.
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
