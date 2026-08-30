// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

namespace HM
{
   // A DMARC policy record, as published in a _dmarc TXT record. See RFC 7489, section 6.3.
   class DMARCRecord
   {
   public:

      enum class Policy
      {
         None = 0,
         Quarantine = 1,
         Reject = 2
      };

      enum class Alignment
      {
         Relaxed = 0,
         Strict = 1
      };

      DMARCRecord();

      // Returns false if the text isn't a valid DMARC policy record.
      static bool Parse(const AnsiString &textRecord, DMARCRecord &record);

      // True if the text starts with the DMARC version tag. Used to pick the
      // DMARC record out of the other TXT records published for a domain.
      static bool HasVersionTag(const AnsiString &textRecord);

      Policy GetPolicy() const { return policy_; }
      Policy GetSubdomainPolicy() const { return subdomain_policy_; }
      bool GetHasSubdomainPolicy() const { return has_subdomain_policy_; }

      Alignment GetDKIMAlignment() const { return dkim_alignment_; }
      Alignment GetSPFAlignment() const { return spf_alignment_; }

      int GetPercent() const { return percent_; }

   private:

      static bool ParsePolicy_(const AnsiString &value, Policy &policy);
      static Alignment ParseAlignment_(const AnsiString &value);

      Policy policy_;
      Policy subdomain_policy_;
      bool has_subdomain_policy_;

      Alignment dkim_alignment_;
      Alignment spf_alignment_;

      int percent_;
   };
}
