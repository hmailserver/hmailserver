// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "StdAfx.h"
#include "AuthenticationResult.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   AuthenticationResult::AuthenticationResult() :
      spf_checked_(false),
      spf_result_(SPF::Neutral),
      dkim_checked_(false),
      dmarc_result_(DMARCResult::NotEvaluated)
   {

   }

   void
   AuthenticationResult::SetSPFResult(SPF::Result result, const String &domain)
   {
      spf_checked_ = true;
      spf_result_ = result;
      spf_domain_ = domain;
   }

   bool
   AuthenticationResult::GetSPFChecked() const
   {
      return spf_checked_;
   }

   SPF::Result
   AuthenticationResult::GetSPFResult() const
   {
      return spf_result_;
   }

   String
   AuthenticationResult::GetSPFDomain() const
   {
      return spf_domain_;
   }

   void
   AuthenticationResult::SetDKIMResults(const std::vector<std::pair<AnsiString, DKIM::Result> > &signatures)
   {
      dkim_checked_ = true;
      dkim_signatures_ = signatures;
   }

   bool
   AuthenticationResult::GetDKIMChecked() const
   {
      return dkim_checked_;
   }

   const std::vector<std::pair<AnsiString, DKIM::Result> > &
   AuthenticationResult::GetDKIMSignatures() const
   {
      return dkim_signatures_;
   }

   void
   AuthenticationResult::SetDMARCResult(DMARCResult result, const String &headerFromDomain)
   {
      dmarc_result_ = result;
      dmarc_domain_ = headerFromDomain;
   }

   AuthenticationResult::DMARCResult
   AuthenticationResult::GetDMARCResult() const
   {
      return dmarc_result_;
   }

   String
   AuthenticationResult::GetDMARCDomain() const
   {
      return dmarc_domain_;
   }
}
