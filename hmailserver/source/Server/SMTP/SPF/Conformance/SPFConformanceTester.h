// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

namespace HM
{
   namespace SPFConformance
   {
      struct Section;

      // Runs the openspf.org conformance suite for RFC 7208, which is vendored
      // in this directory. Built by both the server project and the portable
      // build, and run from both: ClassTester runs it under Visual Studio, where
      // a case can be stepped through, and the portable runner runs it under the
      // address and undefined-behaviour sanitizers. The cases are the same
      // either way; nothing here reaches a network, so a whole run takes
      // milliseconds.
      //
      // Alongside the cases it runs the harness itself: that the table
      // transcribed from rfc7208-tests.yml is whole, and that the stub resolver
      // reads the suite's zones the way the suite means them. The resolver is
      // worth pinning down separately because its rules are not obvious - a zone
      // that times out for one record type and answers for another, host names
      // differing only in case, an alias for itself - and if one of them is
      // wrong, every case leaning on it fails for a reason that looks like the
      // evaluator's fault.
      //
      // Not every case is decided yet. Finding and parsing a domain's record
      // settles the ones whose answer does not depend on a mechanism, which is
      // what RunCases_ does; the rest wait on the evaluator. What SPFRecord and
      // SPFMacroExpander do beyond that is asserted directly, by SPFRecordTester
      // and SPFMacroExpanderTester, because a case the suite leaves undecided
      // says nothing either way. Run() reports how many were decided, and
      // RunCases_ holds that count to a floor: a parser which accepts too much
      // does not fail a case, it quietly stops deciding it.
      class SPFConformanceTester
      {
      public:

         SPFConformanceTester();

         // Runs everything and returns one line per failure, naming the case and
         // what it expected. Empty means everything passed.
         //
         // Failures are returned rather than reported, because where they should
         // go differs: the server build sends them to the debugger, and the
         // portable build to standard output. Returning them also means a run
         // reports every failure it found rather than stopping at the first,
         // which is what makes 203 cases worth having.
         std::vector<AnsiString> Run();

         // How many of the suite's cases the last run actually decided. Rises as
         // more of the evaluation is written; a fall means something that used to
         // be recognised no longer is.
         int GetCasesRun() const { return casesRun_; }

         // How many cases the suite holds, decided or not.
         static int GetCaseCount();

      private:

         void Check_(bool condition, const AnsiString &what);
         void CheckEqual_(const AnsiString &expected, const AnsiString &actual, const AnsiString &what);

         const Section &FindSection_(const AnsiString &description);

         void TestTableIsWhole_();
         void TestResultNames_();
         void TestTypeSpfBecomesTxt_();
         void TestCharacterStringsAreJoined_();
         void TestNulOctetsSurvive_();
         void TestTimeouts_();
         void TestNamesAreMatchedAsDnsMatchesThem_();
         void TestReverseMappings_();
         void TestMxPreferenceOrder_();
         void TestAliases_();
         void TestQueriesAreCounted_();

         int RunCases_();

         std::vector<AnsiString> failures_;
         int casesRun_;
      };
   }
}
