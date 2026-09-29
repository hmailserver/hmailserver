// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

namespace HM
{
   namespace SPFConformance
   {
      struct Section;

      // Runs the openspf.org conformance suite for RFC 7208, vendored in this directory,
      // and the harness itself: that the table transcribed from rfc7208-tests.yml is
      // whole, and that the stub resolver reads the suite's zones as the suite means them.
      class SPFConformanceTester
      {
      public:

         SPFConformanceTester();

         // Runs everything and returns one line per failure, naming the case and what it
         // expected. Empty means everything passed. Returned rather than reported, because
         // where a failure belongs differs between the two hosts - see README.md.
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
