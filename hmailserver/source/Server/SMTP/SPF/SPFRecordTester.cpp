// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "stdafx.h"

#include "SPFRecordTester.h"

#include "SPFRecord.h"
#include "SPFSyntax.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   void
   SPFRecordTester::Report_(const AnsiString &what, const AnsiString &subject, bool valid)
   {
      failures_.push_back(AnsiString(valid ? "Rejected a valid " : "Accepted an invalid ") +
                          what + ": \"" + subject + "\"");
   }

   void
   SPFRecordTester::CheckMacroString_(const AnsiString &text, bool valid)
   {
      if (SPFSyntax::IsValidMacroString(text, SPFSyntax::MacroSet::RecordTerm) != valid)
         Report_("macro-string", text, valid);
   }

   void
   SPFRecordTester::CheckExplanationText_(const AnsiString &text, bool valid)
   {
      if (SPFSyntax::IsValidMacroString(text, SPFSyntax::MacroSet::ExplanationText) != valid)
         Report_("explanation macro-string", text, valid);
   }

   void
   SPFRecordTester::CheckDomainSpec_(const AnsiString &text, bool valid)
   {
      if (SPFSyntax::IsValidDomainSpec(text) != valid)
         Report_("domain-spec", text, valid);
   }

   void
   SPFRecordTester::CheckIP4_(const AnsiString &text, bool valid)
   {
      if (SPFSyntax::IsValidIP4Network(text) != valid)
         Report_("ip4-network", text, valid);
   }

   void
   SPFRecordTester::CheckIP6_(const AnsiString &text, bool valid)
   {
      if (SPFSyntax::IsValidIP6Network(text) != valid)
         Report_("ip6-network", text, valid);
   }

   void
   SPFRecordTester::CheckDomainName_(const AnsiString &text, bool valid)
   {
      if (SPFSyntax::IsValidDomainName(text) != valid)
         Report_("domain name", text, valid);
   }

   bool
   SPFRecordTester::CheckRecord_(const AnsiString &record, bool valid)
   {
      SPFRecord parsed;
      AnsiString error;

      bool parsedOk = SPFRecord::Parse(record, parsed, error);

      if (parsedOk != valid)
      {
         Report_("record", record, valid);

         if (!valid)
            return parsedOk;

         // Say why, since the record was supposed to be fine.
         failures_.push_back(AnsiString("  the parser said: ") + error);
      }

      return parsedOk;
   }

   // Section 4.5: the version, compared without regard to case, and terminated
   // by a space or by the end of the record.
   void
   SPFRecordTester::TestVersion_()
   {
      if (!SPFRecord::HasVersionTag("v=spf1"))
         failures_.push_back("\"v=spf1\" alone is a record.");

      if (!SPFRecord::HasVersionTag("v=spf1 -all"))
         failures_.push_back("\"v=spf1 -all\" is a record.");

      if (!SPFRecord::HasVersionTag("V=sPf1 +all"))
         failures_.push_back("The version is compared without regard to case.");

      if (SPFRecord::HasVersionTag("v=spf10"))
         failures_.push_back("\"v=spf10\" is a record about something else.");

      if (SPFRecord::HasVersionTag("v=spf1mx"))
         failures_.push_back("\"v=spf1mx\" does not terminate the version.");

      if (SPFRecord::HasVersionTag("spf1"))
         failures_.push_back("\"spf1\" is missing the version tag.");

      if (SPFRecord::HasVersionTag(""))
         failures_.push_back("An empty record has no version.");

      if (SPFRecord::HasVersionTag(" v=spf1"))
         failures_.push_back("The version has to come first.");
   }

   // Section 7.1: macro-string and the macros it may name.
   void
   SPFRecordTester::TestMacroStrings_()
   {
      CheckMacroString_("", true);
      CheckMacroString_("plain", true);
      CheckMacroString_("man:dog/cat", true);
      CheckMacroString_("+", true);

      // Every macro a record's own terms may use.
      CheckMacroString_("%{s}%{l}%{o}%{d}%{i}%{p}%{h}%{v}", true);

      // The three shorthands, section 7.1.
      CheckMacroString_("%%", true);
      CheckMacroString_("%_", true);
      CheckMacroString_("%-", true);
      CheckMacroString_("macro%%percent%_%_space%-url-space", true);

      // Transformers: a count, an "r", and delimiters, in that order.
      CheckMacroString_("%{d2}", true);
      CheckMacroString_("%{dr}", true);
      CheckMacroString_("%{d2r}", true);
      CheckMacroString_("%{l2r-}", true);
      CheckMacroString_("%{l2r+-}", true);
      CheckMacroString_("%{s.-+,/_=}", true);

      // Upper case names the same macro, and asks for the expansion to be URL
      // escaped.
      CheckMacroString_("%{D2}", true);
      CheckMacroString_("%{L}", true);

      // A percent sign has to introduce one of the four forms.
      CheckMacroString_("%", false);
      CheckMacroString_("%a", false);
      CheckMacroString_("%abc", false);
      CheckMacroString_("foo%.bar", false);
      CheckMacroString_("%(ir)", false);
      CheckMacroString_("foo%(ir).bar", false);
      CheckMacroString_("%{l", false);
      CheckMacroString_("%{}", false);
      CheckMacroString_("%{z}", false);
      CheckMacroString_("%{d2x}", false);

      // A space is not a macro-literal, so it cannot appear inside a term.
      CheckMacroString_("two words", false);

      // Section 7.2: c, r and t belong to an explanation and nowhere else.
      CheckMacroString_("%{c}", false);
      CheckMacroString_("%{r}", false);
      CheckMacroString_("%{t}", false);

      CheckExplanationText_("%{c}", true);
      CheckExplanationText_("%{r}", true);
      CheckExplanationText_("%{t}", true);
      CheckExplanationText_("%{ir}.%{v}.arpa", true);
      CheckExplanationText_("%{z}", false);
   }

   // Section 12: domain-spec is a macro-string which ends in a top label or in a
   // macro expansion.
   void
   SPFRecordTester::TestDomainSpecs_()
   {
      CheckDomainSpec_("foo.example.com", true);
      CheckDomainSpec_("example.com.", true);
      CheckDomainSpec_("a.b.c.d.example", true);

      // A top label may hold digits, so long as it is not all digits.
      CheckDomainSpec_("example.c0m", true);
      CheckDomainSpec_("example.xn--zckzah", true);
      CheckDomainSpec_("example.a-b", true);

      // Ending in a macro expansion is the other way to end, so no dot is
      // needed at all.
      CheckDomainSpec_("%{H}", true);
      CheckDomainSpec_("%{d}", true);
      CheckDomainSpec_("_spfh.%{d2}", true);
      CheckDomainSpec_("%{i}.%{l2r-}.user.%{d2}", true);

      // Characters that would be odd in a name are still macro-literals, and a
      // domain-spec is checked as one. The name this expands to is a different
      // question, asked later and answered by the DNS.
      CheckDomainSpec_("foo:bar/baz.example.com", true);
      CheckDomainSpec_("mail.example...com", true);
      CheckDomainSpec_("a123456789012345678901234567890123456789012345678901234567890123.example.com", true);

      CheckDomainSpec_("", false);

      // Nothing that could be a top label.
      CheckDomainSpec_("museum", false);
      CheckDomainSpec_("museum.", false);
      CheckDomainSpec_("foo-bar", false);

      // A top label may not be all digits, or the name could be read as an
      // address.
      CheckDomainSpec_("111.222.33.44", false);
      CheckDomainSpec_("abc.123", false);

      // A hyphen may sit inside a top label but not at either end.
      CheckDomainSpec_("example.-com", false);
      CheckDomainSpec_("example.com-", false);

      // The last label has to be the top label, so anything after it is a
      // mistake.
      CheckDomainSpec_("example.com:8080", false);

      CheckDomainSpec_("%{r}.example.com", false);
      CheckDomainSpec_("%{a}.example.com", false);
      CheckDomainSpec_("-all", false);
   }

   // Section 12 for ip4-network, and RFC 4291 section 2.2 for ip6-network.
   void
   SPFRecordTester::TestAddressLiterals_()
   {
      CheckIP4_("1.2.3.4", true);
      CheckIP4_("0.0.0.0", true);
      CheckIP4_("255.255.255.255", true);
      CheckIP4_("192.0.2.5", true);

      CheckIP4_("", false);
      CheckIP4_("1.2.3", false);
      CheckIP4_("1.2.3.4.5", false);
      CheckIP4_("1.2.3.", false);
      CheckIP4_("256.1.1.1", false);
      CheckIP4_("1.2.3.444", false);
      CheckIP4_("1.2.3.4:8080", false);

      // Section 12 spells the digits out, and a leading zero is not among them.
      CheckIP4_("1.2.03.4", false);
      CheckIP4_("01.2.3.4", false);

      CheckIP6_("::", true);
      CheckIP6_("::1", true);
      CheckIP6_("1::", true);
      CheckIP6_("2001:db8::1", true);
      CheckIP6_("1:2:3:4:5:6:7:8", true);
      CheckIP6_("1:2:3:4:5:6:7::", true);
      CheckIP6_("CAFE:BABE::1", true);
      CheckIP6_("Cafe:Babe:8000::", true);

      // The form that ends in a dotted-quad, standing for the last two groups.
      CheckIP6_("::1.1.1.1", true);
      CheckIP6_("::FFFF:1.2.3.4", true);
      CheckIP6_("1:2:3:4:5:6:1.2.3.4", true);

      CheckIP6_("", false);
      CheckIP6_("1:2:3:4:5:6:7", false);
      CheckIP6_("1:2:3:4:5:6:7:8:9", false);
      CheckIP6_("1:2:3:4:5:6:7:8::", false);

      // One run of "::" and no more, and no stray colon at either end.
      CheckIP6_("1::2::3", false);
      CheckIP6_(":CAFE::BABE", false);
      CheckIP6_(":1", false);
      CheckIP6_("1:", false);
      CheckIP6_("1:::2", false);

      CheckIP6_("12345::", false);
      CheckIP6_("gggg::", false);

      // A dotted-quad stands for the last two groups, so it has to be last, and
      // it has to be a dotted-quad.
      CheckIP6_("1.2.3.4::1", false);
      CheckIP6_("::1.2.3", false);
      CheckIP6_("::1.2.3.4.5", false);
      CheckIP6_("1:2:3:4:5:6:7:1.2.3.4", false);
   }

   // Section 4.3: whether a DNS query can be built from a name.
   void
   SPFRecordTester::TestDomainNames_()
   {
      CheckDomainName_("example.com", true);
      CheckDomainName_("example.com.", true);
      CheckDomainName_("a.b.c.example.com", true);
      CheckDomainName_("a12345678901234567890123456789012345678901234567890123456789012.example.com", true);

      CheckDomainName_("", false);

      // A single label is not a fully qualified name.
      CheckDomainName_("A2345678", false);
      CheckDomainName_("localhost", false);

      // An empty label anywhere but the end, and a label over 63 characters.
      CheckDomainName_("a..example.com", false);
      CheckDomainName_(".example.com", false);
      CheckDomainName_("A123456789012345678901234567890123456789012345678901234567890123.example.com", false);
   }

   // Section 5: what each mechanism may carry after its name.
   void
   SPFRecordTester::TestMechanisms_()
   {
      // Section 5.1: all takes nothing.
      CheckRecord_("v=spf1 all", true);
      CheckRecord_("v=spf1 -all", true);
      CheckRecord_("v=spf1 all -all", true);
      CheckRecord_("v=spf1 -all.", false);
      CheckRecord_("v=spf1 -all:foobar", false);
      CheckRecord_("v=spf1 -all/8", false);

      // Every qualifier of section 4.6.2, and the default when none is written.
      {
         SPFRecord record;
         AnsiString error;

         if (!SPFRecord::Parse("v=spf1 all +all -all ~all ?all", record, error))
            failures_.push_back("A record of one mechanism per qualifier should parse.");
         else if (record.GetMechanisms().size() != 5)
            failures_.push_back("A record of five directives should hold five.");
         else
         {
            const std::vector<SPFMechanism> &mechanisms = record.GetMechanisms();

            if (mechanisms[0].GetQualifier() != SPFMechanism::Qualifier::Pass)
               failures_.push_back("A directive with no qualifier passes.");
            if (mechanisms[1].GetQualifier() != SPFMechanism::Qualifier::Pass)
               failures_.push_back("\"+\" passes.");
            if (mechanisms[2].GetQualifier() != SPFMechanism::Qualifier::Fail)
               failures_.push_back("\"-\" fails.");
            if (mechanisms[3].GetQualifier() != SPFMechanism::Qualifier::SoftFail)
               failures_.push_back("\"~\" soft fails.");
            if (mechanisms[4].GetQualifier() != SPFMechanism::Qualifier::Neutral)
               failures_.push_back("\"?\" is neutral.");
         }
      }

      // Sections 5.2 and 5.7: include and exists must name a domain, and carry
      // no prefix length.
      CheckRecord_("v=spf1 include:example.com -all", true);
      CheckRecord_("v=spf1 exists:example.com", true);
      CheckRecord_("v=spf1 include -all", false);
      CheckRecord_("v=spf1 include: -all", false);
      CheckRecord_("v=spf1 exists:", false);
      CheckRecord_("v=spf1 include:example.com/24 -all", false);
      CheckRecord_("v=spf1 exists:example.com/24", false);

      // Sections 5.3 and 5.4: a and mx may name a domain and may carry lengths.
      CheckRecord_("v=spf1 a", true);
      CheckRecord_("v=spf1 mx", true);
      CheckRecord_("v=spf1 a:example.com", true);
      CheckRecord_("v=spf1 mx:example.com", true);
      CheckRecord_("v=spf1 a:example.com/24", true);
      CheckRecord_("v=spf1 a:", false);
      CheckRecord_("v=spf1 mx:", false);

      // Section 5.5: ptr may name a domain and carries no length.
      CheckRecord_("v=spf1 ptr", true);
      CheckRecord_("v=spf1 ptr:example.com -all", true);
      CheckRecord_("v=spf1 ptr:", false);
      CheckRecord_("v=spf1 ptr/0 -all", false);

      // Section 5.6: ip4 and ip6 must carry an address.
      CheckRecord_("v=spf1 ip4:1.2.3.4", true);
      CheckRecord_("v=spf1 ip6:2001:db8::1", true);
      CheckRecord_("v=spf1 ip4", false);
      CheckRecord_("v=spf1 ip6", false);
      CheckRecord_("v=spf1 ip4:", false);
      CheckRecord_("v=spf1 ip4:1.2.3", false);
      CheckRecord_("v=spf1 ip6::CAFE::BABE", false);

      // The names are ABNF literals, so a record may spell them any way.
      CheckRecord_("v=spf1 A:Example.Com MX -ALL", true);
      CheckRecord_("v=spf1 IP4:1.2.3.4 Include:example.com ?All", true);

      // Anything that is neither a mechanism nor a modifier.
      CheckRecord_("v=spf1 moo", false);
      CheckRecord_("v=spf1 ip5:1.2.3.4", false);
      CheckRecord_("v=spf1 redirect:example.com", false);

      // Section 4.6: the whole record is checked, so a mistake after a
      // mechanism that would have matched is still a mistake.
      CheckRecord_("v=spf1 ip4:1.2.3.4 -all moo", false);
   }

   // Sections 5.3 and 5.6: the prefix lengths, and which mechanism may carry
   // which.
   void
   SPFRecordTester::TestPrefixLengths_()
   {
      // a and mx take the dual form: an ip4 length, an ip6 length, or both in
      // that order.
      CheckRecord_("v=spf1 a/0 -all", true);
      CheckRecord_("v=spf1 a/24 -all", true);
      CheckRecord_("v=spf1 a/32 -all", true);
      CheckRecord_("v=spf1 a//0 -all", true);
      CheckRecord_("v=spf1 a//64 -all", true);
      CheckRecord_("v=spf1 a//128 -all", true);
      CheckRecord_("v=spf1 a/24//64 -all", true);
      CheckRecord_("v=spf1 mx/24//64 -all", true);

      CheckRecord_("v=spf1 a/33 -all", false);
      CheckRecord_("v=spf1 a//129 -all", false);
      CheckRecord_("v=spf1 a/24/64 -all", false);
      CheckRecord_("v=spf1 a/ -all", false);
      CheckRecord_("v=spf1 a// -all", false);

      // A length is written without a leading zero.
      CheckRecord_("v=spf1 a/032 -all", false);
      CheckRecord_("v=spf1 ip4:1.2.3.4/032 -all", false);

      // ip4 and ip6 take the one length that goes with their family, written
      // with a single slash.
      CheckRecord_("v=spf1 ip4:1.1.1.1/0 -all", true);
      CheckRecord_("v=spf1 ip4:1.2.3.4/32 -all", true);
      CheckRecord_("v=spf1 ip6:::1.1.1.1/0", true);
      CheckRecord_("v=spf1 ip6:Cafe:Babe:8000::/33", true);
      CheckRecord_("v=spf1 ip6:2001:db8::/128", true);

      CheckRecord_("v=spf1 ip4:1.2.3.4/33 -all", false);
      CheckRecord_("v=spf1 ip4:1.2.3.4//32", false);
      CheckRecord_("v=spf1 ip6:::1.1.1.1/129", false);
      CheckRecord_("v=spf1 ip6:::1.1.1.1//33", false);

      // What the parser hands on: the lengths that were written, and the
      // full-length defaults where none were.
      {
         SPFRecord record;
         AnsiString error;

         if (!SPFRecord::Parse("v=spf1 a a/24 a//64 a/8//96", record, error))
            failures_.push_back("A record of four a mechanisms should parse.");
         else if (record.GetMechanisms().size() != 4)
            failures_.push_back("A record of four directives should hold four.");
         else
         {
            const std::vector<SPFMechanism> &mechanisms = record.GetMechanisms();

            if (mechanisms[0].GetIP4PrefixLength() != 32 || mechanisms[0].GetIP6PrefixLength() != 128)
               failures_.push_back("A mechanism with no length gets the full length of each family.");
            if (mechanisms[1].GetIP4PrefixLength() != 24 || mechanisms[1].GetIP6PrefixLength() != 128)
               failures_.push_back("\"/24\" sets the ip4 length and leaves the ip6 one alone.");
            if (mechanisms[2].GetIP4PrefixLength() != 32 || mechanisms[2].GetIP6PrefixLength() != 64)
               failures_.push_back("\"//64\" sets the ip6 length and leaves the ip4 one alone.");
            if (mechanisms[3].GetIP4PrefixLength() != 8 || mechanisms[3].GetIP6PrefixLength() != 96)
               failures_.push_back("\"/8//96\" sets both lengths.");
         }
      }

      // A slash that does not introduce a length is part of the domain. This is
      // the one that a length read from the wrong end of the term would break.
      {
         SPFRecord record;
         AnsiString error;

         if (!SPFRecord::Parse("v=spf1 a:foo:bar/baz.example.com -all", record, error))
            failures_.push_back("A domain holding a slash should parse.");
         else if (record.GetMechanisms().size() != 2 ||
                  record.GetMechanisms()[0].GetDomainSpec() != "foo:bar/baz.example.com")
         {
            failures_.push_back("A slash not followed by digits belongs to the domain.");
         }
      }
   }

   // Section 6: the modifiers.
   void
   SPFRecordTester::TestModifiers_()
   {
      CheckRecord_("v=spf1 redirect=example.com", true);
      CheckRecord_("v=spf1 -all exp=example.com", true);
      CheckRecord_("v=spf1 exp=exp1.example.com redirect=e2.example.com", true);

      // Section 6: neither may appear twice.
      CheckRecord_("v=spf1 redirect=a.example.com redirect=b.example.com", false);
      CheckRecord_("v=spf1 -all exp=a.example.com exp=b.example.com", false);

      // Both name a domain, so the domain-spec rules apply.
      CheckRecord_("v=spf1 redirect=", false);
      CheckRecord_("v=spf1 exp= -all", false);
      CheckRecord_("v=spf1 redirect=-all ?all", false);
      CheckRecord_("v=spf1 exp=-all", false);
      CheckRecord_("v=spf1 -all exp=%{r}.example.com", false);

      // Section 6.3: a modifier nobody knows is ignored, but it still has to be
      // spelled like one, and its argument is a macro-string rather than a
      // domain.
      CheckRecord_("v=spf1 default=pass", true);
      CheckRecord_("v=spf1 default=+", true);
      CheckRecord_("v=spf1 moo.cow-far_out=man:dog/cat ip4:1.2.3.4 -all", true);
      CheckRecord_("v=spf1 a1-b_c.d=whatever -all", true);

      CheckRecord_("v=spf1 1up=foo", false);
      CheckRecord_("v=spf1 =all", false);
      CheckRecord_("v=spf1 -all foo=%abc", false);
      CheckRecord_("v=spf1 moo.cow/far_out=man:dog/cat ip4:1.2.3.4 -all", false);
      CheckRecord_("v=spf1 moo.cow:far_out=man:dog/cat ip4:1.2.3.4 -all", false);

      // What the parser hands on. An unknown modifier is dropped, because
      // nothing downstream has any use for it.
      {
         SPFRecord record;
         AnsiString error;

         if (!SPFRecord::Parse("v=spf1 a default=pass redirect=r.example.com exp=e.example.com", record, error))
            failures_.push_back("A record carrying both modifiers should parse.");
         else
         {
            if (record.GetMechanisms().size() != 1)
               failures_.push_back("A modifier is not a directive.");
            if (!record.GetHasRedirect() || record.GetRedirectDomainSpec() != "r.example.com")
               failures_.push_back("The redirect domain should be kept as written.");
            if (!record.GetHasExplanation() || record.GetExplanationDomainSpec() != "e.example.com")
               failures_.push_back("The exp domain should be kept as written.");
         }
      }

      {
         SPFRecord record;
         AnsiString error;

         if (!SPFRecord::Parse("v=spf1 -all", record, error))
            failures_.push_back("\"v=spf1 -all\" should parse.");
         else if (record.GetHasRedirect() || record.GetHasExplanation())
            failures_.push_back("A record with no modifiers should report none.");
      }
   }

   void
   SPFRecordTester::TestWholeRecords_()
   {
      // Section 4.5: the version, then terms separated by one or more spaces,
      // and any number of spaces after them.
      CheckRecord_("v=spf1", true);
      CheckRecord_("v=spf1 ", true);
      CheckRecord_("v=spf1 a  -all", true);
      CheckRecord_("v=spf1 a -all ", true);
      CheckRecord_("v=spf1   a   mx   -all   ", true);

      CheckRecord_("v=spf10", false);
      CheckRecord_("v=spf1mx", false);
      CheckRecord_("", false);

      // Section 3.1: a record is 7-bit ASCII. A byte outside it, or a control
      // character, means the record cannot be evaluated - not that it does not
      // apply.
      CheckRecord_(AnsiString("v=spf1 a:example.net -all") + '\x80', false);
      CheckRecord_(AnsiString("v=spf1 ") + '\x80' + "a:example.net -all", false);

      {
         // A control character inside a record, which would otherwise look like
         // a term separator.
         AnsiString record = AnsiString("v=spf1 a:ctrl.example.com") + '\r' + "ptr -all";
         CheckRecord_(record, false);
      }

      {
         // A NUL, which a record read as a C string would never see.
         AnsiString record = AnsiString("v=spf1 a:foo.example.com") + '\0';
         CheckRecord_(record, false);
      }

      // Records from real life, of the shape the conformance suite is full of.
      CheckRecord_("v=spf1 a mx ip4:192.0.2.0/24 ip6:2001:db8::/32 include:_spf.example.net ~all", true);
      CheckRecord_("v=spf1 exists:%{ir}.%{v}._spf.%{d2} -all", true);
      CheckRecord_("v=spf1 ptr:example.com exp=why.%{d2} -all", true);
   }

   std::vector<AnsiString>
   SPFRecordTester::Run()
   {
      failures_.clear();

      TestVersion_();
      TestMacroStrings_();
      TestDomainSpecs_();
      TestAddressLiterals_();
      TestDomainNames_();
      TestMechanisms_();
      TestPrefixLengths_();
      TestModifiers_();
      TestWholeRecords_();

      return failures_;
   }
}
