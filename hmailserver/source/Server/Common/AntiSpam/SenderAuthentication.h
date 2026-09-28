// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

#include "DKIM/DKIM.h"
#include "ARC/ARCVerifier.h"
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
      SPFResult EvaluateSPF(std::shared_ptr<SpamTestData> testData);

      bool GetSPFChecked() const;
      SPFResult GetSPFResult() const;
      // The identity SPF authenticated - the MAIL FROM domain, or the HELO host
      // when the message has a null sender.
      String GetSPFDomain() const;
      // Whether that identity was the HELO host, RFC 7208 section 2.4.
      bool GetSPFCheckedHelo() const;
      // The client address the check was made for.
      String GetSPFClientAddress() const;
      String GetSPFExplanation() const;

      // Verifies every DKIM signature, unless it has already been done.
      DKIM::Result EvaluateDKIM(std::shared_ptr<SpamTestData> testData);

      bool GetDKIMChecked() const;
      DKIM::Result GetDKIMResult() const;
      // One entry per DKIM signature that was evaluated, keyed on the d= domain.
      const std::vector<std::pair<AnsiString, DKIM::Result> > &GetDKIMSignatures() const;

      // Validates the ARC chain, unless it has already been done.
      ARCVerifier::Result EvaluateARC(std::shared_ptr<SpamTestData> testData);

      bool GetARCChecked() const;
      ARCVerifier::Result GetARCResult() const;
      // The d= of the last ARC-Seal, set when the chain passed.
      AnsiString GetARCSealerDomain() const;
      // The results the last sealer recorded, set when the chain passed.
      AnsiString GetARCAuthenticationResults() const;
      String GetARCFailureReason() const;
      // The client address the chain was received from.
      String GetARCClientAddress() const;

      void SetDMARCResult(DMARCResult result, const String &headerFromDomain);
      DMARCResult GetDMARCResult() const;
      String GetDMARCDomain() const;

      // Set when a failing DMARC result was overridden since a trusted sealer vouched for the message.
      void SetDMARCOverriddenBySealer(const String &sealerDomain);
      String GetDMARCOverriddenBySealer() const;

   private:

      bool spf_checked_;
      SPFResult spf_result_;
      String spf_domain_;
      bool spf_checked_helo_;
      String spf_client_address_;
      String spf_explanation_;

      bool dkim_checked_;
      DKIM::Result dkim_result_;
      std::vector<std::pair<AnsiString, DKIM::Result> > dkim_signatures_;

      bool arc_checked_;
      ARCVerifier::Result arc_result_;
      AnsiString arc_sealer_domain_;
      AnsiString arc_authentication_results_;
      String arc_failure_reason_;
      String arc_client_address_;

      DMARCResult dmarc_result_;
      String dmarc_domain_;
      String dmarc_overridden_by_sealer_;
   };
}
