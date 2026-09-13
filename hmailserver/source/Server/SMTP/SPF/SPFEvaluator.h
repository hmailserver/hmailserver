// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

#include "SPFAddress.h"
#include "SPFResult.h"

namespace HM
{
   class SPFDnsLookup;
   class SPFMacroExpander;
   class SPFMechanism;
   class SPFRecord;

   // check_host() of RFC 7208 section 4: whether a domain authorizes a client to
   // send mail on its behalf.
   //
   // One of these per check. It finds the domain's record through
   // SPFRecordLocator, expands the terms through SPFMacroExpander, evaluates the
   // directives in the order they were written, and keeps the count that section
   // 4.6.4 caps.
   //
   // Every DNS query the check makes goes through one counter, the macro
   // expander's queries included, because the limits of section 4.6.4 are per
   // check rather than per record: an include whose target spends the budget
   // leaves none for the record which included it.
   class SPFEvaluator
   {
   public:

      explicit SPFEvaluator(std::shared_ptr<SPFDnsLookup> lookup);
      ~SPFEvaluator();

      // The name of the host doing the checking, for the r macro of RFC 7208
      // section 7.2, and the time of the check, for the t macro. Only the text an
      // exp modifier points at may use either.
      void SetReceivingHost(const AnsiString &name);
      void SetTimestamp(__int64 secondsSinceEpoch);

      // Evaluates domain for the client address, the sender and the HELO
      // argument, which are the four arguments of section 4.1.
      //
      // The explanation is the text of the record's exp modifier, expanded, and
      // is set only for a Fail which a record explained. Empty otherwise -
      // including where the record named an explanation that could not be read,
      // which section 6.2 leaves the receiver to answer with its own words.
      SPFResult Check(const SPFAddress &clientAddress,
                      const AnsiString &domain,
                      const AnsiString &sender,
                      const AnsiString &heloHost,
                      AnsiString &explanation);

      // The limits of section 4.6.4. Public because a test should be able to say
      // what it is testing rather than repeat the number.
      //
      // The first counts terms that query DNS - include, a, mx, ptr and exists,
      // and the redirect modifier - and not the queries themselves: one mx
      // mechanism is one term however many hosts it resolves.
      static const int MaximumTerms = 10;

      // Terms whose queries answered nothing at all. Section 4.6.4 limits the
      // number of such terms, not the number of such queries: one mx over five
      // exchangers with no address of the client's family is one of these, not
      // five.
      static const int MaximumVoidTerms = 2;

      // Section 4.6.4 caps the names one mx mechanism may resolve. Past it the
      // check is a permerror rather than a matter of resolving the first ten -
      // which is what tells it apart from ptr, where section 5.5 has the
      // eleventh name simply not looked at.
      static const int MaximumMxNames = 10;

      // How many of the ten terms the last check used, so that a test can say a
      // record cost what it should.
      int GetTermCount() const { return terms_; }

   private:

      // What evaluating one directive came to.
      enum class Match
      {
         // The mechanism matched. Its qualifier is the result of the check.
         Yes,

         // It did not. Section 4.6.2 moves on to the next directive.
         No,

         // The check is over before the record ran out: a lookup failed, or a
         // limit was passed. error_result_ says which.
         Error
      };

      // explanation is null where the caller will not use one - which an include
      // will not, section 6.2, so fetching it would be a query spent on an answer
      // thrown away.
      SPFResult CheckDomain_(const AnsiString &domain, int depth, AnsiString *explanation);

      Match EvaluateMechanism_(const SPFMechanism &mechanism, const AnsiString &domain, int depth);

      // The mechanisms that query DNS, each handed a target name already expanded
      // and already counted against the limits by EvaluateMechanism_.
      Match MatchA_(const SPFMechanism &mechanism, const AnsiString &targetName);
      Match MatchMX_(const SPFMechanism &mechanism, const AnsiString &targetName);
      Match MatchPTR_(const AnsiString &targetName);
      Match MatchExists_(const AnsiString &targetName);
      Match MatchInclude_(const AnsiString &targetName, int depth);

      Match MatchAddressLiteral_(const SPFMechanism &mechanism);

      // What resolving a mechanism's <target-name> came to, section 4.8.
      enum class TargetName
      {
         Resolved,

         // The expansion produced something no query can be built from. Section
         // 7.1 does not re-parse an expansion, so this is a name that does not
         // exist rather than an error: the mechanism does not match.
         Unusable,

         // The domain-spec itself does not expand, which section 4.6 makes a
         // permerror. The parser has seen every domain-spec a record carries, so
         // nothing reaches this.
         SyntaxError
      };

      TargetName ResolveTargetName_(const SPFMechanism &mechanism, const AnsiString &domain, AnsiString &targetName);

      // The addresses of a host in the client's own family, sections 5.3 and
      // 5.4, and whether any of them is the client within the mechanism's prefix
      // length. False from the first where the lookup could not be answered.
      bool TryLookupAddresses_(const AnsiString &host, std::vector<AnsiString> &addresses);
      bool MatchesAnyAddress_(const std::vector<AnsiString> &addresses, int prefixLength) const;

      // The prefix length a mechanism carries for the family the client
      // connected over. A mechanism which wrote neither gets the whole address.
      int PrefixLengthFor_(const SPFMechanism &mechanism) const;

      // Section 6.2: the text the record's exp modifier points at, expanded.
      // Empty where the record named none, or where what it named could not be
      // read - neither of which is an error.
      AnsiString ExplanationFor_(const SPFRecord &record, const AnsiString &domain);

      // Counts one term against the limit of section 4.6.4. False once the
      // budget is spent, which makes the check a permerror.
      bool CountTerm_();

      // Counts one void term if the queries made since voidQueriesBefore
      // answered nothing. Called once per term, which is what keeps a mechanism
      // that resolves many names from spending the budget several times over.
      void CountVoidTerm_(int voidQueriesBefore);

      bool GetHasTooManyVoidTerms_() const;

      Match Fail_(SPFResult result);

      // Counts the queries an evaluation makes. Defined in the implementation:
      // nothing outside it needs to know that the counting is done by wrapping
      // the resolver the caller supplied.
      class CountingLookup;

      std::shared_ptr<SPFDnsLookup> lookup_;
      std::shared_ptr<CountingLookup> counting_lookup_;
      std::shared_ptr<SPFMacroExpander> expander_;

      SPFAddress client_address_;

      AnsiString receiving_host_;
      __int64 timestamp_;

      int terms_;
      int void_terms_;

      // Set whenever a mechanism returns Match::Error, and read by the loop that
      // gave up because of it.
      SPFResult error_result_;
   };
}
