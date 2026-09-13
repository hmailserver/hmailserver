// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

namespace HM
{
   // Macro expansion, RFC 7208 section 7, and the address forms it is built on, a
   // rule at a time: a wrong expansion usually produces a name that does not exist,
   // so the suite cannot see which macro was wrong. Asserted against section 7.4.
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
