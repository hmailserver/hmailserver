// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "stdafx.h"

#include "SPFRecord.h"

#include "SPFSyntax.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   namespace
   {
      const char *VERSION = "v=spf1";
      const int VERSION_LENGTH = 6;

      // RFC 7208 spells the mechanism and modifier names in lower case, and an
      // ABNF literal is case insensitive, so a record may spell them any way.
      bool EqualsNoCase(const AnsiString &left, const char *right)
      {
         return left.CompareNoCase(right) == 0;
      }

      bool StartsWithNoCase(const AnsiString &text, const char *prefix)
      {
         AnsiString wanted = prefix;

         if (text.GetLength() < wanted.GetLength())
            return false;

         return text.Left(wanted.GetLength()).CompareNoCase(wanted) == 0;
      }

      // The terms of a record, which section 12 separates by one or more spaces
      // and allows to be followed by more of them.
      std::vector<AnsiString> SplitTerms(const AnsiString &text)
      {
         std::vector<AnsiString> terms;
         AnsiString current;

         for (int i = 0; i < text.GetLength(); i++)
         {
            if (text[i] == ' ')
            {
               if (!current.IsEmpty())
                  terms.push_back(current);

               current = "";
               continue;
            }

            current += text[i];
         }

         if (!current.IsEmpty())
            terms.push_back(current);

         return terms;
      }

      // The position of the slash that introduces a trailing run of digits, and
      // the digits themselves. Returns -1 when the term does not end that way,
      // which is what lets "a:foo:bar/baz.example.com" keep its slash: what
      // follows it is not digits, so there is no length there to read.
      int FindTrailingLength(const AnsiString &term, AnsiString &digits)
      {
         int slash = term.ReverseFind('/');

         if (slash < 0)
            return -1;

         digits = term.Mid(slash + 1);

         if (digits.IsEmpty())
            return -1;

         for (int i = 0; i < digits.GetLength(); i++)
         {
            if (digits[i] < '0' || digits[i] > '9')
               return -1;
         }

         return slash;
      }

      // Reads the dual-cidr-length that the a and mx mechanisms of sections 5.3
      // and 5.4 may carry, and takes it off the end of the term:
      //
      //   dual-cidr-length = [ "/" ip4-cidr-length ] [ "//" ip6-cidr-length ]
      //
      // Returns false if what is written there is meant to be a length but is
      // not a valid one - a length over the family's maximum, one written with a
      // leading zero, or the two given in the wrong order.
      bool TryReadDualPrefixLengths(AnsiString &term, int &ip4Length, int &ip6Length)
      {
         AnsiString digits;
         int slash = FindTrailingLength(term, digits);

         if (slash < 0)
            return true;

         if (slash == 0 || term[slash - 1] != '/')
         {
            // A single slash, so an ip4 length, and nothing may precede it.
            // "a/24/64" arrives here with 64 as the ip4 length, and 64 is not
            // one - the second length has to be written with two slashes.
            if (!SPFSyntax::TryParsePrefixLength(digits, 32, ip4Length))
               return false;

            term = term.Left(slash);

            return true;
         }

         if (!SPFSyntax::TryParsePrefixLength(digits, 128, ip6Length))
            return false;

         term = term.Left(slash - 1);

         // An ip4 length may come before the ip6 one, and only before it.
         AnsiString moreDigits;
         int previous = FindTrailingLength(term, moreDigits);

         if (previous < 0)
            return true;

         if (previous > 0 && term[previous - 1] == '/')
            return false;

         if (!SPFSyntax::TryParsePrefixLength(moreDigits, 32, ip4Length))
            return false;

         term = term.Left(previous);

         return true;
      }

      // Reads the one prefix length an ip4 or ip6 mechanism may carry, section
      // 5.6, and takes it off the end of the term. The dual form belongs to a
      // and mx alone, so "ip4:1.2.3.4//32" and "ip6:::1.1.1.1//33" are mistakes
      // rather than lengths.
      bool TryReadSinglePrefixLength(AnsiString &term, int maximumLength, int &length)
      {
         AnsiString digits;
         int slash = FindTrailingLength(term, digits);

         if (slash < 0)
            return true;

         if (slash > 0 && term[slash - 1] == '/')
            return false;

         if (!SPFSyntax::TryParsePrefixLength(digits, maximumLength, length))
            return false;

         term = term.Left(slash);

         return true;
      }
   }

   SPFMechanism::SPFMechanism(Type type, Qualifier qualifier) :
      type_(type),
      qualifier_(qualifier),
      has_domain_spec_(false),
      ip4_prefix_length_(32),
      ip6_prefix_length_(128)
   {

   }

   void
   SPFMechanism::SetDomainSpec(const AnsiString &domainSpec)
   {
      domain_spec_ = domainSpec;
      has_domain_spec_ = true;
   }

   SPFRecord::SPFRecord() :
      has_redirect_(false),
      has_explanation_(false)
   {

   }

   bool
   SPFRecord::HasVersionTag(const AnsiString &textRecord)
   {
      if (!StartsWithNoCase(textRecord, VERSION))
         return false;

      // Section 4.5: the version has to be followed by a space or be the whole
      // record, so that "v=spf10" is a record about something else and
      // "v=spf1mx" is not a record at all.
      return textRecord.GetLength() == VERSION_LENGTH ||
             textRecord[VERSION_LENGTH] == ' ';
   }

   bool
   SPFRecord::Parse(const AnsiString &textRecord, SPFRecord &record, AnsiString &error)
   {
      record = SPFRecord();
      error = "";

      if (!HasVersionTag(textRecord))
      {
         error = "The record does not begin with the SPF version.";
         return false;
      }

      // Section 3.1: a record is 7-bit ASCII, so a byte which is not says the
      // record cannot be evaluated rather than that it does not apply.
      if (!SPFSyntax::IsAsciiPrintable(textRecord))
      {
         error = "The record contains a byte which is not printable ASCII.";
         return false;
      }

      std::vector<AnsiString> terms = SplitTerms(textRecord);

      // The whole record is checked before any of it is used. Section 4.6
      // requires it: a mistake after a mechanism which would have matched is
      // still a mistake, and the record is a permerror either way.
      for (size_t i = 1; i < terms.size(); i++)
      {
         if (!ParseTerm_(terms[i], record, error))
            return false;
      }

      return true;
   }

   bool
   SPFRecord::ParseTerm_(const AnsiString &term, SPFRecord &record, AnsiString &error)
   {
      // A modifier is told from a directive by an "=" before any ":" or "/",
      // which is what makes "redirect:t2.example.com" a mechanism nobody has
      // heard of rather than a misspelled modifier.
      for (int i = 0; i < term.GetLength(); i++)
      {
         char character = term[i];

         if (character == ':' || character == '/')
            break;

         if (character != '=')
            continue;

         return ParseModifier_(term.Left(i), term.Mid(i + 1), record, error);
      }

      return ParseDirective_(term, record, error);
   }

   bool
   SPFRecord::ParseDirective_(const AnsiString &term, SPFRecord &record, AnsiString &error)
   {
      AnsiString rest = term;
      SPFMechanism::Qualifier qualifier = SPFMechanism::Qualifier::Pass;

      if (!rest.IsEmpty())
      {
         switch (rest[0])
         {
         case '+':
            qualifier = SPFMechanism::Qualifier::Pass;
            rest = rest.Mid(1);
            break;
         case '-':
            qualifier = SPFMechanism::Qualifier::Fail;
            rest = rest.Mid(1);
            break;
         case '~':
            qualifier = SPFMechanism::Qualifier::SoftFail;
            rest = rest.Mid(1);
            break;
         case '?':
            qualifier = SPFMechanism::Qualifier::Neutral;
            rest = rest.Mid(1);
            break;
         default:
            break;
         }
      }

      // The name runs up to the ":" which introduces an argument, or to the "/"
      // which introduces a prefix length.
      AnsiString name = rest;
      AnsiString remainder;

      for (int i = 0; i < rest.GetLength(); i++)
      {
         if (rest[i] != ':' && rest[i] != '/')
            continue;

         name = rest.Left(i);
         remainder = rest.Mid(i);
         break;
      }

      if (EqualsNoCase(name, "all"))
         return ParseAll_(qualifier, remainder, record, error);

      if (EqualsNoCase(name, "include"))
         return ParseWithRequiredDomain_(SPFMechanism::Type::Include, qualifier, remainder, record, error);

      if (EqualsNoCase(name, "exists"))
         return ParseWithRequiredDomain_(SPFMechanism::Type::Exists, qualifier, remainder, record, error);

      if (EqualsNoCase(name, "a"))
         return ParseAddressMechanism_(SPFMechanism::Type::A, qualifier, remainder, record, error);

      if (EqualsNoCase(name, "mx"))
         return ParseAddressMechanism_(SPFMechanism::Type::MX, qualifier, remainder, record, error);

      if (EqualsNoCase(name, "ptr"))
         return ParsePtr_(qualifier, remainder, record, error);

      if (EqualsNoCase(name, "ip4"))
         return ParseIP_(SPFMechanism::Type::IP4, qualifier, remainder, record, error);

      if (EqualsNoCase(name, "ip6"))
         return ParseIP_(SPFMechanism::Type::IP6, qualifier, remainder, record, error);

      error = "Unknown term: " + term;

      return false;
   }

   bool
   SPFRecord::ParseAll_(SPFMechanism::Qualifier qualifier, const AnsiString &remainder, SPFRecord &record, AnsiString &error)
   {
      // Section 5.1: all takes nothing at all, not an argument and not a prefix
      // length.
      if (!remainder.IsEmpty())
      {
         error = "The all mechanism takes no argument.";
         return false;
      }

      record.mechanisms_.push_back(SPFMechanism(SPFMechanism::Type::All, qualifier));

      return true;
   }

   bool
   SPFRecord::ParseWithRequiredDomain_(SPFMechanism::Type type, SPFMechanism::Qualifier qualifier, const AnsiString &remainder, SPFRecord &record, AnsiString &error)
   {
      // Sections 5.2 and 5.7: include and exists must name a domain, and take no
      // prefix length.
      if (remainder.IsEmpty() || remainder[0] != ':')
      {
         error = "The mechanism needs a domain.";
         return false;
      }

      AnsiString domainSpec = remainder.Mid(1);

      if (!SPFSyntax::IsValidDomainSpec(domainSpec))
      {
         error = "Not a valid domain: " + domainSpec;
         return false;
      }

      SPFMechanism mechanism(type, qualifier);
      mechanism.SetDomainSpec(domainSpec);

      record.mechanisms_.push_back(mechanism);

      return true;
   }

   bool
   SPFRecord::ParseAddressMechanism_(SPFMechanism::Type type, SPFMechanism::Qualifier qualifier, const AnsiString &remainder, SPFRecord &record, AnsiString &error)
   {
      // Sections 5.3 and 5.4: a and mx may name a domain, and may carry either
      // or both prefix lengths.
      SPFMechanism mechanism(type, qualifier);

      AnsiString rest = remainder;
      int ip4Length = 32;
      int ip6Length = 128;

      if (!TryReadDualPrefixLengths(rest, ip4Length, ip6Length))
      {
         error = "Not a valid prefix length: " + remainder;
         return false;
      }

      mechanism.SetIP4PrefixLength(ip4Length);
      mechanism.SetIP6PrefixLength(ip6Length);

      if (!rest.IsEmpty())
      {
         if (rest[0] != ':')
         {
            error = "Not a valid mechanism argument: " + remainder;
            return false;
         }

         AnsiString domainSpec = rest.Mid(1);

         if (!SPFSyntax::IsValidDomainSpec(domainSpec))
         {
            error = "Not a valid domain: " + domainSpec;
            return false;
         }

         mechanism.SetDomainSpec(domainSpec);
      }

      record.mechanisms_.push_back(mechanism);

      return true;
   }

   bool
   SPFRecord::ParsePtr_(SPFMechanism::Qualifier qualifier, const AnsiString &remainder, SPFRecord &record, AnsiString &error)
   {
      // Section 5.5: ptr may name a domain, and takes no prefix length.
      SPFMechanism mechanism(SPFMechanism::Type::PTR, qualifier);

      if (!remainder.IsEmpty())
      {
         if (remainder[0] != ':')
         {
            error = "Not a valid ptr argument: " + remainder;
            return false;
         }

         AnsiString domainSpec = remainder.Mid(1);

         if (!SPFSyntax::IsValidDomainSpec(domainSpec))
         {
            error = "Not a valid domain: " + domainSpec;
            return false;
         }

         mechanism.SetDomainSpec(domainSpec);
      }

      record.mechanisms_.push_back(mechanism);

      return true;
   }

   bool
   SPFRecord::ParseIP_(SPFMechanism::Type type, SPFMechanism::Qualifier qualifier, const AnsiString &remainder, SPFRecord &record, AnsiString &error)
   {
      // Section 5.6: ip4 and ip6 must carry an address, and may carry the one
      // prefix length that goes with their family - never both.
      if (remainder.IsEmpty() || remainder[0] != ':')
      {
         error = "The mechanism needs an address.";
         return false;
      }

      SPFMechanism mechanism(type, qualifier);

      AnsiString rest = remainder.Mid(1);

      if (type == SPFMechanism::Type::IP4)
      {
         int ip4Length = 32;

         if (!TryReadSinglePrefixLength(rest, 32, ip4Length))
         {
            error = "Not a valid IPv4 prefix length: " + remainder;
            return false;
         }

         if (!SPFSyntax::IsValidIP4Network(rest))
         {
            error = "Not a valid IPv4 address: " + rest;
            return false;
         }

         mechanism.SetIP4PrefixLength(ip4Length);
      }
      else
      {
         int ip6Length = 128;

         if (!TryReadSinglePrefixLength(rest, 128, ip6Length))
         {
            error = "Not a valid IPv6 prefix length: " + remainder;
            return false;
         }

         if (!SPFSyntax::IsValidIP6Network(rest))
         {
            error = "Not a valid IPv6 address: " + rest;
            return false;
         }

         mechanism.SetIP6PrefixLength(ip6Length);
      }

      mechanism.SetAddress(rest);

      record.mechanisms_.push_back(mechanism);

      return true;
   }

   bool
   SPFRecord::ParseModifier_(const AnsiString &name, const AnsiString &value, SPFRecord &record, AnsiString &error)
   {
      if (!SPFSyntax::IsValidModifierName(name))
      {
         error = "Not a valid modifier name: " + name;
         return false;
      }

      // Section 6: redirect and exp may each appear once. Twice is a permerror
      // rather than a case of the last one winning.
      if (EqualsNoCase(name, "redirect"))
      {
         if (record.has_redirect_)
         {
            error = "The record carries more than one redirect modifier.";
            return false;
         }

         if (!SPFSyntax::IsValidDomainSpec(value))
         {
            error = "Not a valid redirect domain: " + value;
            return false;
         }

         record.has_redirect_ = true;
         record.redirect_domain_spec_ = value;

         return true;
      }

      if (EqualsNoCase(name, "exp"))
      {
         if (record.has_explanation_)
         {
            error = "The record carries more than one exp modifier.";
            return false;
         }

         if (!SPFSyntax::IsValidDomainSpec(value))
         {
            error = "Not a valid exp domain: " + value;
            return false;
         }

         record.has_explanation_ = true;
         record.explanation_domain_spec_ = value;

         return true;
      }

      // Section 6.3: a modifier nobody knows is ignored, but it still has to be
      // spelled like one. Its argument is a macro-string, not a domain, so
      // "default=+" is a modifier and not a mistake.
      if (!SPFSyntax::IsValidMacroString(value, SPFSyntax::MacroSet::RecordTerm))
      {
         error = "Not a valid modifier value: " + value;
         return false;
      }

      return true;
   }
}
