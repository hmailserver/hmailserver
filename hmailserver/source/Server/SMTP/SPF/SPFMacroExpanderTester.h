// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

namespace HM
{
   // The macro expansion of RFC 7208 section 7, and the address forms it is
   // built on, checked a rule at a time.
   //
   // Most of what section 7 says is not visible in the conformance suite's
   // results: a macro which expands wrongly usually produces a name which does
   // not exist, and the case then fails for a reason that says nothing about
   // which macro was wrong. The cases that do pin the text - the ones which
   // assert an explanation - cover nine macros between them and none of the
   // transformers on their own. So the expansions are asserted directly here,
   // against the examples of section 7.4 and against the suite's own zone data,
   // and the suite is left to say that the whole works end to end.
   //
   // Failures are returned rather than reported, for the same reasons as
   // SPFRecordTester: where they belong differs between the server build and the
   // portable one, and a run should report all of them rather than stop at the
   // first.
   class SPFMacroExpanderTester
   {
   public:

      std::vector<AnsiString> Run();

   private:

      void TestAddressForms_();
      void TestPrefixMatching_();
      void TestSpecExamples_();
      void TestTransformers_();
      void TestLiterals_();
      void TestUrlEscaping_();
      void TestPostmasterDefaults_();
      void TestExplanationText_();
      void TestValidatedName_();
      void TestTruncation_();

      std::vector<AnsiString> failures_;
   };
}
