// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

#include "SPFAddress.h"

namespace HM
{
   class SPFDnsLookup;

   // The macro expansion of RFC 7208 section 7: what turns the target a record
   // writes - "%{ir}.%{v}._spf.%{d2}" - into the name a mechanism is evaluated
   // against, and the text an exp modifier points at into the explanation of a
   // fail.
   //
   // Separate from the record parser because the two see different things. A
   // record is parsed once, knowing nothing about the message; an expansion
   // depends on the client address, the sender and the domain currently being
   // evaluated, and the last of those changes as include and redirect move the
   // evaluation along. Section 7.1 also does not re-parse what an expansion
   // produces, so a target is expanded exactly once, when the mechanism is
   // reached.
   //
   // One of these is made per SPF check. It holds the inputs of section 4.1
   // which do not change over the check, and it remembers the p macro's
   // validated names, which cost DNS queries and depend only on the client
   // address.
   class SPFMacroExpander
   {
   public:

      // The three inputs of RFC 7208 section 4.1. A sender with no local part
      // gets "postmaster" for one, and an empty sender means the check is being
      // made against the HELO argument, which section 2.4 has stand in as
      // "postmaster@" that host.
      SPFMacroExpander(std::shared_ptr<SPFDnsLookup> lookup,
                       const SPFAddress &clientAddress,
                       const AnsiString &sender,
                       const AnsiString &heloHost);

      // The name of the host doing the checking, which the r macro of section
      // 7.2 expands to. Section 7.3 leaves the string to the implementation
      // where it is not known, and "unknown" is what this uses until told
      // otherwise.
      void SetReceivingHost(const AnsiString &name);

      // What the t macro expands to: the time of the check, in seconds since the
      // epoch. Given rather than read from the clock, so that an expansion is
      // reproducible and a test can assert one.
      void SetTimestamp(__int64 secondsSinceEpoch);

      // Expands the target of a mechanism or of a redirect or exp modifier, and
      // leaves it ready to be looked up: a trailing dot removed, and the name
      // shortened from the left if section 7.1's limit of 253 characters was
      // passed.
      //
      // domain is <domain>, the domain being evaluated, which the d macro
      // expands to and which the p macro prefers a name from.
      //
      // Returns false if the text is not a macro-string, or uses a macro a
      // record's own terms may not - which section 4.6 makes a permerror. The
      // parser has already looked at everything a record carries, so this only
      // fires for text which has not been through it.
      bool ExpandDomainSpec(const AnsiString &domainSpec, const AnsiString &domain, AnsiString &expanded);

      // Expands the text an exp modifier fetched, which may use the c, r and t
      // macros of section 7.2 as well. No trailing dot is removed and no length
      // limit applies: the result is a message to a person, not a name to look
      // up.
      //
      // Returns false if the text is not a macro-string. Unlike a record's own
      // terms this is not a permerror - section 6.2 has the receiver fall back
      // to its own explanation - and unlike a record's own terms, nothing has
      // checked the text before now: it came out of a TXT record which only the
      // domain that published it has ever looked at.
      bool ExpandExplanation(const AnsiString &text, const AnsiString &domain, AnsiString &expanded);

      // The names the client's reverse mapping gives which resolve back to the
      // client address, in the order the mapping gave them. The p macro of
      // section 7.3 is one consumer; the ptr mechanism of section 5.5 is the
      // other, and it wants the whole list rather than the one name the macro
      // settles on.
      //
      // Here rather than in the evaluator because the work is the same for both
      // and is done once per check: the lookups depend on the client address
      // alone, and section 4.6.4 counts them.
      const std::vector<AnsiString> &GetValidatedNames();

      // How many of the names a reverse lookup returns the p macro will check.
      // Section 7.3 caps it, because the names are whatever the client's own
      // reverse zone says and validating each costs a query.
      static const int MaximumPtrNames = 10;

   private:

      bool Expand_(const AnsiString &text, bool asDomainName, const AnsiString &domain, AnsiString &expanded);

      // The value one macro letter stands for, before the transformers of
      // section 7.3 are applied to it.
      AnsiString GetMacroValue_(char letter, const AnsiString &domain);

      // The p macro, section 7.3: a name from the client's reverse mapping which
      // resolves back to the client address, preferring <domain> itself and then
      // a subdomain of it. "unknown" where nothing validates.
      AnsiString GetValidatedName_(const AnsiString &domain);

      void FindValidatedNames_();

      std::shared_ptr<SPFDnsLookup> lookup_;

      SPFAddress client_address_;

      // <sender>, and the two halves of it the l and o macros expand to.
      AnsiString sender_;
      AnsiString local_part_;
      AnsiString sender_domain_;

      AnsiString helo_host_;
      AnsiString receiving_host_;

      __int64 timestamp_;

      // The p macro's answer depends on the client address alone, which does not
      // change over a check, so the lookups behind it are made once. Which of
      // the names is preferred does depend on <domain>, so the list is what is
      // kept rather than the answer.
      bool validated_names_known_;
      std::vector<AnsiString> validated_names_;
   };
}
