// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "stdafx.h"
#include "SenderAuthentication.h"

#include "SpamTestData.h"

#include "../BO/MessageData.h"
#include "../Persistence/PersistentMessage.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   SenderAuthentication::SenderAuthentication() :
      spf_checked_(false),

      // There is no result for "not checked yet", and this is not one: a neutral
      // is what a record that mentions nobody produces, which is a thing SPF can
      // actually answer. spf_checked_ is what says whether a check was made, and
      // every caller has to consult it - the Authentication-Results header does,
      // and the spam tests only ever compare against Pass or Fail, so neither
      // this value nor a None in its place is observable. Anything that starts
      // reading the result without the flag needs a real sentinel first.
      spf_result_(SPFResult::Neutral),
      dkim_checked_(false),
      dkim_result_(DKIM::Neutral),
      dmarc_result_(DMARCResult::NotEvaluated)
   {

   }

   SPFResult
   SenderAuthentication::EvaluateSPF(std::shared_ptr<SpamTestData> testData)
   {
      if (spf_checked_)
         return spf_result_;

      const IPAddress &originatingAddress = testData->GetOriginatingIP();

      if (originatingAddress.IsAny())
         return spf_result_;

      String explanation;
      SPFResult result = SPF::Instance()->Test(originatingAddress.ToString(), testData->GetEnvelopeFrom(), testData->GetHeloHost(), explanation);

      String domain = SPF::GetCheckedDomain(testData->GetEnvelopeFrom(), testData->GetHeloHost());

      spf_checked_ = true;
      spf_result_ = result;
      spf_domain_ = domain;
      spf_explanation_ = explanation;

      return spf_result_;
   }

   bool
   SenderAuthentication::GetSPFChecked() const
   {
      return spf_checked_;
   }

   SPFResult
   SenderAuthentication::GetSPFResult() const
   {
      return spf_result_;
   }

   String
   SenderAuthentication::GetSPFDomain() const
   {
      return spf_domain_;
   }

   String
   SenderAuthentication::GetSPFExplanation() const
   {
      return spf_explanation_;
   }

   DKIM::Result
   SenderAuthentication::EvaluateDKIM(std::shared_ptr<SpamTestData> testData)
   {
      if (dkim_checked_)
         return dkim_result_;

      std::shared_ptr<MessageData> messageData = testData->GetMessageData();

      if (!messageData)
         return dkim_result_;

      const String fileName = PersistentMessage::GetFileName(messageData->GetMessage());

      DKIM dkim;
      std::vector<std::pair<AnsiString, DKIM::Result> > signatureResults;
      DKIM::Result result = dkim.Verify(fileName, signatureResults);

      dkim_checked_ = true;
      dkim_result_ = result;
      dkim_signatures_ = signatureResults;

      return dkim_result_;
   }

   bool
   SenderAuthentication::GetDKIMChecked() const
   {
      return dkim_checked_;
   }

   DKIM::Result
   SenderAuthentication::GetDKIMResult() const
   {
      return dkim_result_;
   }

   const std::vector<std::pair<AnsiString, DKIM::Result> > &
   SenderAuthentication::GetDKIMSignatures() const
   {
      return dkim_signatures_;
   }

   void
   SenderAuthentication::SetDMARCResult(DMARCResult result, const String &headerFromDomain)
   {
      dmarc_result_ = result;
      dmarc_domain_ = headerFromDomain;
   }

   SenderAuthentication::DMARCResult
   SenderAuthentication::GetDMARCResult() const
   {
      return dmarc_result_;
   }

   String
   SenderAuthentication::GetDMARCDomain() const
   {
      return dmarc_domain_;
   }
}
