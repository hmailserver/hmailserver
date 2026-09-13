// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "stdafx.h"

#include "SPFSyntax.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   namespace
   {
      bool IsDigit(char character)
      {
         return character >= '0' && character <= '9';
      }

      bool IsAlpha(char character)
      {
         return (character >= 'a' && character <= 'z') ||
                (character >= 'A' && character <= 'Z');
      }

      bool IsAlphaNumeric(char character)
      {
         return IsAlpha(character) || IsDigit(character);
      }

      bool IsHexDigit(char character)
      {
         return IsDigit(character) ||
                (character >= 'a' && character <= 'f') ||
                (character >= 'A' && character <= 'F');
      }

      char ToLower(char character)
      {
         return SPFSyntax::ToLowerAscii(character);
      }

      // macro-literal: a visible character other than "%". RFC 7208 section 12
      // writes it as %x21-24 / %x26-7E, which is every printable character
      // except the space and the percent sign.
      //
      // The space is what section 12 separates a record's terms by, so a term
      // cannot hold one. The text an exp modifier fetches is not a term and is
      // spelled by a rule of its own which puts the space back:
      //
      //   explain-string = *( macro-string / SP )
      //
      // Without that, an explanation could not be a sentence.
      bool IsMacroLiteral(char character, SPFSyntax::MacroSet macros)
      {
         unsigned char byte = (unsigned char) character;

         if (byte == ' ')
            return macros == SPFSyntax::MacroSet::ExplanationText;

         return (byte >= 0x21 && byte <= 0x24) || (byte >= 0x26 && byte <= 0x7E);
      }

      // delimiter, RFC 7208 section 7.1. These are the characters a transformer
      // may be told to split on.
      bool IsMacroDelimiter(char character)
      {
         return character == '.' || character == '-' || character == '+' ||
                character == ',' || character == '/' || character == '_' ||
                character == '=';
      }

      // macro-letter. The names are spelled in lower case in section 7.1, and
      // ABNF makes a literal case insensitive, so the upper case forms name the
      // same macros - section 7.1 gives them the added meaning that the
      // expansion is URL escaped.
      bool IsMacroLetter(char character, SPFSyntax::MacroSet macros)
      {
         switch (ToLower(character))
         {
         case 's':
         case 'l':
         case 'o':
         case 'd':
         case 'i':
         case 'p':
         case 'h':
         case 'v':
            return true;

         case 'c':
         case 'r':
         case 't':
            // Only in the text an exp modifier points at, section 7.2.
            return macros == SPFSyntax::MacroSet::ExplanationText;
         }

         return false;
      }

      // toplabel, RFC 7208 section 12: a label which cannot be read as a
      // number, so that a domain-spec can never be mistaken for an address.
      //
      // toplabel = ( *alphanum ALPHA *alphanum ) /
      //            ( 1*alphanum "-" *( alphanum / "-" ) alphanum )
      bool IsTopLabel(const AnsiString &label)
      {
         int length = label.GetLength();

         if (length == 0)
            return false;

         bool hasAlpha = false;
         bool hasHyphen = false;

         for (int i = 0; i < length; i++)
         {
            char character = label[i];

            if (IsAlpha(character))
               hasAlpha = true;
            else if (character == '-')
               hasHyphen = true;
            else if (!IsDigit(character))
               return false;
         }

         // The first form: anything alphanumeric, so long as one of them is a
         // letter. No hyphens, which the second form is for.
         if (hasAlpha && !hasHyphen)
            return true;

         // The second form allows hyphens inside the label, but it may neither
         // begin nor end with one.
         if (hasHyphen && label[0] != '-' && label[length - 1] != '-')
            return true;

         return false;
      }

      // One octet of an ip4-network, as section 12 spells it out: 0-255, and
      // written without a leading zero.
      bool IsOctet(const AnsiString &text)
      {
         int length = text.GetLength();

         if (length < 1 || length > 3)
            return false;

         for (int i = 0; i < length; i++)
         {
            if (!IsDigit(text[i]))
               return false;
         }

         if (length > 1 && text[0] == '0')
            return false;

         int value = 0;

         for (int i = 0; i < length; i++)
            value = value * 10 + (text[i] - '0');

         return value <= 255;
      }

      // A group of an IPv6 address: one to four hex digits.
      bool IsHexGroup(const AnsiString &text)
      {
         int length = text.GetLength();

         if (length < 1 || length > 4)
            return false;

         for (int i = 0; i < length; i++)
         {
            if (!IsHexDigit(text[i]))
               return false;
         }

         return true;
      }

      std::vector<AnsiString> Split(const AnsiString &text, char separator)
      {
         std::vector<AnsiString> parts;
         AnsiString current;

         for (int i = 0; i < text.GetLength(); i++)
         {
            if (text[i] == separator)
            {
               parts.push_back(current);
               current = "";
               continue;
            }

            current += text[i];
         }

         parts.push_back(current);

         return parts;
      }
   }

   bool
   SPFSyntax::IsAsciiPrintable(const AnsiString &text)
   {
      for (int i = 0; i < text.GetLength(); i++)
      {
         unsigned char byte = (unsigned char) text[i];

         // The space is here because a whole record is being checked, and the
         // terms of a record are separated by spaces. Anything below it is a
         // control character and anything above 0x7e is not ASCII, and section
         // 3.1 allows neither.
         if (byte < 0x20 || byte > 0x7E)
            return false;
      }

      return true;
   }

   bool
   SPFSyntax::TryReadMacro(const AnsiString &text, int position, MacroSet macros, Macro &macro)
   {
      macro.is_literal = false;
      macro.literal = 0;
      macro.letter = 0;
      macro.digits = 0;
      macro.reverse = false;
      macro.delimiters = "";
      macro.end = position;

      int length = text.GetLength();

      if (position < 0 || position >= length || text[position] != '%')
         return false;

      if (position + 1 >= length)
      {
         // A percent sign at the very end of the string expands to nothing, and
         // is not allowed to stand alone.
         return false;
      }

      char next = text[position + 1];

      if (next == '%' || next == '_' || next == '-')
      {
         macro.is_literal = true;
         macro.literal = next;
         macro.end = position + 2;

         return true;
      }

      if (next != '{')
         return false;

      int at = position + 2;

      if (at >= length || !IsMacroLetter(text[at], macros))
         return false;

      macro.letter = text[at];
      at++;

      // The digits say how many of the right-hand parts to keep. The count is
      // capped rather than accumulated without limit: a count past the number of
      // parts means every part either way, and no expansion has more parts than
      // it has characters, so clamping changes no answer and cannot overflow.
      const int maximumDigits = 256;

      while (at < length && IsDigit(text[at]))
      {
         if (macro.digits < maximumDigits)
            macro.digits = macro.digits * 10 + (text[at] - '0');

         if (macro.digits > maximumDigits)
            macro.digits = maximumDigits;

         at++;
      }

      if (at < length && ToLower(text[at]) == 'r')
      {
         macro.reverse = true;
         at++;
      }

      int delimitersAt = at;

      while (at < length && IsMacroDelimiter(text[at]))
         at++;

      macro.delimiters = text.Mid(delimitersAt, at - delimitersAt);

      if (at >= length || text[at] != '}')
         return false;

      macro.end = at + 1;

      return true;
   }

   bool
   SPFSyntax::IsValidMacroString(const AnsiString &text, MacroSet macros)
   {
      int at = 0;

      while (at < text.GetLength())
      {
         if (text[at] == '%')
         {
            Macro macro;

            if (!TryReadMacro(text, at, macros, macro))
               return false;

            at = macro.end;
            continue;
         }

         if (!IsMacroLiteral(text[at], macros))
            return false;

         at++;
      }

      return true;
   }

   bool
   SPFSyntax::IsValidDomainSpec(const AnsiString &text)
   {
      if (text.IsEmpty())
         return false;

      if (!IsValidMacroString(text, MacroSet::RecordTerm))
         return false;

      // domain-end is either a macro expansion at the very end, or a top label
      // preceded by a dot and optionally followed by one.
      int at = 0;
      int lastExpansionEnd = -1;

      while (at < text.GetLength())
      {
         if (text[at] == '%')
         {
            // IsValidMacroString has already walked this, so every expansion
            // here reads cleanly.
            Macro macro;
            TryReadMacro(text, at, MacroSet::RecordTerm, macro);

            at = macro.end;
            lastExpansionEnd = at;
            continue;
         }

         at++;
      }

      if (lastExpansionEnd == text.GetLength())
         return true;

      AnsiString name = text;

      // A trailing dot only says the name is already absolute.
      if (name[name.GetLength() - 1] == '.')
         name = name.Left(name.GetLength() - 1);

      int lastDot = name.ReverseFind('.');

      if (lastDot < 0)
      {
         // No dot at all, so there is nothing that could be a top label. This
         // is what rules out "a:museum".
         return false;
      }

      return IsTopLabel(name.Mid(lastDot + 1));
   }

   bool
   SPFSyntax::IsValidIP4Network(const AnsiString &text)
   {
      std::vector<AnsiString> octets = Split(text, '.');

      if (octets.size() != 4)
         return false;

      for (size_t i = 0; i < octets.size(); i++)
      {
         if (!IsOctet(octets[i]))
            return false;
      }

      return true;
   }

   bool
   SPFSyntax::IsValidIP6Network(const AnsiString &text)
   {
      // RFC 4291 section 2.2. Groups of one to four hex digits separated by
      // colons; at most one "::", standing for one or more groups of zeros; and
      // optionally a dotted-quad as the final group, standing for the last two.
      AnsiString before = text;
      AnsiString after;
      bool compressed = false;

      int doubleColon = text.Find("::");

      if (doubleColon >= 0)
      {
         compressed = true;
         before = text.Left(doubleColon);
         after = text.Mid(doubleColon + 2);

         // A second "::" would not say how many groups either stood for. This is
         // what rules out ":CAFE::BABE" once the leading colon is accounted for.
         if (after.Find("::") >= 0)
            return false;
      }

      // The groups as written, in order. Splitting a side which is empty would
      // produce one empty group, so an empty side contributes nothing instead.
      std::vector<AnsiString> groups;

      if (!before.IsEmpty())
      {
         std::vector<AnsiString> parts = Split(before, ':');
         groups.insert(groups.end(), parts.begin(), parts.end());
      }

      if (!after.IsEmpty())
      {
         std::vector<AnsiString> parts = Split(after, ':');
         groups.insert(groups.end(), parts.begin(), parts.end());
      }

      int written = 0;

      for (size_t i = 0; i < groups.size(); i++)
      {
         bool isLast = (i + 1 == groups.size());

         // A dotted-quad stands for the last two groups, so it may only be the
         // last group of the address. Anywhere else - "1.2.3.4::1" - it is just
         // a group with dots in it, and not a group at all.
         if (isLast && groups[i].Find('.') >= 0)
         {
            if (!IsValidIP4Network(groups[i]))
               return false;

            written += 2;
            continue;
         }

         // An empty group here is a stray colon: a leading or trailing one, or
         // two in a row somewhere other than the single "::".
         if (!IsHexGroup(groups[i]))
            return false;

         written++;
      }

      if (compressed)
      {
         // "::" stands for at least one group, so the groups written out have to
         // leave room for it.
         return written <= 7;
      }

      return written == 8;
   }

   bool
   SPFSyntax::TryParsePrefixLength(const AnsiString &text, int maximumLength, int &length)
   {
      if (text.IsEmpty())
         return false;

      for (int i = 0; i < text.GetLength(); i++)
      {
         if (!IsDigit(text[i]))
            return false;
      }

      // Section 12 writes this as ("0" / %x31-39 0*2DIGIT), so a zero on its own
      // is a length and "032" is not a length at all.
      if (text.GetLength() > 1 && text[0] == '0')
         return false;

      if (text.GetLength() > 3)
         return false;

      int value = 0;

      for (int i = 0; i < text.GetLength(); i++)
         value = value * 10 + (text[i] - '0');

      if (value > maximumLength)
         return false;

      length = value;

      return true;
   }

   bool
   SPFSyntax::IsValidDomainName(const AnsiString &text)
   {
      if (text.IsEmpty())
         return false;

      // A name of more than 253 characters cannot be asked about, RFC 1035
      // section 2.3.4.
      if (text.GetLength() > 253)
         return false;

      AnsiString name = text;

      // A trailing dot only says the name is already absolute, and leaves an
      // empty label behind which is not the kind section 4.3 objects to.
      if (name[name.GetLength() - 1] == '.')
         name = name.Left(name.GetLength() - 1);

      if (name.IsEmpty())
         return false;

      std::vector<AnsiString> labels = Split(name, '.');

      if (labels.size() < 2)
      {
         // A single label is not a fully qualified name, which is what makes the
         // HELO argument "A2345678" nothing an SPF check can be done against.
         return false;
      }

      for (size_t i = 0; i < labels.size(); i++)
      {
         if (labels[i].IsEmpty() || labels[i].GetLength() > 63)
            return false;
      }

      return true;
   }

   char
   SPFSyntax::ToLowerAscii(char character)
   {
      if (character >= 'A' && character <= 'Z')
         return (char) (character - 'A' + 'a');

      return character;
   }

   bool
   SPFSyntax::EqualsDnsName(const AnsiString &left, const AnsiString &right)
   {
      if (left.GetLength() != right.GetLength())
         return false;

      for (int i = 0; i < left.GetLength(); i++)
      {
         if (ToLowerAscii(left[i]) != ToLowerAscii(right[i]))
            return false;
      }

      return true;
   }

   bool
   SPFSyntax::EndsWithDnsName(const AnsiString &name, const AnsiString &suffix)
   {
      if (name.GetLength() < suffix.GetLength())
         return false;

      return EqualsDnsName(name.Right(suffix.GetLength()), suffix);
   }

   bool
   SPFSyntax::IsValidModifierName(const AnsiString &text)
   {
      if (text.IsEmpty())
         return false;

      if (!IsAlpha(text[0]))
         return false;

      for (int i = 1; i < text.GetLength(); i++)
      {
         char character = text[i];

         if (!IsAlphaNumeric(character) && character != '-' &&
             character != '_' && character != '.')
         {
            return false;
         }
      }

      return true;
   }
}
