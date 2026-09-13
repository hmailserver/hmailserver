// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

#include "SPFAddress.h"

namespace HM
{
   class SPFDnsLookup;

   // Macro expansion, RFC 7208 section 7: what turns a term's target into the name
   // a mechanism is evaluated against, and the text an exp modifier points at into
   // the explanation of a fail. One per check - see README.md.
   class SPFMacroExpander
   {
   public:

      // The three inputs of section 4.1. A sender with no local part gets
      // "postmaster" for one; an empty sender means the check is for the HELO
      // argument, which section 2.4 has stand in as postmaster at that host.
      SPFMacroExpander(std::shared_ptr<SPFDnsLookup> lookup,
                       const SPFAddress &clientAddress,
                       const AnsiString &sender,
                       const AnsiString &heloHost);

      // What the r macro of section 7.2 expands to. Section 7.3 leaves the string to
      // the implementation where it is not known, and this uses "unknown".
      void SetReceivingHost(const AnsiString &name);

      // What the t macro expands to. Given rather than read from the clock, so that
      // an expansion is reproducible and a test can assert one.
      void SetTimestamp(__int64 secondsSinceEpoch);

      // Expands a term's target and leaves it ready to look up: trailing dot removed,
      // and shortened from the left past section 7.1's 253 characters. False where
      // the text is not a macro-string, which section 4.6 makes a permerror.
      bool ExpandDomainSpec(const AnsiString &domainSpec, const AnsiString &domain, AnsiString &expanded);

      // Expands the text an exp modifier fetched, which may also use c, r and t. No
      // dot is removed and no limit applies. False where it does not parse - which
      // section 6.2 answers with the receiver's own words, not an error.
      bool ExpandExplanation(const AnsiString &text, const AnsiString &domain, AnsiString &expanded);

      // The validated names of section 5.5, which the ptr mechanism needs as well as
      // the p macro. Looked up once per check, because section 4.6.4 counts them.
      const std::vector<AnsiString> &GetValidatedNames();

      // Section 7.3 caps how many names a reverse lookup is followed into.
      static const int MaximumPtrNames = 10;

   private:

      bool Expand_(const AnsiString &text, bool asDomainName, const AnsiString &domain, AnsiString &expanded);

      // What one macro letter stands for, before section 7.3's transformers.
      AnsiString GetMacroValue_(char letter, const AnsiString &domain);

      // The p macro, section 7.3: a name from the client's reverse mapping that
      // resolves back to it, preferring <domain> and then a subdomain of it.
      // "unknown" where nothing validates.
      AnsiString GetValidatedName_(const AnsiString &domain);

      void FindValidatedNames_();

      std::shared_ptr<SPFDnsLookup> lookup_;

      SPFAddress client_address_;

      // <sender>, and the two halves the l and o macros expand to.
      AnsiString sender_;
      AnsiString local_part_;
      AnsiString sender_domain_;

      AnsiString helo_host_;
      AnsiString receiving_host_;

      __int64 timestamp_;

      // The p macro's lookups depend on the client address alone, so they are made
      // once; which name is preferred depends on <domain>, so the list is kept.
      bool validated_names_known_;
      std::vector<AnsiString> validated_names_;
   };
}
