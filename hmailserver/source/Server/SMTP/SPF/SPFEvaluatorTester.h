// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

namespace HM
{
   // The corners of RFC 7208 section 4 which the conformance suite next to this
   // does not reach.
   //
   // The suite is the better witness that the evaluator is right, and it decides
   // all 203 of its cases; this is for what it happens not to cover. Three sorts
   // of thing turned up when the suite's own cases were used to check each other,
   // by breaking a behaviour and seeing whether any case noticed:
   //
   //   - A lookup that fails. Sections 5.3 and 5.4 make a failed A, AAAA or MX
   //     query a temperror, but the suite only has a case for exists, so
   //     answering a permerror instead went unnoticed.
   //   - The term counting of section 4.6.4. The suite's mech-over-limit spends
   //     its budget on names that do not exist, so it trips the void-lookup limit
   //     before the term limit, and its redirect-loop recurses rather than
   //     counting; neither notices a term that is not counted at all.
   //   - What the resolver hands back. Every address in the suite's zones is one,
   //     so a match against something that is not went unnoticed.
   //
   // Failures are returned rather than reported, for the same reasons as
   // SPFRecordTester: where they belong differs between the server build and the
   // portable one, and a run should report all of them.
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

      std::vector<AnsiString> failures_;
   };
}
