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

      // A query answered with no records at all. Section 4.6.4 limits these
      // separately, because a record can spend an evaluation's budget on names
      // which do not exist as easily as on names which do.
      static const int MaximumVoidLookups = 2;

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

      SPFResult CheckDomain_(const AnsiString &domain, int depth, AnsiString &explanation);

      Match EvaluateMechanism_(const SPFMechanism &mechanism, const AnsiString &domain, int depth);

      Match MatchAll_();
      Match MatchAddressLiteral_(const SPFMechanism &mechanism);
      Match MatchA_(const SPFMechanism &mechanism, const AnsiString &domain);
      Match MatchMX_(const SPFMechanism &mechanism, const AnsiString &domain);
      Match MatchPTR_(const SPFMechanism &mechanism, const AnsiString &domain);
      Match MatchExists_(const SPFMechanism &mechanism, const AnsiString &domain);
      Match MatchInclude_(const SPFMechanism &mechanism, const AnsiString &domain, int depth);

      // Section 4.8's <target-name>: the mechanism's domain-spec expanded, or
      // domain where the mechanism named none. False where the domain-spec would
      // not expand, which section 4.6 makes a permerror.
      bool TryGetTargetName_(const SPFMechanism &mechanism, const AnsiString &domain, AnsiString &targetName);

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

      // Set whenever a mechanism returns Match::Error, and read by the loop that
      // gave up because of it.
      SPFResult error_result_;
   };
}
