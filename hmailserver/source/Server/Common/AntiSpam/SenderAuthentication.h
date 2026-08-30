// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

#include "DKIM/DKIM.h"
#include "../../SMTP/SPF/SPF.h"

namespace HM
{
   class SpamTestData;

   // Authenticates the sender of a message using SPF, DKIM and DMARC, and collects
   // the outcomes so that every test which needs them shares a single evaluation.
   class SenderAuthentication
   {
   public:

      enum class DMARCResult
      {
         NotEvaluated = 0,
         Pass = 1,
         Fail = 2
      };

      SenderAuthentication();

      // Verifies SPF, unless it has already been verified for this message.
      SPF::Result EvaluateSPF(std::shared_ptr<SpamTestData> testData);

      bool GetSPFChecked() const;
      SPF::Result GetSPFResult() const;
      // The identity SPF authenticated - the MAIL FROM domain, or the HELO host
      // when the message has a null sender.
      String GetSPFDomain() const;
      String GetSPFExplanation() const;

      // Verifies every DKIM signature, unless it has already been done.
      DKIM::Result EvaluateDKIM(std::shared_ptr<SpamTestData> testData);

      bool GetDKIMChecked() const;
      DKIM::Result GetDKIMResult() const;
      // One entry per DKIM signature that was evaluated, keyed on the d= domain.
      const std::vector<std::pair<AnsiString, DKIM::Result> > &GetDKIMSignatures() const;

      void SetDMARCResult(DMARCResult result, const String &headerFromDomain);
      DMARCResult GetDMARCResult() const;
      String GetDMARCDomain() const;

   private:

      bool spf_checked_;
      SPF::Result spf_result_;
      String spf_domain_;
      String spf_explanation_;

      bool dkim_checked_;
      DKIM::Result dkim_result_;
      std::vector<std::pair<AnsiString, DKIM::Result> > dkim_signatures_;

      DMARCResult dmarc_result_;
      String dmarc_domain_;
   };
}
