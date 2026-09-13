// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "stdafx.h"

#include "SPFMacroExpander.h"

#include "SPFDnsLookup.h"
#include "SPFSyntax.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   namespace
   {
      // RFC 7208 section 4.3: where the sender has no local part, "postmaster"
      // stands in for one.
      const char *POSTMASTER = "postmaster";

      // Splits a macro's value at every one of the delimiter characters. Empty
      // parts are kept: what is between two delimiters is a part, and dropping
      // it would move the parts a digit transformer counts from.
      std::vector<AnsiString> SplitAtAny(const AnsiString &text, const AnsiString &delimiters)
      {
         std::vector<AnsiString> parts;
         AnsiString current;

         for (int i = 0; i < text.GetLength(); i++)
         {
            if (delimiters.Find(text[i]) >= 0)
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

      // The transformers of section 7.3, in the order it applies them: split at the
      // delimiters, reverse if "r" was written, keep the digits' worth of right-hand
      // parts, and rejoin with dots whatever they were split at - see README.md.
      AnsiString Transform(const AnsiString &value, const SPFSyntax::Macro &macro)
      {
         AnsiString delimiters = macro.delimiters;

         if (delimiters.IsEmpty())
            delimiters = ".";

         std::vector<AnsiString> parts = SplitAtAny(value, delimiters);

         if (macro.reverse)
         {
            for (int low = 0, high = (int) parts.size(); low + 1 < high; low++)
            {
               high--;

               AnsiString held = parts[low];
               parts[low] = parts[high];
               parts[high] = held;
            }
         }

         // A count of more parts than there are means all of them, and so does a
         // count of none.
         int count = (int) parts.size();
         int first = 0;

         if (macro.digits > 0 && macro.digits < count)
            first = count - macro.digits;

         AnsiString text;

         for (int i = first; i < count; i++)
         {
            if (i > first)
               text += '.';

            text += parts[i];
         }

         return text;
      }

      // Section 7.1: an upper case macro letter means the expansion is URL escaped. RFC
      // 3986 section 2.3 leaves the unreserved characters alone - letters, digits, "-",
      // ".", "_" and "~" - and everything else becomes a percent sign and two hex digits.
      AnsiString UrlEscape(const AnsiString &text)
      {
         const char *hexDigits = "0123456789ABCDEF";

         AnsiString escaped;

         for (int i = 0; i < text.GetLength(); i++)
         {
            unsigned char byte = (unsigned char) text[i];

            bool unreserved = (byte >= 'a' && byte <= 'z') ||
                              (byte >= 'A' && byte <= 'Z') ||
                              (byte >= '0' && byte <= '9') ||
                              byte == '-' || byte == '.' || byte == '_' || byte == '~';

            if (unreserved)
            {
               escaped += (char) byte;
               continue;
            }

            escaped += '%';
            escaped += hexDigits[byte >> 4];
            escaped += hexDigits[byte & 0x0F];
         }

         return escaped;
      }

      AnsiString ToDecimal(__int64 value)
      {
         if (value < 0)
            value = 0;

         char digits[24];
         int count = 0;

         do
         {
            digits[count++] = (char) ('0' + (int) (value % 10));
            value /= 10;
         }
         while (value > 0 && count < (int) sizeof(digits));

         AnsiString text;

         while (count > 0)
            text += digits[--count];

         return text;
      }
   }

   SPFMacroExpander::SPFMacroExpander(std::shared_ptr<SPFDnsLookup> lookup,
                                      const SPFAddress &clientAddress,
                                      const AnsiString &sender,
                                      const AnsiString &heloHost) :
      lookup_(lookup),
      client_address_(clientAddress),
      helo_host_(heloHost),
      receiving_host_("unknown"),
      timestamp_(0),
      validated_names_known_(false)
   {
      // RFC 7208 section 2.4: a check made without a sender is made for the HELO
      // argument, and <sender> is then postmaster at that host. Section 4.3 fills
      // the same local part in for a sender which was given without one.
      if (sender.IsEmpty())
      {
         local_part_ = POSTMASTER;
         sender_domain_ = heloHost;
         sender_ = AnsiString(POSTMASTER) + "@" + heloHost;

         return;
      }

      int at = sender.ReverseFind('@');

      if (at < 0)
      {
         // Not an address, so there is no local part to take. Nothing reaches here in the
         // server - a sender the evaluation found no domain in never gets as far as a record
         // - but the macros still have to have values.
         local_part_ = POSTMASTER;
         sender_domain_ = sender;
         sender_ = AnsiString(POSTMASTER) + "@" + sender;

         return;
      }

      local_part_ = sender.Left(at);
      sender_domain_ = sender.Mid(at + 1);

      if (local_part_.IsEmpty())
      {
         local_part_ = POSTMASTER;
         sender_ = AnsiString(POSTMASTER) + sender;

         return;
      }

      sender_ = sender;
   }

   void
   SPFMacroExpander::SetReceivingHost(const AnsiString &name)
   {
      receiving_host_ = name;
   }

   void
   SPFMacroExpander::SetTimestamp(__int64 secondsSinceEpoch)
   {
      timestamp_ = secondsSinceEpoch;
   }

   bool
   SPFMacroExpander::ExpandDomainSpec(const AnsiString &domainSpec, const AnsiString &domain, AnsiString &expanded)
   {
      return Expand_(domainSpec, true, domain, expanded);
   }

   bool
   SPFMacroExpander::ExpandExplanation(const AnsiString &text, const AnsiString &domain, AnsiString &expanded)
   {
      return Expand_(text, false, domain, expanded);
   }

   bool
   SPFMacroExpander::Expand_(const AnsiString &text, bool asDomainName, const AnsiString &domain, AnsiString &expanded)
   {
      expanded = "";

      SPFSyntax::MacroSet macros = asDomainName ? SPFSyntax::MacroSet::RecordTerm
                                                : SPFSyntax::MacroSet::ExplanationText;

      // Checked before anything is expanded rather than as the walk goes, so that a string
      // which goes wrong halfway through expands to nothing at all: a half-built name is
      // not something either caller could use.
      if (!SPFSyntax::IsValidMacroString(text, macros))
         return false;

      AnsiString result;
      int at = 0;

      while (at < text.GetLength())
      {
         if (text[at] != '%')
         {
            result += text[at];
            at++;

            continue;
         }

         SPFSyntax::Macro macro;

         if (!SPFSyntax::TryReadMacro(text, at, macros, macro))
         {
            // IsValidMacroString walked the same string with the same reader, so
            // this cannot happen. Handled rather than asserted because the two
            // must not be able to disagree silently.
            return false;
         }

         at = macro.end;

         if (macro.is_literal)
         {
            // Section 7.1: "%%" is a percent sign, "%_" a space, and "%-" the
            // way a space is written in a URL.
            switch (macro.literal)
            {
            case '%':
               result += '%';
               break;

            case '_':
               result += ' ';
               break;

            default:
               result += "%20";
               break;
            }

            continue;
         }

         AnsiString value = Transform(GetMacroValue_(SPFSyntax::ToLowerAscii(macro.letter), domain), macro);

         if (macro.letter != SPFSyntax::ToLowerAscii(macro.letter))
            value = UrlEscape(value);

         result += value;
      }

      if (asDomainName)
      {
         // A trailing dot says only that the name is already absolute, and the
         // length below is the length of the name without it.
         if (!result.IsEmpty() && result[result.GetLength() - 1] == '.')
            result = result.Left(result.GetLength() - 1);

         // Section 7.1: a name too long to be asked about loses whole labels
         // from the left until it fits.
         while (result.GetLength() > 253)
         {
            int dot = result.Find('.');

            if (dot < 0)
               break;

            result = result.Mid(dot + 1);
         }
      }

      expanded = result;

      return true;
   }

   AnsiString
   SPFMacroExpander::GetMacroValue_(char letter, const AnsiString &domain)
   {
      switch (letter)
      {
      case 's':
         return sender_;

      case 'l':
         return local_part_;

      case 'o':
         return sender_domain_;

      case 'd':
         return domain;

      case 'i':
         return client_address_.GetDottedForm();

      case 'p':
         return GetValidatedName_(domain);

      case 'h':
         return helo_host_;

      case 'v':
         return client_address_.GetArpaLabel();

      case 'c':
         return client_address_.GetReadableForm();

      case 'r':
         return receiving_host_;

      case 't':
         return ToDecimal(timestamp_);
      }

      // TryReadMacro accepts no other letter, so there is nothing to fall back
      // to and nothing that could make use of one.
      return "";
   }

   const std::vector<AnsiString> &
   SPFMacroExpander::GetValidatedNames()
   {
      if (!validated_names_known_)
      {
         validated_names_known_ = true;
         FindValidatedNames_();
      }

      return validated_names_;
   }

   AnsiString
   SPFMacroExpander::GetValidatedName_(const AnsiString &domain)
   {
      GetValidatedNames();

      if (validated_names_.empty())
      {
         // Section 7.3: no validated name, or a DNS error while looking for one,
         // and the macro expands to "unknown". This is why the p macro cannot
         // produce a temperror the way a mechanism can.
         return "unknown";
      }

      // Section 7.3 wants <domain> itself where it validated, then a subdomain
      // of it, and is content with any of the names otherwise.
      for (int i = 0; i < (int) validated_names_.size(); i++)
      {
         if (SPFSyntax::EqualsDnsName(validated_names_[i], domain))
            return validated_names_[i];
      }

      AnsiString suffix = AnsiString(".") + domain;

      for (int i = 0; i < (int) validated_names_.size(); i++)
      {
         if (SPFSyntax::EndsWithDnsName(validated_names_[i], suffix))
            return validated_names_[i];
      }

      return validated_names_[0];
   }

   void
   SPFMacroExpander::FindValidatedNames_()
   {
      std::vector<AnsiString> names;

      if (!lookup_->GetPTRRecords(client_address_.GetReverseName(), names))
         return;

      if ((int) names.size() > MaximumPtrNames)
         names.resize(MaximumPtrNames);

      for (int i = 0; i < (int) names.size(); i++)
      {
         std::vector<AnsiString> addresses;

         // Section 5.5 validates a name against the family the client connected over, so an
         // IPv6 client is checked against the name's AAAA records and an IPv4 client against
         // its A records. A name which answers only for the other family does not validate.
         bool answered = (client_address_.GetFamily() == SPFAddress::Family::IP6)
                            ? lookup_->GetAAAARecords(names[i], addresses)
                            : lookup_->GetARecords(names[i], addresses);

         if (!answered)
            continue;

         for (int a = 0; a < (int) addresses.size(); a++)
         {
            SPFAddress parsed;

            if (!SPFAddress::TryParse(addresses[a], parsed))
               continue;

            if (!parsed.Equals(client_address_))
               continue;

            validated_names_.push_back(names[i]);
            break;
         }
      }
   }
}
