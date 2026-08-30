// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "StdAfx.h"

#include "SpamTestDMARC.h"

#include "DMARCEvaluator.h"
#include "DMARCPolicyLocator.h"
#include "DMARCTxtLookup.h"

#include "../AntiSpamConfiguration.h"
#include "../AuthenticationResult.h"
#include "../SpamTestData.h"
#include "../SpamTestResult.h"
#include "../DKIM/DKIM.h"

#include "../../BO/MessageData.h"
#include "../../Persistence/PersistentMessage.h"
#include "../../Util/Parsing/AddresslistParser.h"

#include "../../../SMTP/SPF/SPF.h"

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

      std::shared_ptr<AuthenticationResult> authenticationResult = pTestData->GetAuthenticationResult();

      if (!authenticationResult)
         authenticationResult = std::shared_ptr<AuthenticationResult>(new AuthenticationResult);

      EnsureSPFResult_(pTestData, authenticationResult);
      EnsureDKIMResults_(pTestData, authenticationResult);

      if (IsAuthenticated_(authenticationResult, record, headerFromDomain))
      {
         std::shared_ptr<SpamTestResult> pResult = std::shared_ptr<SpamTestResult>(new SpamTestResult(GetName(), SpamTestResult::Pass, 0, ""));
         setSpamTestResults.insert(pResult);

         return setSpamTestResults;
      }

      String message;
      message.Format(_T("Rejected by DMARC. (%s)"), headerFromDomain.c_str());

      int score = Configuration::Instance()->GetAntiSpamConfiguration().GetDMARCFailureScore();

      std::shared_ptr<SpamTestResult> pResult = std::shared_ptr<SpamTestResult>(new SpamTestResult(GetName(), SpamTestResult::Fail, score, message));
      setSpamTestResults.insert(pResult);

      return setSpamTestResults;
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

   void
   SpamTestDMARC::EnsureSPFResult_(std::shared_ptr<SpamTestData> pTestData, std::shared_ptr<AuthenticationResult> authenticationResult)
   {
      if (authenticationResult->GetSPFChecked())
         return;

      const IPAddress &originatingAddress = pTestData->GetOriginatingIP();

      if (originatingAddress.IsAny())
         return;

      String explanation;
      SPF::Result result = SPF::Instance()->Test(originatingAddress.ToString(), pTestData->GetEnvelopeFrom(), pTestData->GetHeloHost(), explanation);

      // With a null sender, SPF authenticates the HELO identity instead.
      String domain = StringParser::ExtractDomain(pTestData->GetEnvelopeFrom());
      if (domain.IsEmpty())
         domain = pTestData->GetHeloHost();

      authenticationResult->SetSPFResult(result, domain);
   }

   void
   SpamTestDMARC::EnsureDKIMResults_(std::shared_ptr<SpamTestData> pTestData, std::shared_ptr<AuthenticationResult> authenticationResult)
   {
      if (authenticationResult->GetDKIMChecked())
         return;

      std::shared_ptr<MessageData> pMessageData = pTestData->GetMessageData();

      if (!pMessageData)
         return;

      const String fileName = PersistentMessage::GetFileName(pMessageData->GetMessage());

      DKIM dkim;
      std::vector<std::pair<AnsiString, DKIM::Result> > signatureResults;
      dkim.Verify(fileName, signatureResults);

      authenticationResult->SetDKIMResults(signatureResults);
   }

   bool
   SpamTestDMARC::IsAuthenticated_(std::shared_ptr<AuthenticationResult> authenticationResult, const DMARCRecord &record, const String &headerFromDomain)
   {
      if (authenticationResult->GetSPFResult() == SPF::Pass &&
          DMARCEvaluator::IsAligned(authenticationResult->GetSPFDomain(), headerFromDomain, record.GetSPFAlignment()))
      {
         return true;
      }

      for (auto signature : authenticationResult->GetDKIMSignatures())
      {
         if (signature.second != DKIM::Pass)
            continue;

         if (DMARCEvaluator::IsAligned(String(signature.first), headerFromDomain, record.GetDKIMAlignment()))
            return true;
      }

      return false;
   }
}
