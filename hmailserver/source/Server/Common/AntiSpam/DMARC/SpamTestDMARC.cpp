// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "stdafx.h"

#include "SpamTestDMARC.h"

#include "DMARCEvaluator.h"
#include "DMARCPolicyLocator.h"
#include "DMARCTxtLookup.h"

#include "../AntiSpamConfiguration.h"
#include "../SenderAuthentication.h"
#include "../SpamTestData.h"
#include "../SpamTestResult.h"
#include "../ARC/ARCAuthenticationResults.h"
#include "../../BO/ARCTrustedSealers.h"
#include "../DKIM/DKIM.h"

#include "../../BO/MessageData.h"
#include "../../Util/Parsing/AddresslistParser.h"

#include "../../../SMTP/SPF/SPF.h"

#include <random>

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   String
   SpamTestDMARC::GetName() const
   {
      return "SpamTestDMARC";
   }

   bool
   SpamTestDMARC::GetIsEnabled()
   {
      return Configuration::Instance()->GetAntiSpamConfiguration().GetDMARCEnabled();
   }

   std::set<std::shared_ptr<SpamTestResult> >
   SpamTestDMARC::RunTest(std::shared_ptr<SpamTestData> pTestData)
   {
      std::set<std::shared_ptr<SpamTestResult> > setSpamTestResults;

      String headerFromDomain = GetHeaderFromDomain_(pTestData);

      if (headerFromDomain.IsEmpty())
      {
         // Without exactly one From address, the message can't be evaluated.
         return setSpamTestResults;
      }

      DMARCRecord record;
      String policyDomain;

      DMARCPolicyLocator locator(std::shared_ptr<DMARCTxtLookup>(new DMARCDnsTxtLookup));
      DMARCPolicyLocator::Result locateResult = locator.Locate(headerFromDomain, record, policyDomain);

      if (locateResult != DMARCPolicyLocator::Result::Found)
      {
         // The domain either publishes no policy, or the lookup failed. Neither is
         // something we should punish the message for.
         return setSpamTestResults;
      }

      std::shared_ptr<SenderAuthentication> senderAuthentication = pTestData->GetSenderAuthentication();

      // No-ops if the SPF and DKIM tests have already run for this message.
      senderAuthentication->EvaluateSPF(pTestData);
      senderAuthentication->EvaluateDKIM(pTestData);

      if (IsAuthenticated_(senderAuthentication, record, headerFromDomain))
      {
         senderAuthentication->SetDMARCResult(SenderAuthentication::DMARCResult::Pass, headerFromDomain);

         std::shared_ptr<SpamTestResult> pResult = std::shared_ptr<SpamTestResult>(new SpamTestResult(GetName(), SpamTestResult::Pass, 0, ""));
         setSpamTestResults.insert(pResult);

         return setSpamTestResults;
      }

      senderAuthentication->SetDMARCResult(SenderAuthentication::DMARCResult::Fail, headerFromDomain);

      String trustedSealer;
      if (IsVouchedForByTrustedSealer_(pTestData, record, headerFromDomain, trustedSealer))
      {
         // The message failed since it was forwarded. A sealer we trust saw it pass
         // before that, so the failure is neither scored nor acted on.
         senderAuthentication->SetDMARCOverriddenBySealer(trustedSealer);

         LOG_DEBUG("DMARC: The failure was overridden since the trusted ARC sealer " + trustedSealer + " saw the message pass.");

         std::shared_ptr<SpamTestResult> pResult = std::shared_ptr<SpamTestResult>(new SpamTestResult(GetName(), SpamTestResult::Pass, 0, ""));
         setSpamTestResults.insert(pResult);

         return setSpamTestResults;
      }

      String message;
      message.Format(_T("Rejected by DMARC. (%s)"), headerFromDomain.c_str());

      AntiSpamConfiguration &config = Configuration::Instance()->GetAntiSpamConfiguration();

      std::shared_ptr<SpamTestResult> pResult = std::shared_ptr<SpamTestResult>(new SpamTestResult(GetName(), SpamTestResult::Fail, config.GetDMARCFailureScore(), message));

      if (config.GetDMARCHonorPolicy())
      {
         DMARCRecord::Policy policy = DMARCEvaluator::GetApplicablePolicy(record, headerFromDomain, policyDomain);

         switch (GetPolicyToApply_(record, policy))
         {
         case DMARCRecord::Policy::Reject:
            pResult->SetRejectMessage(true);
            break;
         case DMARCRecord::Policy::Quarantine:
            pResult->SetMarkAsSpam(true);
            break;
         }
      }

      setSpamTestResults.insert(pResult);

      return setSpamTestResults;
   }

   DMARCRecord::Policy
   SpamTestDMARC::GetPolicyToApply_(const DMARCRecord &record, DMARCRecord::Policy policy)
   {
      int percent = record.GetPercent();

      if (percent >= 100)
         return policy;

      // The pct tag lets a domain owner roll a policy out gradually, by having
      // receivers apply it to a random sample of that size of their messages.

      // Thread local since the generator isn't thread safe, and messages are
      // processed on several threads at the same time.
      thread_local std::mt19937 generator(std::random_device{}());
      std::uniform_int_distribution<int> distribution(0, 99);

      if (distribution(generator) < percent)
         return policy;

      // Outside the sampled percentage the policy is applied one step down.
      return DegradePolicy_(policy);
   }

   DMARCRecord::Policy
   SpamTestDMARC::DegradePolicy_(DMARCRecord::Policy policy)
   {
      switch (policy)
      {
      case DMARCRecord::Policy::Reject:
         return DMARCRecord::Policy::Quarantine;
      case DMARCRecord::Policy::Quarantine:
         return DMARCRecord::Policy::None;
      }

      return DMARCRecord::Policy::None;
   }

   bool
   SpamTestDMARC::IsVouchedForByTrustedSealer_(std::shared_ptr<SpamTestData> pTestData, const DMARCRecord &record,
                                               const String &headerFromDomain, String &sealerDomain)
   {
      AntiSpamConfiguration &config = Configuration::Instance()->GetAntiSpamConfiguration();

      if (!config.GetARCEnabled())
         return false;

      std::shared_ptr<SenderAuthentication> senderAuthentication = pTestData->GetSenderAuthentication();

      if (senderAuthentication->EvaluateARC(pTestData) != ARCVerifier::Result::Pass)
         return false;

      // Only the last sealer counts: it is the hop that handed the message to us.
      String sealer = senderAuthentication->GetARCSealerDomain();

      if (!config.GetARCTrustedSealers()->IsTrusted(sealer))
         return false;

      // The sealer must also have seen the original message authenticate.
      if (!ARCAuthenticationResults::ShowsAlignedPass(senderAuthentication->GetARCAuthenticationResults(), headerFromDomain,
                                                      record.GetSPFAlignment(), record.GetDKIMAlignment()))
      {
         LOG_DEBUG("DMARC: The trusted ARC sealer " + sealer + " did not record an aligned pass.");
         return false;
      }

      sealerDomain = sealer;
      return true;
   }

   String
   SpamTestDMARC::GetHeaderFromDomain_(std::shared_ptr<SpamTestData> pTestData)
   {
      std::shared_ptr<MessageData> pMessageData = pTestData->GetMessageData();

      if (!pMessageData)
         return "";

      AddresslistParser parser;
      std::vector<std::shared_ptr<Address> > addresses = parser.ParseList(pMessageData->GetFrom());

      // RFC 7489: a message with anything but a single From address isn't evaluable.
      if (addresses.size() != 1)
         return "";

      String domain = addresses[0]->sDomainName;
      domain.ToLower();

      return domain;
   }

   bool
   SpamTestDMARC::IsAuthenticated_(std::shared_ptr<SenderAuthentication> senderAuthentication, const DMARCRecord &record, const String &headerFromDomain)
   {
      if (senderAuthentication->GetSPFResult() == SPFResult::Pass &&
          DMARCEvaluator::IsAligned(senderAuthentication->GetSPFDomain(), headerFromDomain, record.GetSPFAlignment()))
      {
         return true;
      }

      for (auto signature : senderAuthentication->GetDKIMSignatures())
      {
         if (signature.second != DKIM::Pass)
            continue;

         if (DMARCEvaluator::IsAligned(String(signature.first), headerFromDomain, record.GetDKIMAlignment()))
            return true;
      }

      return false;
   }
}
