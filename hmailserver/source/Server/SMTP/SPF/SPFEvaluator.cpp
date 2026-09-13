// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "stdafx.h"

#include "SPFEvaluator.h"

#include "SPFDnsLookup.h"
#include "SPFMacroExpander.h"
#include "SPFRecord.h"
#include "SPFRecordLocator.h"
#include "SPFSyntax.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   namespace
   {
      // An include or a redirect moves the evaluation to another record, which may do the
      // same. The term limit of section 4.6.4 already bounds the depth, since every step
      // costs one; this is only so a change to the counting cannot overflow the stack.
      const int MAXIMUM_DEPTH = SPFEvaluator::MaximumTerms + 2;

      // Section 5.5: a validated name matches if it is the target name or sits
      // below it.
      bool IsAtOrBelow(const AnsiString &name, const AnsiString &targetName)
      {
         if (SPFSyntax::EqualsDnsName(name, targetName))
            return true;

         return SPFSyntax::EndsWithDnsName(name, AnsiString(".") + targetName);
      }

      // Section 4.6.2: the qualifier of the directive that matched is the result
      // of the check. "+" is the default, which is why a bare "all" passes.
      SPFResult ResultOf(SPFMechanism::Qualifier qualifier)
      {
         switch (qualifier)
         {
         case SPFMechanism::Qualifier::Pass:
            return SPFResult::Pass;

         case SPFMechanism::Qualifier::Fail:
            return SPFResult::Fail;

         case SPFMechanism::Qualifier::SoftFail:
            return SPFResult::SoftFail;

         case SPFMechanism::Qualifier::Neutral:
            break;
         }

         return SPFResult::Neutral;
      }
   }

   // Wraps the resolver an evaluation was given and counts what it is asked. Here
   // rather than in the evaluator's own methods because the macro expander queries DNS
   // too, and section 4.6.4 counts an evaluation's queries - see README.md.
   class SPFEvaluator::CountingLookup : public SPFDnsLookup
   {
   public:

      explicit CountingLookup(std::shared_ptr<SPFDnsLookup> lookup) :
         lookup_(lookup),
         void_lookups_(0)
      {

      }

      virtual bool GetTXTRecords(const AnsiString &domain, std::vector<AnsiString> &records)
      {
         return Count_(lookup_->GetTXTRecords(domain, records), records);
      }

      virtual bool GetARecords(const AnsiString &host, std::vector<AnsiString> &addresses)
      {
         return Count_(lookup_->GetARecords(host, addresses), addresses);
      }

      virtual bool GetAAAARecords(const AnsiString &host, std::vector<AnsiString> &addresses)
      {
         return Count_(lookup_->GetAAAARecords(host, addresses), addresses);
      }

      virtual bool GetMXRecords(const AnsiString &domain, std::vector<AnsiString> &hostNames)
      {
         return Count_(lookup_->GetMXRecords(domain, hostNames), hostNames);
      }

      virtual bool GetPTRRecords(const AnsiString &reverseName, std::vector<AnsiString> &hostNames)
      {
         return Count_(lookup_->GetPTRRecords(reverseName, hostNames), hostNames);
      }

      // How many queries have answered nothing. The evaluator turns this into
      // the count of *terms* that answered nothing, which is what section 4.6.4
      // limits.
      int GetVoidQueryCount() const { return void_lookups_; }

   private:

      bool Count_(bool answered, const std::vector<AnsiString> &values)
      {
         // A query which could not be answered at all is a temporary failure and
         // is the caller's to report; only an answer holding nothing is a void
         // lookup.
         if (answered && values.empty())
            void_lookups_++;

         return answered;
      }

      std::shared_ptr<SPFDnsLookup> lookup_;

      int void_lookups_;
   };

   SPFEvaluator::SPFEvaluator(std::shared_ptr<SPFDnsLookup> lookup) :
      lookup_(lookup),
      receiving_host_("unknown"),
      timestamp_(0),
      terms_(0),
      void_terms_(0),
      error_result_(SPFResult::PermError)
   {

   }

   SPFEvaluator::~SPFEvaluator()
   {

   }

   void
   SPFEvaluator::SetReceivingHost(const AnsiString &name)
   {
      receiving_host_ = name;
   }

   void
   SPFEvaluator::SetTimestamp(__int64 secondsSinceEpoch)
   {
      timestamp_ = secondsSinceEpoch;
   }

   SPFResult
   SPFEvaluator::Check(const SPFAddress &clientAddress, const AnsiString &domain,
                       const AnsiString &sender, const AnsiString &heloHost,
                       AnsiString &explanation)
   {
      explanation = "";

      terms_ = 0;
      void_terms_ = 0;
      error_result_ = SPFResult::PermError;
      client_address_ = clientAddress;

      counting_lookup_ = std::make_shared<CountingLookup>(lookup_);

      expander_ = std::make_shared<SPFMacroExpander>(counting_lookup_, clientAddress, sender, heloHost);
      expander_->SetReceivingHost(receiving_host_);
      expander_->SetTimestamp(timestamp_);

      return CheckDomain_(domain, 0, &explanation);
   }

   SPFResult
   SPFEvaluator::CheckDomain_(const AnsiString &domain, int depth, AnsiString *explanation)
   {
      if (explanation)
         *explanation = "";

      if (depth > MAXIMUM_DEPTH)
         return SPFResult::PermError;

      SPFRecordLocator locator(counting_lookup_);

      SPFRecord record;
      AnsiString error;

      // The record lookup is the DNS query of whichever term sent the evaluation
      // here - an include, a redirect, or the check itself - so a void answer here
      // is that term's one void.
      int voidQueriesBefore = counting_lookup_->GetVoidQueryCount();

      SPFRecordLocator::Result located = locator.Locate(domain, record, error);

      CountVoidTerm_(voidQueriesBefore);

      switch (located)
      {
      case SPFRecordLocator::Result::NoRecord:
         return SPFResult::None;

      case SPFRecordLocator::Result::TemporaryError:
         return SPFResult::TempError;

      case SPFRecordLocator::Result::Ambiguous:
      case SPFRecordLocator::Result::SyntaxError:
         return SPFResult::PermError;

      case SPFRecordLocator::Result::Found:
         break;
      }

      if (GetHasTooManyVoidTerms_())
         return SPFResult::PermError;

      // Section 4.6.2: the directives are evaluated in the order they were
      // written, and the first one that matches decides.
      const std::vector<SPFMechanism> &mechanisms = record.GetMechanisms();

      for (size_t i = 0; i < mechanisms.size(); i++)
      {
         Match match = EvaluateMechanism_(mechanisms[i], domain, depth);

         // Checked after every term rather than where a query is made, so that
         // one place decides it. A term which matched on its way past the limit
         // does not get to keep the match: section 4.6.4 ends the check.
         if (GetHasTooManyVoidTerms_())
            return SPFResult::PermError;

         if (match == Match::Error)
            return error_result_;

         if (match == Match::No)
            continue;

         SPFResult result = ResultOf(mechanisms[i].GetQualifier());

         // Section 6.2: only a fail is explained, and only by the record that
         // produced it - and only where the caller has somewhere to put it.
         if (result == SPFResult::Fail && explanation)
            *explanation = ExplanationFor_(record, domain);

         return result;
      }

      // Section 6.1: where nothing matched, a redirect hands the whole check to
      // another domain - its result, and its explanation. This record's exp
      // modifier is not used, which is what redirect-cancels-exp is about.
      if (record.GetHasRedirect())
      {
         SPFMechanism redirect(SPFMechanism::Type::Include, SPFMechanism::Qualifier::Pass);

         redirect.SetDomainSpec(record.GetRedirectDomainSpec());

         AnsiString targetName;

         switch (ResolveTargetName_(redirect, domain, targetName))
         {
         case TargetName::SyntaxError:
            return SPFResult::PermError;

         case TargetName::Unusable:
            // A redirect naming something no query can be built from names a
            // domain that publishes no record, which section 6.1 makes a
            // permerror the same way it does for one that exists and has none.
            return SPFResult::PermError;

         case TargetName::Resolved:
            break;
         }

         if (!CountTerm_())
            return SPFResult::PermError;

         SPFResult result = CheckDomain_(targetName, depth + 1, explanation);

         // Section 6.1: a redirect naming a domain which publishes no record is
         // a permerror rather than the "none" that domain would produce on its
         // own. The record said where to look and was wrong.
         if (result == SPFResult::None)
         {
            if (explanation)
               *explanation = "";

            return SPFResult::PermError;
         }

         return result;
      }

      // Section 4.7: a record which neither matched nor redirected says nothing
      // about this client.
      return SPFResult::Neutral;
   }

   SPFEvaluator::Match
   SPFEvaluator::EvaluateMechanism_(const SPFMechanism &mechanism, const AnsiString &domain, int depth)
   {
      switch (mechanism.GetType())
      {
      case SPFMechanism::Type::All:
         // Section 5.1: it always matches, which is what puts a result on every
         // client a record did not mention.
         return Match::Yes;

      case SPFMechanism::Type::IP4:
      case SPFMechanism::Type::IP6:
         return MatchAddressLiteral_(mechanism);

      default:
         break;
      }

      // What is left is the five terms that query DNS. Resolving the target name
      // and counting the term are the same for all of them, and section 4.6.4
      // counts a term once however many queries it goes on to make.
      AnsiString targetName;

      switch (ResolveTargetName_(mechanism, domain, targetName))
      {
      case TargetName::SyntaxError:
         return Fail_(SPFResult::PermError);

      case TargetName::Unusable:
         return Match::No;

      case TargetName::Resolved:
         break;
      }

      if (!CountTerm_())
         return Fail_(SPFResult::PermError);

      int voidQueriesBefore = counting_lookup_->GetVoidQueryCount();

      Match match = Match::No;

      switch (mechanism.GetType())
      {
      case SPFMechanism::Type::A:
         match = MatchA_(mechanism, targetName);
         break;

      case SPFMechanism::Type::MX:
         match = MatchMX_(mechanism, targetName);
         break;

      case SPFMechanism::Type::PTR:
         match = MatchPTR_(targetName);
         break;

      case SPFMechanism::Type::Exists:
         match = MatchExists_(targetName);
         break;

      case SPFMechanism::Type::Include:
         // An include's own void is the record lookup its recursion makes, which
         // CheckDomain_ counts there; counting again here would charge one term
         // twice.
         return MatchInclude_(targetName, depth);

      default:
         break;
      }

      CountVoidTerm_(voidQueriesBefore);

      return match;
   }

   SPFEvaluator::Match
   SPFEvaluator::MatchAddressLiteral_(const SPFMechanism &mechanism)
   {
      SPFAddress network;

      if (!SPFAddress::TryParse(mechanism.GetAddress(), network))
      {
         // The parser accepted the literal against the same grammar this reads,
         // so there is nothing here it could reject.
         return Fail_(SPFResult::PermError);
      }

      // Sections 5.6 and 5.7: an ip4 mechanism is about IPv4 clients and an ip6
      // mechanism about IPv6 ones. MatchesPrefix already refuses to compare the
      // families, so the mechanism's own length is all that is needed.
      int prefixLength = (mechanism.GetType() == SPFMechanism::Type::IP4)
                            ? mechanism.GetIP4PrefixLength()
                            : mechanism.GetIP6PrefixLength();

      return client_address_.MatchesPrefix(network, prefixLength) ? Match::Yes : Match::No;
   }

   SPFEvaluator::Match
   SPFEvaluator::MatchA_(const SPFMechanism &mechanism, const AnsiString &targetName)
   {
      std::vector<AnsiString> addresses;

      if (!TryLookupAddresses_(targetName, addresses))
         return Fail_(SPFResult::TempError);

      return MatchesAnyAddress_(addresses, PrefixLengthFor_(mechanism)) ? Match::Yes : Match::No;
   }

   SPFEvaluator::Match
   SPFEvaluator::MatchMX_(const SPFMechanism &mechanism, const AnsiString &targetName)
   {
      std::vector<AnsiString> hostNames;

      if (!counting_lookup_->GetMXRecords(targetName, hostNames))
         return Fail_(SPFResult::TempError);

      if ((int) hostNames.size() > MaximumMxNames)
         return Fail_(SPFResult::PermError);

      int prefixLength = PrefixLengthFor_(mechanism);

      for (size_t i = 0; i < hostNames.size(); i++)
      {
         std::vector<AnsiString> addresses;

         if (!TryLookupAddresses_(hostNames[i], addresses))
            return Fail_(SPFResult::TempError);

         if (MatchesAnyAddress_(addresses, prefixLength))
            return Match::Yes;
      }

      return Match::No;
   }

   SPFEvaluator::Match
   SPFEvaluator::MatchPTR_(const AnsiString &targetName)
   {
      // Section 5.5: the names the client's reverse mapping gives, kept only where they
      // resolve back to the client. The expander does the work, since the p macro of
      // section 7.3 needs the same list. A failed lookup leaves it empty, not an error.
      const std::vector<AnsiString> &names = expander_->GetValidatedNames();

      for (size_t i = 0; i < names.size(); i++)
      {
         if (IsAtOrBelow(names[i], targetName))
            return Match::Yes;
      }

      return Match::No;
   }

   SPFEvaluator::Match
   SPFEvaluator::MatchExists_(const AnsiString &targetName)
   {
      // Section 5.7: the query is for an A record whatever family the client
      // connected over, because what is being asked is only whether the name
      // exists. An IPv6 client does not make this an AAAA query.
      std::vector<AnsiString> addresses;

      if (!counting_lookup_->GetARecords(targetName, addresses))
         return Fail_(SPFResult::TempError);

      return addresses.empty() ? Match::No : Match::Yes;
   }

   SPFEvaluator::Match
   SPFEvaluator::MatchInclude_(const AnsiString &targetName, int depth)
   {
      // Section 6.2: the included record's own exp modifier is not used, so it is
      // not fetched either - a query spent on an answer this would throw away,
      // and one that would spend the void budget of the evaluation that asked.
      SPFResult result = CheckDomain_(targetName, depth + 1, 0);

      // Section 5.2's table. An include asks a question of another domain and
      // only "yes" counts: the included record's fail is not this record's fail,
      // it just means the client is not among the ones that domain listed.
      switch (result)
      {
      case SPFResult::Pass:
         return Match::Yes;

      case SPFResult::Fail:
      case SPFResult::SoftFail:
      case SPFResult::Neutral:
         return Match::No;

      case SPFResult::TempError:
         return Fail_(SPFResult::TempError);

      case SPFResult::None:
      case SPFResult::PermError:
         // A domain which publishes no record cannot be included. The record
         // named it, so the mistake is the record's.
         return Fail_(SPFResult::PermError);
      }

      return Match::No;
   }

   SPFEvaluator::TargetName
   SPFEvaluator::ResolveTargetName_(const SPFMechanism &mechanism, const AnsiString &domain, AnsiString &targetName)
   {
      if (!mechanism.GetHasDomainSpec())
      {
         // Sections 5.3, 5.4 and 5.5: a, mx and ptr check the domain being
         // evaluated where they name none of their own.
         targetName = domain;
      }
      else if (!expander_->ExpandDomainSpec(mechanism.GetDomainSpec(), domain, targetName))
      {
         return TargetName::SyntaxError;
      }

      // Section 7.1 does not re-parse an expansion, so what came out is checked only for
      // whether a query can be built from it at all; one that cannot is a name that does
      // not exist. Without this an empty expansion reaches the resolver as a failure.
      if (!SPFSyntax::IsValidDomainName(targetName))
         return TargetName::Unusable;

      return TargetName::Resolved;
   }

   bool
   SPFEvaluator::TryLookupAddresses_(const AnsiString &host, std::vector<AnsiString> &addresses)
   {
      // Sections 5.3 and 5.4: which records are asked for is decided by the
      // family the client connected over, and not by what the host publishes.
      if (client_address_.GetFamily() == SPFAddress::Family::IP6)
         return counting_lookup_->GetAAAARecords(host, addresses);

      return counting_lookup_->GetARecords(host, addresses);
   }

   bool
   SPFEvaluator::MatchesAnyAddress_(const std::vector<AnsiString> &addresses, int prefixLength) const
   {
      for (size_t i = 0; i < addresses.size(); i++)
      {
         SPFAddress address;

         if (!SPFAddress::TryParse(addresses[i], address))
         {
            // Something the resolver returned which is not an address. Nothing
            // can be matched against it, and section 5.3 gives no way to report
            // it, so it is simply not a match.
            continue;
         }

         if (client_address_.MatchesPrefix(address, prefixLength))
            return true;
      }

      return false;
   }

   int
   SPFEvaluator::PrefixLengthFor_(const SPFMechanism &mechanism) const
   {
      // Section 5.3: a and mx carry a length for each family, and the one that
      // applies is the one for the family the client connected over.
      if (client_address_.GetFamily() == SPFAddress::Family::IP6)
         return mechanism.GetIP6PrefixLength();

      return mechanism.GetIP4PrefixLength();
   }

   AnsiString
   SPFEvaluator::ExplanationFor_(const SPFRecord &record, const AnsiString &domain)
   {
      if (!record.GetHasExplanation())
         return "";

      AnsiString targetName;

      if (!expander_->ExpandDomainSpec(record.GetExplanationDomainSpec(), domain, targetName))
         return "";

      std::vector<AnsiString> records;

      // Section 4.6.4 leaves the exp modifier out of the term limit, and this query is
      // outside every void-term window too: the result of the check is already decided by
      // the time an explanation is wanted.
      if (!counting_lookup_->GetTXTRecords(targetName, records))
         return "";

      // Section 6.2: no record, more than one, a lookup that failed, or an
      // explanation which does not parse - each of them leaves the receiver to
      // explain the fail in its own words, and none of them is an error.
      if (records.size() != 1)
         return "";

      AnsiString explanation;

      if (!expander_->ExpandExplanation(records[0], domain, explanation))
         return "";

      return explanation;
   }

   bool
   SPFEvaluator::CountTerm_()
   {
      terms_++;

      return terms_ <= MaximumTerms;
   }

   void
   SPFEvaluator::CountVoidTerm_(int voidQueriesBefore)
   {
      // Section 4.6.4 limits the terms whose queries answer nothing, not the
      // queries. However many names one mx resolved, or one ptr validated, the
      // term spends at most one of the two.
      if (counting_lookup_->GetVoidQueryCount() > voidQueriesBefore)
         void_terms_++;
   }

   bool
   SPFEvaluator::GetHasTooManyVoidTerms_() const
   {
      return void_terms_ > MaximumVoidTerms;
   }

   SPFEvaluator::Match
   SPFEvaluator::Fail_(SPFResult result)
   {
      error_result_ = result;

      return Match::Error;
   }
}
