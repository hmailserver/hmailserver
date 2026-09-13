// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

namespace HM
{
   // One directive of an SPF record: a mechanism, and the qualifier which says
   // what a match by that mechanism means. RFC 7208, sections 4.6.1 and 5.
   class SPFMechanism
   {
   public:

      enum class Type
      {
         All,
         Include,
         A,
         MX,
         PTR,
         IP4,
         IP6,
         Exists
      };

      // The result a match produces, RFC 7208 section 4.6.2. "+" is the default
      // where a directive carries no qualifier.
      enum class Qualifier
      {
         Pass,
         Fail,
         SoftFail,
         Neutral
      };

      // No default constructor on purpose. A mechanism without a type and a
      // qualifier is not a mechanism, and the defaults one would have to pick
      // are "all" and "+" - the pair that authorizes every client there is.
      SPFMechanism(Type type, Qualifier qualifier);

      Type GetType() const { return type_; }
      Qualifier GetQualifier() const { return qualifier_; }

      // The mechanism's target, still as it was written: a macro-string which
      // the expander of RFC 7208 section 7 turns into a domain name when the
      // mechanism is evaluated. Empty where the mechanism carried none, which
      // for a, mx and ptr means the domain being evaluated.
      const AnsiString &GetDomainSpec() const { return domain_spec_; }
      bool GetHasDomainSpec() const { return has_domain_spec_; }

      void SetDomainSpec(const AnsiString &domainSpec);

      // The address literal of an ip4 or ip6 mechanism, as written.
      const AnsiString &GetAddress() const { return address_; }
      void SetAddress(const AnsiString &address) { address_ = address; }

      // The prefix lengths of RFC 7208 section 5.3. A mechanism which names
      // neither gets the full-length defaults, so a caller never has to ask
      // whether one was written.
      int GetIP4PrefixLength() const { return ip4_prefix_length_; }
      int GetIP6PrefixLength() const { return ip6_prefix_length_; }

      void SetIP4PrefixLength(int length) { ip4_prefix_length_ = length; }
      void SetIP6PrefixLength(int length) { ip6_prefix_length_ = length; }

   private:

      Type type_;
      Qualifier qualifier_;

      AnsiString domain_spec_;
      bool has_domain_spec_;

      AnsiString address_;

      int ip4_prefix_length_;
      int ip6_prefix_length_;
   };

   // An SPF record, as published in a domain's TXT record. RFC 7208, section 4.5.
   //
   // Parsing is separate from evaluating, and has to be: RFC 7208 section 4.6
   // requires the whole record to be checked before any of it is acted on, so a
   // syntax error after a mechanism which would have matched is still a
   // permerror. That is what the suite's detect-errors-anywhere case is for.
   //
   // A term's target is kept as written rather than expanded. Macro expansion
   // depends on the message being evaluated - the sender, the client address -
   // which a record knows nothing about, and RFC 7208 section 7.1 does not
   // re-check the syntax of what an expansion produces.
   class SPFRecord
   {
   public:

      SPFRecord();

      // Returns false if the text is not a syntactically valid SPF record, which
      // RFC 7208 section 4.6 makes a permerror. The error says what was wrong
      // with it, for the log.
      static bool Parse(const AnsiString &textRecord, SPFRecord &record, AnsiString &error);

      // True if the text begins with the version section of RFC 7208 section
      // 4.5: "v=spf1", compared without regard to case, followed by a space or
      // the end of the record. Used to pick the SPF record out of the other TXT
      // records a domain publishes.
      static bool HasVersionTag(const AnsiString &textRecord);

      // The directives, in the order they were written. RFC 7208 section 4.6.2
      // evaluates them in that order and the first match decides.
      const std::vector<SPFMechanism> &GetMechanisms() const { return mechanisms_; }

      // The redirect modifier of RFC 7208 section 6.1, used only when no
      // mechanism matched. Position does not matter - section 6 makes modifiers
      // position independent - so it is kept aside rather than among the
      // directives.
      bool GetHasRedirect() const { return has_redirect_; }
      const AnsiString &GetRedirectDomainSpec() const { return redirect_domain_spec_; }

      // The exp modifier of RFC 7208 section 6.2, naming the domain whose TXT
      // record explains a fail.
      bool GetHasExplanation() const { return has_explanation_; }
      const AnsiString &GetExplanationDomainSpec() const { return explanation_domain_spec_; }

   private:

      static bool ParseTerm_(const AnsiString &term, SPFRecord &record, AnsiString &error);
      static bool ParseDirective_(const AnsiString &term, SPFRecord &record, AnsiString &error);
      static bool ParseModifier_(const AnsiString &name, const AnsiString &value, SPFRecord &record, AnsiString &error);

      // One per shape of mechanism, because what each may carry after its name
      // differs: all takes nothing, include and exists must name a domain, a and
      // mx may name one and may carry either prefix length, ptr may name one and
      // carries none, and ip4 and ip6 must carry an address and at most the one
      // length that goes with their family.
      static bool ParseAll_(SPFMechanism::Qualifier qualifier, const AnsiString &remainder, SPFRecord &record, AnsiString &error);
      static bool ParseWithRequiredDomain_(SPFMechanism::Type type, SPFMechanism::Qualifier qualifier, const AnsiString &remainder, SPFRecord &record, AnsiString &error);
      static bool ParseAddressMechanism_(SPFMechanism::Type type, SPFMechanism::Qualifier qualifier, const AnsiString &remainder, SPFRecord &record, AnsiString &error);
      static bool ParsePtr_(SPFMechanism::Qualifier qualifier, const AnsiString &remainder, SPFRecord &record, AnsiString &error);
      static bool ParseIP_(SPFMechanism::Type type, SPFMechanism::Qualifier qualifier, const AnsiString &remainder, SPFRecord &record, AnsiString &error);

      std::vector<SPFMechanism> mechanisms_;

      bool has_redirect_;
      AnsiString redirect_domain_spec_;

      bool has_explanation_;
      AnsiString explanation_domain_spec_;
   };
}
