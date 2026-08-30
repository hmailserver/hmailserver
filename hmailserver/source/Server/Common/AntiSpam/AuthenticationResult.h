// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

#include "DKIM/DKIM.h"
#include "../../SMTP/SPF/SPF.h"

namespace HM
{
   // Collects the SPF and DKIM outcomes for a single message, so that they can be
   // reused by tests running later during the same delivery.
   class AuthenticationResult
   {
   public:

      enum class DMARCResult
      {
         NotEvaluated = 0,
         Pass = 1,
         Fail = 2
      };

      AuthenticationResult();

      // domain is the identity SPF authenticated - the MAIL FROM domain, or the
      // HELO host when the message has a null sender.
      void SetSPFResult(SPF::Result result, const String &domain);
      bool GetSPFChecked() const;
      SPF::Result GetSPFResult() const;
      String GetSPFDomain() const;

      // One entry per DKIM signature that was evaluated, keyed on the d= domain.
      void SetDKIMResults(const std::vector<std::pair<AnsiString, DKIM::Result> > &signatures);
      bool GetDKIMChecked() const;
      const std::vector<std::pair<AnsiString, DKIM::Result> > &GetDKIMSignatures() const;

      void SetDMARCResult(DMARCResult result, const String &headerFromDomain);
      DMARCResult GetDMARCResult() const;
      String GetDMARCDomain() const;

   private:

      bool spf_checked_;
      SPF::Result spf_result_;
      String spf_domain_;

      bool dkim_checked_;
      std::vector<std::pair<AnsiString, DKIM::Result> > dkim_signatures_;

      DMARCResult dmarc_result_;
      String dmarc_domain_;
   };
}
