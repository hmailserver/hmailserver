// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "stdafx.h"

#include "SPFMacroExpanderTester.h"

#include "SPFAddress.h"
#include "SPFMacroExpander.h"
#include "SPFTestLookup.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   namespace
   {
      SPFAddress AddressOf(const AnsiString &text)
      {
         SPFAddress address;

         SPFAddress::TryParse(text, address);

         return address;
      }
   }

   std::vector<AnsiString>
   SPFMacroExpanderTester::Run()
   {
      failures_.clear();

      TestAddressForms_();
      TestPrefixMatching_();
      TestSpecExamples_();
      TestTransformers_();
      TestLiterals_();
      TestUrlEscaping_();
      TestPostmasterDefaults_();
      TestExplanationText_();
      TestValidatedName_();
      TestTruncation_();

      return failures_;
   }

   // The forms of an address the macros of section 7.2 ask for.
   void
   SPFMacroExpanderTester::TestAddressForms_()
   {
      struct Expected
      {
         const char *text;

         // 0 where the literal is not one RFC 7208 section 12 spells.
         const char *dotted;
         const char *readable;
         const char *reverseName;
         bool isIP6;
      };

      const Expected expected[] =
      {
         { "192.168.218.40", "192.168.218.40", "192.168.218.40",
           "40.218.168.192.in-addr.arpa", false },

         { "0.0.0.0", "0.0.0.0", "0.0.0.0", "0.0.0.0.in-addr.arpa", false },

         { "255.255.255.255", "255.255.255.255", "255.255.255.255",
           "255.255.255.255.in-addr.arpa", false },

         // The suite's v-macro-ip6 asserts this reverse name as text, in an
         // explanation, and writes the hex digits in upper case.
         { "CAFE:BABE::1",
           "C.A.F.E.B.A.B.E.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.1",
           "cafe:babe::1",
           "1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.E.B.A.B.E.F.A.C.ip6.arpa",
           true },

         { "2001:db8:ff0:100::2",
           "2.0.0.1.0.D.B.8.0.F.F.0.0.1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.2",
           "2001:db8:ff0:100::2",
           "2.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.1.0.0.F.F.0.8.B.D.0.1.0.0.2.ip6.arpa",
           true },

         // RFC 5952 section 4.2.2: a run of one zero group stays written out.
         { "2001:db8:0:1:1:1:1:1",
           "2.0.0.1.0.D.B.8.0.0.0.0.0.0.0.1.0.0.0.1.0.0.0.1.0.0.0.1.0.0.0.1",
           "2001:db8:0:1:1:1:1:1",
           "1.0.0.0.1.0.0.0.1.0.0.0.1.0.0.0.1.0.0.0.0.0.0.0.8.B.D.0.1.0.0.2.ip6.arpa",
           true },

         { "::1",
           "0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.1",
           "::1",
           "1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.ip6.arpa",
           true },

         { "::",
           "0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0",
           "::",
           "0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.ip6.arpa",
           true },

         // The undef-macro case connects from this. Written with a dotted tail,
         // but not the mapped form, so it stays IPv6.
         { "CAFE:BABE::192.168.218.40",
           "C.A.F.E.B.A.B.E.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.C.0.A.8.D.A.2.8",
           "cafe:babe::c0a8:da28",
           "8.2.A.D.8.A.0.C.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.E.B.A.B.E.F.A.C.ip6.arpa",
           true },

         // RFC 4291 section 2.5.5.2. The client reached the server over IPv4
         // whatever the address was written as, and sections 5.6 and 5.7 pick a
         // mechanism by family, so it has to be read as IPv4.
         { "::ffff:192.0.2.1", "192.0.2.1", "192.0.2.1",
           "1.2.0.192.in-addr.arpa", false }
      };

      for (size_t i = 0; i < sizeof(expected) / sizeof(expected[0]); i++)
      {
         const Expected &wanted = expected[i];

         SPFAddress address;

         if (!SPFAddress::TryParse(wanted.text, address))
         {
            failures_.push_back(AnsiString("Rejected the address literal \"") + wanted.text + "\"");
            continue;
         }

         SPFAddress::Family family = wanted.isIP6 ? SPFAddress::Family::IP6
                                                  : SPFAddress::Family::IP4;

         if (address.GetFamily() != family)
         {
            failures_.push_back(AnsiString("\"") + wanted.text + "\" came out as the wrong family");
            continue;
         }

         AnsiString arpaLabel = wanted.isIP6 ? "ip6" : "in-addr";

         struct Check
         {
            const char *what;
            AnsiString actual;
            const char *expected;
         };

         const Check checks[] =
         {
            { "%{i}", address.GetDottedForm(), wanted.dotted },
            { "%{c}", address.GetReadableForm(), wanted.readable },
            { "%{v}", address.GetArpaLabel(), arpaLabel },
            { "the reverse name", address.GetReverseName(), wanted.reverseName }
         };

         for (size_t c = 0; c < sizeof(checks) / sizeof(checks[0]); c++)
         {
            if (checks[c].actual == AnsiString(checks[c].expected))
               continue;

            failures_.push_back(AnsiString(checks[c].what) + " of " + wanted.text + " is \"" +
                                checks[c].actual + "\", expected \"" + checks[c].expected + "\"");
         }
      }

      // Literals RFC 7208 section 12 does not spell. The grammar is SPFSyntax's
      // and is tested there; what matters here is that an address which is not
      // one leaves the family unset rather than producing forms of its own.
      const char *rejected[] =
      {
         "", "1.2.3", "1.2.3.4.5", "256.1.1.1", "01.2.3.4", "1.2.3.4 ",
         "192.168.218.40:25", "::ggg", "1::2::3", "cafe:babe", "12345::1",
         "1.2.3.4/24", "[192.168.218.40]"
      };

      for (size_t i = 0; i < sizeof(rejected) / sizeof(rejected[0]); i++)
      {
         SPFAddress address;

         if (!SPFAddress::TryParse(rejected[i], address))
         {
            if (address.GetFamily() != SPFAddress::Family::None)
               failures_.push_back(AnsiString("\"") + rejected[i] + "\" was rejected but left a family behind");

            continue;
         }

         failures_.push_back(AnsiString("Accepted \"") + rejected[i] + "\" as an address literal");
      }
   }

   // Sections 5.3, 5.4, 5.6 and 5.7 match a client against a network, and the
   // evaluator has nothing else to decide an ip4 or ip6 mechanism with.
   void
   SPFMacroExpanderTester::TestPrefixMatching_()
   {
      struct Case
      {
         const char *client;
         const char *network;
         int prefixLength;
         bool matches;
      };

      const Case cases[] =
      {
         { "1.2.3.4", "1.2.3.4", 32, true },
         { "1.2.3.4", "1.2.3.5", 32, false },
         { "1.2.3.4", "1.2.3.0", 24, true },
         { "1.2.3.4", "1.2.4.0", 24, false },
         { "1.2.3.4", "9.9.9.9", 0, true },

         // A boundary inside a byte, which is where an implementation that
         // compared whole bytes would go wrong.
         { "1.2.3.130", "1.2.3.128", 25, true },
         { "1.2.3.127", "1.2.3.128", 25, false },
         { "1.2.3.127", "1.2.3.128", 24, true },

         // A length past the family's own is not a length at all.
         { "1.2.3.4", "1.2.3.4", 33, false },
         { "1.2.3.4", "1.2.3.4", -1, false },

         { "CAFE:BABE::1", "CAFE:BABE::1", 128, true },
         { "CAFE:BABE::1", "CAFE:BABE::2", 128, false },
         { "CAFE:BABE::1", "CAFE:BABE::", 64, true },
         { "CAFE:BABF::1", "CAFE:BABE::", 64, false },
         { "CAFE:BABE::1", "CAFE:BABE::", 128, false },
         { "2001:db8:ff0:100::2", "2001:db8:ff0:100::", 64, true },
         { "CAFE:BABE::1", "CAFE:BABE::1", 129, false },

         // Section 5.6: the families never match each other, however the
         // addresses are written.
         { "192.0.2.1", "::ffff:192.0.2.1", 32, true },
         { "192.0.2.1", "::", 0, false },
         { "::1", "1.2.3.4", 0, false }
      };

      for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++)
      {
         SPFAddress client = AddressOf(cases[i].client);
         SPFAddress network = AddressOf(cases[i].network);

         if (client.MatchesPrefix(network, cases[i].prefixLength) == cases[i].matches)
            continue;

         failures_.push_back(AnsiString(cases[i].client) +
                             (cases[i].matches ? " did not match " : " matched ") +
                             cases[i].network + "/" +
                             (cases[i].prefixLength < 0 ? "-1" : "") + " as it should");
      }

      // An address which did not parse matches nothing, which is what keeps a
      // client whose address could not be read from passing "ip4:0.0.0.0/0".
      SPFAddress unparsed;

      if (unparsed.MatchesPrefix(AddressOf("0.0.0.0"), 0))
         failures_.push_back("An address which was never parsed matched a network");

      if (unparsed.Equals(unparsed))
         failures_.push_back("An address which was never parsed equalled itself");

      if (!AddressOf("1.2.3.4").Equals(AddressOf("1.2.3.4")))
         failures_.push_back("An address did not equal itself");
   }

   // The examples of RFC 7208 section 7.4, which is the only place the RFC says
   // outright what an expansion comes to.
   void
   SPFMacroExpanderTester::TestSpecExamples_()
   {
      auto lookup = std::make_shared<SPFTestLookup>();

      SPFMacroExpander expander(lookup, AddressOf("192.0.2.3"),
                                "strong-bad@email.example.com", "mx.example.org");

      const AnsiString domain = "email.example.com";

      struct Case
      {
         const char *macroString;
         const char *expected;
      };

      const Case cases[] =
      {
         { "%{s}", "strong-bad@email.example.com" },
         { "%{o}", "email.example.com" },
         { "%{d}", "email.example.com" },
         { "%{d4}", "email.example.com" },
         { "%{d3}", "email.example.com" },
         { "%{d2}", "example.com" },
         { "%{d1}", "com" },
         { "%{dr}", "com.example.email" },
         { "%{d2r}", "example.email" },
         { "%{l}", "strong-bad" },
         { "%{l-}", "strong.bad" },
         { "%{lr}", "strong-bad" },
         { "%{lr-}", "bad.strong" },
         { "%{l1r-}", "strong" },
         { "%{h}", "mx.example.org" },
         { "%{i}", "192.0.2.3" },
         { "%{ir}", "3.2.0.192" },
         { "%{v}", "in-addr" },

         // The macro-string examples of the same section.
         { "%{ir}.%{v}._spf.%{d2}", "3.2.0.192.in-addr._spf.example.com" },
         { "%{lr-}.lp._spf.%{d2}", "bad.strong.lp._spf.example.com" },
         { "%{lr-}.lp.%{ir}.%{v}._spf.%{d2}", "bad.strong.lp.3.2.0.192.in-addr._spf.example.com" },
         { "%{ir}.%{v}.%{l1r-}.lp._spf.%{d2}", "3.2.0.192.in-addr.strong.lp._spf.example.com" },
         { "%{d2}.trusted-domains.example.net", "example.com.trusted-domains.example.net" }
      };

      for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++)
      {
         AnsiString expanded;

         if (!expander.ExpandDomainSpec(cases[i].macroString, domain, expanded))
         {
            failures_.push_back(AnsiString("Would not expand \"") + cases[i].macroString + "\"");
            continue;
         }

         if (expanded == AnsiString(cases[i].expected))
            continue;

         failures_.push_back(AnsiString("\"") + cases[i].macroString + "\" expanded to \"" +
                             expanded + "\", expected \"" + cases[i].expected + "\"");
      }
   }

   // Section 7.3's transformers on their own, which no conformance case pins the
   // text of.
   void
   SPFMacroExpanderTester::TestTransformers_()
   {
      auto lookup = std::make_shared<SPFTestLookup>();

      // The senders of the suite's macro-reverse-split-on-dash and
      // macro-multiple-delimiters cases, whose records reach the zones
      // "gladstone.philip.user.example.com" and "bar.foo.user.example.com".
      SPFMacroExpander first(lookup, AddressOf("1.2.3.4"),
                             "philip-gladstone-test@e11.example.com", "mail.example.com");

      SPFMacroExpander second(lookup, AddressOf("1.2.3.4"),
                              "foo-bar+zip+quux@e12.example.com", "mail.example.com");

      struct Case
      {
         SPFMacroExpander *expander;
         const char *macroString;
         const char *domain;
         const char *expected;
      };

      const Case cases[] =
      {
         { &first, "%{i}.%{l2r-}.user.%{d2}", "e11.example.com",
           "1.2.3.4.gladstone.philip.user.example.com" },

         { &second, "%{l2r+-}.user.%{d2}", "e12.example.com",
           "bar.foo.user.example.com" },

         // Every part where the count asks for more than there are, and where it
         // asks for none. RFC 7208 does not allow a count of zero and does not
         // say what one means; every part is the reading SPFSyntax settled on.
         { &first, "%{d9}", "e11.example.com", "e11.example.com" },
         { &first, "%{d0}", "e11.example.com", "e11.example.com" },
         { &first, "%{d999}", "e11.example.com", "e11.example.com" },

         // A delimiter the value does not hold leaves it in one part.
         { &first, "%{d+}", "e11.example.com", "e11.example.com" },
         { &first, "%{d2+}", "e11.example.com", "e11.example.com" },

         // The parts are rejoined with dots whatever they were split at, which is
         // the whole purpose of naming a delimiter.
         { &first, "%{l-}", "e11.example.com", "philip.gladstone.test" },
         { &first, "%{l1-}", "e11.example.com", "test" },
         { &first, "%{lr-}", "e11.example.com", "test.gladstone.philip" },

         // Reversal with the default delimiter, and reversal of a single part.
         { &first, "%{or}", "e11.example.com", "com.example.e11" },
         { &first, "%{hr}", "e11.example.com", "com.example.mail" },
         { &first, "%{lr}", "e11.example.com", "philip-gladstone-test" },

         // ABNF makes the "r" case insensitive, like every other literal.
         { &first, "%{lR-}", "e11.example.com", "test.gladstone.philip" },
         { &first, "%{dR}", "e11.example.com", "com.example.e11" },

         // A delimiter written more than once, and every delimiter at once.
         { &second, "%{l++}", "e12.example.com", "foo-bar.zip.quux" },
         { &second, "%{l.-+,/_=}", "e12.example.com", "foo.bar.zip.quux" }
      };

      for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++)
      {
         AnsiString expanded;

         if (!cases[i].expander->ExpandDomainSpec(cases[i].macroString, cases[i].domain, expanded))
         {
            failures_.push_back(AnsiString("Would not expand \"") + cases[i].macroString + "\"");
            continue;
         }

         if (expanded == AnsiString(cases[i].expected))
            continue;

         failures_.push_back(AnsiString("\"") + cases[i].macroString + "\" expanded to \"" +
                             expanded + "\", expected \"" + cases[i].expected + "\"");
      }
   }

   // Section 7.1's three literals. The suite's macro-mania-in-domain reaches a
   // zone whose name holds a percent sign, two spaces and a "%20", which is what
   // these produce.
   void
   SPFMacroExpanderTester::TestLiterals_()
   {
      auto lookup = std::make_shared<SPFTestLookup>();

      SPFMacroExpander expander(lookup, AddressOf("1.2.3.4"),
                                "test@e1a.example.com", "mail.example.com");

      struct Case
      {
         const char *macroString;
         const char *expected;
      };

      const Case cases[] =
      {
         { "%%", "%" },
         { "%_", " " },
         { "%-", "%20" },

         // The target of the suite's e1a.example.com, and the zone it reaches.
         { "macro%%percent%_%_space%-url-space.example.com",
           "macro%percent  space%20url-space.example.com" },

         { "%%%%", "%%" },
         { "%_%_", "  " },
         { "%-%-", "%20%20" }
      };

      for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++)
      {
         AnsiString expanded;

         if (!expander.ExpandDomainSpec(cases[i].macroString, "e1a.example.com", expanded))
         {
            failures_.push_back(AnsiString("Would not expand \"") + cases[i].macroString + "\"");
            continue;
         }

         if (expanded == AnsiString(cases[i].expected))
            continue;

         failures_.push_back(AnsiString("\"") + cases[i].macroString + "\" expanded to \"" +
                             expanded + "\", expected \"" + cases[i].expected + "\"");
      }

      // A stray percent sign is not a macro. The parser rejects a record which holds one,
      // and the expander has to agree, because the text an exp modifier fetches never went
      // through the parser.
      const char *notMacros[] = { "%", "%(ir)", "foo%(ir).sbl.example.com", "foo%.example.com",
                                  "%{", "%{d", "%{d.", "%z", "100%" };

      for (size_t i = 0; i < sizeof(notMacros) / sizeof(notMacros[0]); i++)
      {
         AnsiString expanded;

         if (!expander.ExpandDomainSpec(notMacros[i], "e1a.example.com", expanded))
            continue;

         failures_.push_back(AnsiString("Expanded \"") + notMacros[i] + "\", which is not a macro-string");
      }
   }

   // Section 7.1: an upper case macro letter expands as the lower case one does
   // and is then URL escaped.
   void
   SPFMacroExpanderTester::TestUrlEscaping_()
   {
      auto lookup = std::make_shared<SPFTestLookup>();

      // The sender of the suite's upper-macro case, whose explanation is
      // asserted as text.
      SPFMacroExpander expander(lookup, AddressOf("192.168.218.42"),
                                "~jack&jill=up-a_b3.c@e8.example.com", "msgbas2x.cos.example.com");

      const AnsiString domain = "e8.example.com";

      struct Case
      {
         const char *macroString;
         const char *expected;
      };

      const Case cases[] =
      {
         { "http://example.com/why.html?l=%{L}",
           "http://example.com/why.html?l=~jack%26jill%3Dup-a_b3.c" },

         // RFC 3986 section 2.3's unreserved set is what survives: the letters,
         // the digits, and these four.
         { "%{L}", "~jack%26jill%3Dup-a_b3.c" },
         { "%{l}", "~jack&jill=up-a_b3.c" },

         // The escape happens after the transformers, so an upper case letter may
         // carry them too.
         { "%{L1}", "c" },
         { "%{D2}", "example.com" },
         { "%{S}", "~jack%26jill%3Dup-a_b3.c%40e8.example.com" },

         // The at sign of a sender is not unreserved, and neither is the colon of
         // an IPv6 address.
         { "%{s}", "~jack&jill=up-a_b3.c@e8.example.com" },
         { "%{I}", "192.168.218.42" },
         { "%{V}", "in-addr" }
      };

      for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++)
      {
         AnsiString expanded;

         if (!expander.ExpandExplanation(cases[i].macroString, domain, expanded))
         {
            failures_.push_back(AnsiString("Would not expand \"") + cases[i].macroString + "\"");
            continue;
         }

         if (expanded == AnsiString(cases[i].expected))
            continue;

         failures_.push_back(AnsiString("\"") + cases[i].macroString + "\" expanded to \"" +
                             expanded + "\", expected \"" + cases[i].expected + "\"");
      }

      // An IPv6 client, where the c macro holds colons and the escape shows.
      SPFMacroExpander overIP6(lookup, AddressOf("CAFE:BABE::1"),
                               "test@e4.example.com", "msgbas2x.cos.example.com");

      AnsiString expanded;

      if (overIP6.ExpandExplanation("%{C}", "e4.example.com", expanded))
      {
         if (expanded != AnsiString("cafe%3Ababe%3A%3A1"))
            failures_.push_back(AnsiString("%{C} of an IPv6 client expanded to \"") + expanded + "\"");
      }
      else
      {
         failures_.push_back("Would not expand %{C}");
      }
   }

   // Section 4.3 fills "postmaster" in for a sender with no local part, and
   // section 2.4 has a check made without a sender made for the HELO argument
   // instead. The suite's nolocalpart asserts the first as an explanation.
   void
   SPFMacroExpanderTester::TestPostmasterDefaults_()
   {
      auto lookup = std::make_shared<SPFTestLookup>();

      struct Case
      {
         const char *sender;
         const char *helo;
         const char *macroString;
         const char *expected;
      };

      const Case cases[] =
      {
         { "@example.net", "mail.example.net", "%{l}", "postmaster" },
         { "@example.net", "mail.example.net", "%{o}", "example.net" },
         { "@example.net", "mail.example.net", "%{s}", "postmaster@example.net" },

         // No sender at all: the check is being made for the HELO argument.
         { "", "mail.example.net", "%{l}", "postmaster" },
         { "", "mail.example.net", "%{o}", "mail.example.net" },
         { "", "mail.example.net", "%{s}", "postmaster@mail.example.net" },
         { "", "mail.example.net", "%{h}", "mail.example.net" },

         // A local part which holds an at sign of its own. Section 4.3 takes the
         // domain from after the last one.
         { "a@b@example.net", "mail.example.net", "%{l}", "a@b" },
         { "a@b@example.net", "mail.example.net", "%{o}", "example.net" },

         { "test@example.net", "mail.example.net", "%{l}", "test" },
         { "test@example.net", "mail.example.net", "%{s}", "test@example.net" }
      };

      for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++)
      {
         SPFMacroExpander expander(lookup, AddressOf("1.2.3.4"), cases[i].sender, cases[i].helo);

         AnsiString expanded;

         if (!expander.ExpandExplanation(cases[i].macroString, "example.net", expanded))
         {
            failures_.push_back(AnsiString("Would not expand \"") + cases[i].macroString + "\"");
            continue;
         }

         if (expanded == AnsiString(cases[i].expected))
            continue;

         failures_.push_back(AnsiString("\"") + cases[i].macroString + "\" of the sender \"" +
                             cases[i].sender + "\" expanded to \"" + expanded +
                             "\", expected \"" + cases[i].expected + "\"");
      }
   }

   // Section 7.2 gives the c, r and t macros to the text an exp modifier points
   // at and to nothing else, and section 12's explain-string allows the spaces
   // that make an explanation a sentence.
   void
   SPFMacroExpanderTester::TestExplanationText_()
   {
      auto lookup = std::make_shared<SPFTestLookup>();

      SPFMacroExpander expander(lookup, AddressOf("192.168.218.40"),
                                "test@e4.example.com", "msgbas2x.cos.example.com");

      expander.SetReceivingHost("mx.receiver.example");
      expander.SetTimestamp(1234567890);

      const AnsiString domain = "e4.example.com";

      struct Case
      {
         const char *text;
         const char *expected;
      };

      const Case cases[] =
      {
         // The suite's e4msg.example.com, whose expansion v-macro-ip4 asserts.
         { "%{c} is queried as %{ir}.%{v}.arpa",
           "192.168.218.40 is queried as 40.218.168.192.in-addr.arpa" },

         // Its 40.218.168.192.example.com, which exp-txt-macro-char asserts.
         { "Connections from %{c} not authorized.",
           "Connections from 192.168.218.40 not authorized." },

         { "%{r}", "mx.receiver.example" },
         { "%{t}", "1234567890" },

         // Spaces, and nothing else, on their own.
         { "This is a test.", "This is a test." },
         { "Congratulations!  That was tricky.", "Congratulations!  That was tricky." },
         { " ", " " },
         { "", "" }
      };

      for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++)
      {
         AnsiString expanded;

         if (!expander.ExpandExplanation(cases[i].text, domain, expanded))
         {
            failures_.push_back(AnsiString("Would not expand the explanation \"") + cases[i].text + "\"");
            continue;
         }

         if (expanded == AnsiString(cases[i].expected))
            continue;

         failures_.push_back(AnsiString("The explanation \"") + cases[i].text + "\" expanded to \"" +
                             expanded + "\", expected \"" + cases[i].expected + "\"");
      }

      // The same macros in a record's own term. The suite's exp-txt-macro-char
      // neighbour, "exp=%{r}.example.com", is a permerror for this reason.
      const char *explanationOnly[] = { "%{c}", "%{r}", "%{t}", "%{C}", "%{R}", "%{T}",
                                        "%{r}.example.com", "a space.example.com" };

      for (size_t i = 0; i < sizeof(explanationOnly) / sizeof(explanationOnly[0]); i++)
      {
         AnsiString expanded;

         if (!expander.ExpandDomainSpec(explanationOnly[i], domain, expanded))
            continue;

         failures_.push_back(AnsiString("Expanded \"") + explanationOnly[i] +
                             "\" as a record's own term, where it does not belong");
      }

      // A macro letter which is not one, wherever it is written. The suite's
      // undef-macro and its "The %{x}-files." explanation.
      const char *notMacros[] = { "%{x}", "%{a}", "%{z}", "The %{x}-files.", "%{}" };

      for (size_t i = 0; i < sizeof(notMacros) / sizeof(notMacros[0]); i++)
      {
         AnsiString expanded;

         if (expander.ExpandExplanation(notMacros[i], domain, expanded))
            failures_.push_back(AnsiString("Expanded \"") + notMacros[i] + "\", which names no macro");

         if (expander.ExpandDomainSpec(notMacros[i], domain, expanded))
            failures_.push_back(AnsiString("Expanded \"") + notMacros[i] + "\" as a term");
      }

      // Section 7.3 leaves the r macro to the implementation where the receiving
      // host is not known.
      SPFMacroExpander unnamed(lookup, AddressOf("192.168.218.40"),
                               "test@e4.example.com", "msgbas2x.cos.example.com");

      AnsiString expanded;

      if (unnamed.ExpandExplanation("%{r}", domain, expanded) && expanded != AnsiString("unknown"))
         failures_.push_back(AnsiString("%{r} with no receiving host expanded to \"") + expanded + "\"");
   }

   // The p macro of section 7.3: the one expansion that costs DNS queries.
   void
   SPFMacroExpanderTester::TestValidatedName_()
   {
      const AnsiString reverseOf40 = "40.218.168.192.in-addr.arpa";
      const AnsiString reverseOf41 = "41.218.168.192.in-addr.arpa";
      const AnsiString reverseOf42 = "42.218.168.192.in-addr.arpa";

      // The suite's own zone for the four p-macro cases.
      auto suite = std::make_shared<SPFTestLookup>();

      suite->AddPTR(reverseOf40, "mx.example.com");
      suite->AddPTR(reverseOf41, "mx.example.com");
      suite->AddPTR(reverseOf42, "mx.example.com");
      suite->AddPTR(reverseOf42, "mx.e7.example.com");

      // The suite's reverse mappings for the two IPv6 hosts, whose zone names it
      // writes with the hex digits in upper case.
      suite->AddPTR("1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.E.B.A.B.E.F.A.C.ip6.arpa",
                    "mx.example.com");
      suite->AddPTR("3.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.E.B.A.B.E.F.A.C.ip6.arpa",
                    "mx.example.com");

      suite->AddA("mx.example.com", "192.168.218.41");
      suite->AddA("mx.example.com", "192.168.218.42");
      suite->AddAAAA("mx.example.com", "CAFE:BABE::2");
      suite->AddAAAA("mx.example.com", "CAFE:BABE::3");

      suite->AddA("mx.e7.example.com", "192.168.218.42");

      struct Case
      {
         const char *client;
         const char *domain;
         const char *expected;
         const char *what;
      };

      const Case cases[] =
      {
         // p-macro-ip4-novalid: the name the reverse mapping gives does not
         // resolve back to this address.
         { "192.168.218.40", "e6.example.com", "unknown", "a name which does not validate" },

         // p-macro-ip4-valid: it does, and it is neither <domain> nor below it,
         // so any validated name will do.
         { "192.168.218.41", "e6.example.com", "mx.example.com", "a name which validates" },

         // p-macro-ip6-novalid and p-macro-ip6-valid, which validate against the
         // name's AAAA records because the client came over IPv6.
         { "CAFE:BABE::1", "e6.example.com", "unknown", "an IPv6 name which does not validate" },
         { "CAFE:BABE::3", "e6.example.com", "mx.example.com", "an IPv6 name which validates" },

         // p-macro-multiple: two names validate, and section 7.3 prefers the one
         // below <domain>.
         { "192.168.218.42", "e7.example.com", "mx.e7.example.com", "a subdomain of the domain" },

         // The same two names, with <domain> itself among them.
         { "192.168.218.42", "mx.example.com", "mx.example.com", "the domain itself" },

         // Neither, so the first validated name.
         { "192.168.218.42", "elsewhere.example", "mx.example.com", "no preferred name" },

         // A comparison DNS makes without regard to case.
         { "192.168.218.42", "E7.EXAMPLE.COM", "mx.e7.example.com", "a subdomain in another case" },

         // A reverse mapping with nothing in it at all.
         { "1.2.3.4", "e6.example.com", "unknown", "no reverse mapping" }
      };

      for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++)
      {
         SPFMacroExpander expander(suite, AddressOf(cases[i].client),
                                   "test@e6.example.com", "msgbas2x.cos.example.com");

         AnsiString expanded;

         if (!expander.ExpandExplanation("connect from %{p}", cases[i].domain, expanded))
         {
            failures_.push_back(AnsiString("Would not expand %{p} for ") + cases[i].what);
            continue;
         }

         AnsiString wanted = AnsiString("connect from ") + cases[i].expected;

         if (expanded == wanted)
            continue;

         failures_.push_back(AnsiString("%{p} with ") + cases[i].what + " expanded to \"" +
                             expanded + "\", expected \"" + wanted + "\"");
      }

      // Section 7.3: a DNS error while looking for a validated name is "unknown"
      // as well. This is what keeps the p macro from producing the temperror a
      // mechanism would.
      auto failing = std::make_shared<SPFTestLookup>();

      failing->Fail(reverseOf41);
      failing->AddPTR(reverseOf41, "mx.example.com");
      failing->AddA("mx.example.com", "192.168.218.41");

      {
         SPFMacroExpander expander(failing, AddressOf("192.168.218.41"),
                                   "test@e6.example.com", "mail.example.com");

         AnsiString expanded;

         if (!expander.ExpandExplanation("%{p}", "e6.example.com", expanded))
            failures_.push_back("A failed reverse lookup made %{p} refuse to expand");
         else if (expanded != AnsiString("unknown"))
            failures_.push_back(AnsiString("%{p} after a failed reverse lookup expanded to \"") + expanded + "\"");
      }

      // A name whose own lookup fails does not validate, and does not stop the
      // names after it from being tried.
      auto partlyFailing = std::make_shared<SPFTestLookup>();

      partlyFailing->AddPTR(reverseOf41, "broken.example.com");
      partlyFailing->AddPTR(reverseOf41, "mx.example.com");
      partlyFailing->Fail("broken.example.com");
      partlyFailing->AddA("mx.example.com", "192.168.218.41");

      {
         SPFMacroExpander expander(partlyFailing, AddressOf("192.168.218.41"),
                                   "test@e6.example.com", "mail.example.com");

         AnsiString expanded;

         if (expander.ExpandExplanation("%{p}", "e6.example.com", expanded) &&
             expanded != AnsiString("mx.example.com"))
         {
            failures_.push_back(AnsiString("%{p} past a name whose lookup failed expanded to \"") +
                                expanded + "\"");
         }
      }

      // Section 7.3 caps how many of the names a reverse mapping gives are
      // looked up, because they are whatever the client's own zone says. Only
      // the eleventh validates here, so it is never reached.
      auto many = std::make_shared<SPFTestLookup>();

      // Written out rather than built, so that the order is plain: ten names
      // which do not validate, then one which does.
      const char *names[] = { "h1.example.com", "h2.example.com", "h3.example.com",
                              "h4.example.com", "h5.example.com", "h6.example.com",
                              "h7.example.com", "h8.example.com", "h9.example.com",
                              "h10.example.com", "h11.example.com" };

      for (size_t i = 0; i < sizeof(names) / sizeof(names[0]); i++)
      {
         many->AddPTR(reverseOf40, names[i]);

         // Every name but the last resolves to some other address.
         many->AddA(names[i], (i + 1 == sizeof(names) / sizeof(names[0]))
                                 ? "192.168.218.40" : "10.0.0.1");
      }

      {
         SPFMacroExpander expander(many, AddressOf("192.168.218.40"),
                                   "test@e6.example.com", "mail.example.com");

         AnsiString expanded;

         if (expander.ExpandExplanation("%{p}", "e6.example.com", expanded) &&
             expanded != AnsiString("unknown"))
         {
            failures_.push_back(AnsiString("%{p} looked past the first ") +
                                "ten names of a reverse mapping and found \"" + expanded + "\"");
         }
      }

      // The lookups behind the p macro are made once per check, however many
      // times the macro is written. Without that, a record which mentions it
      // twice would cost twice the queries.
      auto counted = std::make_shared<SPFTestLookup>();

      counted->AddPTR(reverseOf41, "mx.example.com");
      counted->AddA("mx.example.com", "192.168.218.41");

      {
         SPFMacroExpander expander(counted, AddressOf("192.168.218.41"),
                                   "test@e6.example.com", "mail.example.com");

         AnsiString expanded;

         expander.ExpandDomainSpec("%{p}.should.example.com", "e6.example.com", expanded);
         expander.ExpandDomainSpec("%{p}.ok.example.com", "e6.example.com", expanded);
         expander.ExpandExplanation("connect from %{p}", "e6.example.com", expanded);

         // One reverse lookup, and one lookup of the single name it gave.
         if (counted->GetQueryCount() != 2)
         {
            failures_.push_back(AnsiString("Three mentions of %{p} cost more than the two lookups ") +
                                "one of them does");
         }
      }
   }

   // Section 7.1: a name an expansion produced which is too long to ask about
   // loses whole labels from the left until it fits.
   void
   SPFMacroExpanderTester::TestTruncation_()
   {
      auto lookup = std::make_shared<SPFTestLookup>();

      // The suite's domain-name-truncation case, whose explanation is asserted as
      // text and can only be found if the name is shortened.
      SPFMacroExpander expander(lookup, AddressOf("192.168.218.40"),
                                "test@somewhat.long.exp.example.com", "msgbas2x.cos.example.com");

      const AnsiString domain = "somewhat.long.exp.example.com";

      AnsiString expanded;

      if (!expander.ExpandDomainSpec("foobar.%{o}.%{o}.%{o}.%{o}.%{o}.%{o}.%{o}.%{o}.example.com",
                                     domain, expanded))
      {
         failures_.push_back("Would not expand the domain-name-truncation target");
      }
      else
      {
         AnsiString wanted = "somewhat.long.exp.example.com.somewhat.long.exp.example.com."
                             "somewhat.long.exp.example.com.somewhat.long.exp.example.com."
                             "somewhat.long.exp.example.com.somewhat.long.exp.example.com."
                             "somewhat.long.exp.example.com.somewhat.long.exp.example.com."
                             "example.com";

         if (expanded != wanted)
         {
            failures_.push_back(AnsiString("The domain-name-truncation target expanded to \"") +
                                expanded + "\"");
         }

         if (expanded.GetLength() > 253)
            failures_.push_back("A truncated name is still longer than 253 characters");
      }

      // A name exactly at the limit is left alone, and one character more loses
      // its first label.
      AnsiString label;

      while (label.GetLength() < 63)
         label += 'a';

      // Three labels of 63 characters and one of 61, with the three dots between
      // them, is exactly the 253 a name may be.
      AnsiString atLimit = label + "." + label + "." + label + "." + label.Left(61);

      if (atLimit.GetLength() != 253)
      {
         failures_.push_back("The name built for the limit is not 253 characters long");
      }
      else
      {
         if (expander.ExpandDomainSpec(atLimit, domain, expanded) && expanded != atLimit)
            failures_.push_back("A name of exactly 253 characters was shortened");

         AnsiString overLimit = AnsiString("x.") + atLimit;

         if (expander.ExpandDomainSpec(overLimit, domain, expanded) && expanded != atLimit)
         {
            failures_.push_back(AnsiString("A name of 255 characters came out as \"") +
                                expanded + "\"");
         }

         // Two labels over, so that an implementation which dropped one label and
         // stopped would still be too long.
         AnsiString farOverLimit = AnsiString("x.y.") + atLimit;

         if (expander.ExpandDomainSpec(farOverLimit, domain, expanded) && expanded != atLimit)
         {
            failures_.push_back(AnsiString("A name of 257 characters came out as \"") +
                                expanded + "\"");
         }
      }

      // A trailing dot says only that the name is absolute. The suite's
      // "redirect=%{d}.d.spf.example.com." reaches a zone written without one.
      if (expander.ExpandDomainSpec("%{d}.d.spf.example.com.", "example.com", expanded) &&
          expanded != AnsiString("example.com.d.spf.example.com"))
      {
         failures_.push_back(AnsiString("A target written with a trailing dot expanded to \"") +
                             expanded + "\"");
      }

      // An explanation is text rather than a name, so neither the limit nor the
      // trailing dot applies to it.
      if (expander.ExpandExplanation("This is a test.", domain, expanded) &&
          expanded != AnsiString("This is a test."))
      {
         failures_.push_back("An explanation lost its full stop");
      }
   }
}
