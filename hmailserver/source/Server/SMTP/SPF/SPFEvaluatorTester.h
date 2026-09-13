// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

namespace HM
{
   // The corners of RFC 7208 section 4 the conformance suite beside this does not
   // reach - README.md lists them. Failures are returned rather than reported, for
   // the reasons SPFRecordTester's are.
   class SPFEvaluatorTester
   {
   public:

      std::vector<AnsiString> Run();

   private:

      void TestTemporaryFailures_();
      void TestTermCounting_();
      void TestTermLimit_();
      void TestResolverAnswers_();
      void TestExplanationOnlyForFail_();
      void TestVoidLookupsAreCountedPerTerm_();
      void TestIncludeDoesNotFetchAnExplanation_();
      void TestUnusableTargetNames_();

      std::vector<AnsiString> failures_;
   };
}
