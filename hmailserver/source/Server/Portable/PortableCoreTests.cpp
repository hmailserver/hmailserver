// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

// Test runner for the portable build.
//
// Sources under Common that carry no Windows dependency are compiled into
// hmailserver-core and exercised from here, so that they can be run under the
// address and undefined-behaviour sanitizers on a machine with no Windows SDK.
// The server project builds the same sources for the service itself; this adds
// a second compiler and a second way of running them, it does not replace the
// regression tests.
//
// Add a source to CMakeLists.txt and a case here as each becomes buildable
// without the Windows headers.

#include "stdafx.h"

#include "../Common/Mime/MimeChar.h"
#include "../SMTP/SPF/Conformance/SPFConformanceTester.h"
#include "../SMTP/SPF/SPFEvaluatorTester.h"
#include "../SMTP/SPF/SPFMacroExpanderTester.h"
#include "../SMTP/SPF/SPFRecordTester.h"

#include <cstdio>
#include <cstring>

namespace
{
   int failures = 0;

   void Check(bool condition, const char *what)
   {
      if (condition)
         return;

      printf("FAILED: %s\n", what);
      failures++;
   }

   // RFC 2822 section 3.2.1, and the table in MimeChar.cpp.
   void TestMimeChar()
   {
      Check(HM::CMimeChar::IsSpace(' '), "space is space");
      Check(HM::CMimeChar::IsSpace('\t'), "tab is space");
      Check(!HM::CMimeChar::IsSpace('a'), "a is not space");

      Check(HM::CMimeChar::IsControl('\0'), "NUL is control");
      Check(HM::CMimeChar::IsControl('\x1f'), "0x1f is control");
      Check(!HM::CMimeChar::IsControl(' '), "space is not control");

      Check(HM::CMimeChar::IsPrintable('a'), "a is printable");
      Check(!HM::CMimeChar::IsPrintable('\x7f'), "DEL is not printable");

      // The specials of RFC 2822, which may not appear unquoted in an atom.
      const char *specials = "()<>@,;:\\\".[]";
      for (const char *p = specials; *p != 0; p++)
      {
         Check(HM::CMimeChar::IsSpecial((unsigned char) *p), "special is special");
         Check(!HM::CMimeChar::IsToken((unsigned char) *p), "special is not a token char");
      }

      Check(!HM::CMimeChar::IsSpecial('a'), "a is not special");
      Check(HM::CMimeChar::IsToken('a'), "a is a token char");
      Check(!HM::CMimeChar::IsToken(' '), "space is not a token char");

      for (unsigned char c = '0'; c <= '9'; c++)
         Check(HM::CMimeChar::IsHexDigit(c), "digit is a hex digit");
      for (unsigned char c = 'A'; c <= 'F'; c++)
         Check(HM::CMimeChar::IsHexDigit(c), "A-F is a hex digit");
      Check(!HM::CMimeChar::IsHexDigit('g'), "g is not a hex digit");

      // a-f are deliberately not asserted either way. The table covers only
      // 0-9 and A-F, matching the uppercase that RFC 2045 requires a
      // quoted-printable encoder to produce. Whether the decoder should accept
      // lowercase from encoders that emit it anyway is a question about
      // MimeCode.cpp, not about this table.

      // Everything at or above 0x80 is non-ASCII, and the table is not consulted
      // for it. Walking the whole range also gives the sanitizers a chance to
      // notice an index running off the 128-entry table.
      for (int c = 0; c <= 255; c++)
      {
         unsigned char ch = (unsigned char) c;
         bool nonAscii = HM::CMimeChar::IsNonAscii(ch);

         Check(nonAscii == (c >= 0x80), "non-ascii is exactly 0x80 and above");

         if (nonAscii)
         {
            Check(!HM::CMimeChar::IsControl(ch), "non-ascii is never control");
            Check(!HM::CMimeChar::IsSpace(ch), "non-ascii is never space");
            Check(HM::CMimeChar::IsToken(ch), "non-ascii is always a token char");
         }

         Check(HM::CMimeChar::IsDelimiter(ch) ==
               (HM::CMimeChar::IsSpace(ch) || HM::CMimeChar::IsSpecial(ch)),
               "delimiter is space or special");
      }
   }

   void ReportFailures(const std::vector<HM::AnsiString> &reported)
   {
      for (const HM::AnsiString &failure : reported)
      {
         printf("FAILED: %s\n", failure.c_str());
         failures++;
      }
   }

   // The SPF record grammar, the macro expansion of RFC 7208 section 7, and the
   // RFC 7208 conformance suite. All three are built into the server project as
   // well and run from ClassTester there; the cases are the same either way, and
   // none of them reaches a network, so together they cost milliseconds.
   void TestSPF()
   {
      HM::SPFRecordTester recordTester;
      ReportFailures(recordTester.Run());

      HM::SPFMacroExpanderTester macroExpanderTester;
      ReportFailures(macroExpanderTester.Run());

      HM::SPFEvaluatorTester evaluatorTester;
      ReportFailures(evaluatorTester.Run());

      HM::SPFConformance::SPFConformanceTester conformanceTester;
      ReportFailures(conformanceTester.Run());

      printf("SPF conformance: %d of %d cases decided.\n",
             conformanceTester.GetCasesRun(),
             HM::SPFConformance::SPFConformanceTester::GetCaseCount());
   }
}

int main()
{
   TestMimeChar();
   TestSPF();

   if (failures > 0)
   {
      printf("%d check(s) failed.\n", failures);
      return 1;
   }

   printf("All checks passed.\n");
   return 0;
}
