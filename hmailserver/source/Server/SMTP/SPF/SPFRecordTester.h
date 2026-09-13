// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

namespace HM
{
   // The grammar of RFC 7208 checked directly, a rule at a time.
   //
   // The conformance suite next to this exercises the same code end to end, and
   // is the better witness that the parser is right; what this adds is the
   // corners the suite happens not to visit, and a failure that names the rule
   // rather than a case whose result changed.
   //
   // Failures are returned rather than reported, for the same reasons as
   // SPFConformanceTester: where they belong differs between the server build
   // and the portable one, and a run should report all of them.
   class SPFRecordTester
   {
   public:

      std::vector<AnsiString> Run();

   private:

      void CheckMacroString_(const AnsiString &text, bool valid);
      void CheckExplanationText_(const AnsiString &text, bool valid);
      void CheckDomainSpec_(const AnsiString &text, bool valid);
      void CheckIP4_(const AnsiString &text, bool valid);
      void CheckIP6_(const AnsiString &text, bool valid);
      void CheckDomainName_(const AnsiString &text, bool valid);

      // Parses a record and reports it if the outcome is not the expected one.
      // Returns true when the record parsed, so that a caller can go on to look
      // at what came out.
      bool CheckRecord_(const AnsiString &record, bool valid);

      void Report_(const AnsiString &what, const AnsiString &subject, bool valid);

      void TestVersion_();
      void TestMacroStrings_();
      void TestDomainSpecs_();
      void TestAddressLiterals_();
      void TestDomainNames_();
      void TestMechanisms_();
      void TestPrefixLengths_();
      void TestModifiers_();
      void TestWholeRecords_();

      std::vector<AnsiString> failures_;
   };
}
