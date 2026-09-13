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

   // check_host() of RFC 7208 section 4: whether a domain authorizes a client.
   // One per check, and one counter sees every query it makes including the macro
   // expander's, because section 4.6.4's limits are per check - see README.md.
   class SPFEvaluator
   {
   public:

      explicit SPFEvaluator(std::shared_ptr<SPFDnsLookup> lookup);
      ~SPFEvaluator();

      // The r and t macros of section 7.2, which only the text an exp modifier points
      // at may use.
      void SetReceivingHost(const AnsiString &name);
      void SetTimestamp(__int64 secondsSinceEpoch);

      // Evaluates domain for the four arguments of section 4.1. The explanation is set
      // only for a Fail which the record explained; empty otherwise, including where
      // what it named could not be read, which section 6.2 leaves to the receiver.
      SPFResult Check(const SPFAddress &clientAddress,
                      const AnsiString &domain,
                      const AnsiString &sender,
                      const AnsiString &heloHost,
                      AnsiString &explanation);

      // Section 4.6.4's limits. Public so a test can name what it tests. This one
      // counts terms that query DNS, not the queries: one mx is one term however
      // many hosts it resolves.
      static const int MaximumTerms = 10;

      // Terms whose queries answered nothing. Section 4.6.4 limits those terms, not
      // those queries: one mx over five exchangers with no address of the client's
      // family is one of these, not five.
      static const int MaximumVoidTerms = 2;

      // Section 4.6.4 caps the names one mx may resolve. Past it the check is a
      // permerror, which is what tells it apart from ptr - section 5.5 has the
      // eleventh name there simply not looked at.
      static const int MaximumMxNames = 10;

      // How many terms the last check used, so a test can say a record cost what it
      // should.
      int GetTermCount() const { return terms_; }

   private:

      // What evaluating one directive came to.
      enum class Match
      {
         // The mechanism matched. Its qualifier is the result of the check.
         Yes,

         // It did not. Section 4.6.2 moves on to the next directive.
         No,

         // The check is over: a lookup failed or a limit was passed. error_result_
         // says which.
         Error
      };

      // explanation is null where the caller will not use one, which an include will
      // not - section 6.2 - so fetching it would spend a query on an answer thrown
      // away.
      SPFResult CheckDomain_(const AnsiString &domain, int depth, AnsiString *explanation);

      Match EvaluateMechanism_(const SPFMechanism &mechanism, const AnsiString &domain, int depth);

      // The mechanisms that query DNS, each handed a target name already expanded and
      // counted by EvaluateMechanism_.
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

         // Nothing a query can be built from. Section 7.1 does not re-parse an
         // expansion, so this is a name that does not exist: no match, not an error.
         Unusable,

         // The domain-spec does not expand, which section 4.6 makes a permerror. The
         // parser has seen every one a record carries, so nothing reaches this.
         SyntaxError
      };

      TargetName ResolveTargetName_(const SPFMechanism &mechanism, const AnsiString &domain, AnsiString &targetName);

      // The addresses of a host in the client's own family, sections 5.3 and 5.4, and
      // whether any is the client within the prefix length.
      bool TryLookupAddresses_(const AnsiString &host, std::vector<AnsiString> &addresses);
      bool MatchesAnyAddress_(const std::vector<AnsiString> &addresses, int prefixLength) const;

      // The prefix length for the family the client connected over.
      int PrefixLengthFor_(const SPFMechanism &mechanism) const;

      // Section 6.2: the exp modifier's text, expanded. Empty where the record named
      // none or what it named could not be read - neither is an error.
      AnsiString ExplanationFor_(const SPFRecord &record, const AnsiString &domain);

      // Counts one term, section 4.6.4. False once the budget is spent.
      bool CountTerm_();

      // Counts one void term if anything queried since voidQueriesBefore answered
      // nothing. Once per term, so a mechanism resolving many names spends one.
      void CountVoidTerm_(int voidQueriesBefore);

      bool GetHasTooManyVoidTerms_() const;

      Match Fail_(SPFResult result);

      // Counts the queries an evaluation makes. Defined in the implementation.
      class CountingLookup;

      std::shared_ptr<SPFDnsLookup> lookup_;
      std::shared_ptr<CountingLookup> counting_lookup_;
      std::shared_ptr<SPFMacroExpander> expander_;

      SPFAddress client_address_;

      AnsiString receiving_host_;
      __int64 timestamp_;

      int terms_;
      int void_terms_;

      // Set when a mechanism returns Match::Error, read by the loop that gave up.
      SPFResult error_result_;
   };
}
