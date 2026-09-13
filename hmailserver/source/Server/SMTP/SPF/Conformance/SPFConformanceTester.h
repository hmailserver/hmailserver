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
      // What it runs today is the harness rather than an evaluation: that the
      // table transcribed from rfc7208-tests.yml is whole, and that the stub
      // resolver reads the suite's zones the way the suite means them. There is
      // no SPF evaluator to hand the cases to yet - the record parser, the macro
      // expander and the evaluator come after this - so no case is decided, and
      // Run() says how many of them it decided. RunCases_ is where they will be
      // driven from.
      //
      // The resolver is worth pinning down ahead of the evaluator because its
      // rules are not obvious - a zone that times out for one record type and
      // answers for another, host names differing only in case, an alias for
      // itself - and if one of them is wrong, every case leaning on it fails for
      // a reason that looks like the evaluator's fault.
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

         // How many of the suite's cases the last run actually decided. Zero
         // until there is an evaluator to decide them.
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
