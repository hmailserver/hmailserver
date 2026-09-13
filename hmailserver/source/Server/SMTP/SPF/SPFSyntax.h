// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

namespace HM
{
   // The grammar of an SPF record, from the ABNF of RFC 7208 section 12 and the
   // macro rules of section 7.1.
   //
   // Its own file because the parser and the macro expander both need it, and
   // because the rules are fussier than they look: a domain-spec has to end in
   // something a DNS name can end in, an address literal is stricter here than
   // the one a resolver would accept, and a prefix length may not be written
   // with a leading zero.
   class SPFSyntax
   {
   public:

      // Which macros a string is allowed to use. RFC 7208 section 7.2 gives c, r
      // and t only to the explanation that an exp modifier points at; a record's
      // own terms may not use them. That is what makes "exp=%{r}.example.com" a
      // permerror while the very same macro is fine in the text exp fetches.
      enum class MacroSet
      {
         RecordTerm,
         ExplanationText
      };

      // One macro expansion, taken apart:
      //
      //   macro-expand = ( "%{" macro-letter transformers *delimiter "}" )
      //                  / "%%" / "%_" / "%-"
      //   transformers = *DIGIT [ "r" ]
      //
      // The parser only needs to know whether a macro-string is well formed and
      // the expander needs the pieces, but the two have to agree about where a
      // macro ends, so there is one reader and this is what it returns.
      struct Macro
      {
         // "%%", "%_" and "%-" carry no letter. The character which follows the
         // percent sign is kept as written; what it stands for is section 7.1's
         // business rather than the grammar's.
         bool is_literal;
         char literal;

         // The macro letter, as written. ABNF makes the letter itself case
         // insensitive; section 7.1 gives the upper case spelling the added
         // meaning that the expansion is URL escaped, so the case is kept.
         char letter;

         // How many of the right-hand parts to keep. Zero means all of them -
         // either no digits were written, or a zero was. RFC 7208 does not
         // allow a value of zero and does not say what to do with one; keeping
         // every part is the only reading that leaves the record usable, and
         // evaluating a record beats failing it.
         int digits;

         // The "r" of the transformers, in either case.
         bool reverse;

         // The characters to split the value on, as written. Empty where none
         // were, and section 7.1's default is then ".".
         AnsiString delimiters;

         // Just past the closing "}", or past the two characters of a "%%",
         // "%_" or "%-".
         int end;
      };

      // Reads the macro expansion which begins at position. Returns false if
      // what is there is not one, which is what makes a stray "%" a syntax
      // error and what keeps "exp=%{r}.example.com" out of a record's own terms.
      static bool TryReadMacro(const AnsiString &text, int position, MacroSet macros, Macro &macro);

      // macro-string. The argument of an unknown modifier is one of these, so
      // there is no requirement that it look like a domain name.
      static bool IsValidMacroString(const AnsiString &text, MacroSet macros);

      // domain-spec: a macro-string which ends in either a top label or a macro
      // expansion. Every mechanism and modifier that names a domain takes one of
      // these, which is why "a:foo-bar" is a permerror while "a:foo.bar" is not:
      // a name with no dot in it cannot end in a top label.
      //
      // Only ever a record term, so the exp-only macros are not allowed. What
      // the macros expand to is not checked here and is not checked anywhere:
      // section 7.1 does not re-parse an expansion, and a name that comes out of
      // one unusable is treated as a name that does not exist.
      static bool IsValidDomainSpec(const AnsiString &text);

      // ip4-network: four decimal octets, and no more. Stricter than what a
      // resolver would take - no leading zeros, no shorthand - because section
      // 12 spells the digits out.
      static bool IsValidIP4Network(const AnsiString &text);

      // ip6-network: the textual forms of RFC 4291 section 2.2, including the one
      // that ends in a dotted-quad.
      static bool IsValidIP6Network(const AnsiString &text);

      // A prefix length written after a "/". Returns false unless the text is
      // all digits, has no leading zero, and is within maximumLength - the ABNF
      // allows "0" on its own but not "032", and the suite has a case for each.
      static bool TryParsePrefixLength(const AnsiString &text, int maximumLength, int &length);

      // Whether every byte is one an SPF record may contain. RFC 7208 section
      // 3.1 restricts a record to 7-bit ASCII, and section 12's macro-literal
      // rules out the control characters and the space within a term.
      static bool IsAsciiPrintable(const AnsiString &text);

      // Whether a DNS query can be built from a name at all: RFC 7208 section
      // 4.3 wants labels that are neither empty nor over 63 characters, and a
      // name with more than one label. What that means differs by where the name
      // came from - for the domain being checked it is a "none", for the target
      // of a mechanism it is a name that does not exist - so this only answers
      // the question and leaves the consequence to the caller.
      static bool IsValidDomainName(const AnsiString &text);

      // ASCII lower case, and only ASCII. RFC 4343 compares DNS names without
      // regard to case for the letters A-Z and for nothing else, so folding
      // through std::ctype would fold bytes a name may legitimately carry.
      static char ToLowerAscii(char character);

      // Whether two DNS names are the same name, and whether one ends in the
      // other, both compared as RFC 4343 compares them.
      static bool EqualsDnsName(const AnsiString &left, const AnsiString &right);
      static bool EndsWithDnsName(const AnsiString &name, const AnsiString &suffix);

      // name, as an unknown modifier's name must be spelled: a letter, then
      // letters, digits, "-", "_" and ".". This is why "1up=foo" and "=all" are
      // permerrors while "moo.cow-far_out=man:dog/cat" is a modifier nobody
      // knows and everybody ignores.
      static bool IsValidModifierName(const AnsiString &text);
   };
}
