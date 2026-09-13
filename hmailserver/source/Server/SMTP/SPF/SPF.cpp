// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "stdafx.h"

#include "SPF.h"

#include "SPFAddress.h"
#include "SPFDnsResolver.h"
#include "SPFEvaluator.h"

#include "../../Common/Application/Configuration.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   SPF::SPF(void)
   {

   }

   SPF::~SPF(void)
   {

   }

   String
   SPF::GetCheckedDomain(const String &senderEmail, const String &heloHost)
   {
      String domain = StringParser::ExtractDomain(senderEmail);

      if (domain.IsEmpty())
         return heloHost;

      return domain;
   }

   SPF::Result
   SPF::Test(const String &sSenderIP, const String &sSenderEmail, const String &sHeloHost, String &sExplanation)
   {
      sExplanation = "";

      SPFAddress clientAddress;

      if (!SPFAddress::TryParse(AnsiString(sSenderIP), clientAddress))
      {
         // Section 4.1 makes the client address an input, so without one there is no check
         // to make. A "none" rather than an error of either kind: the domain has not been
         // asked anything. A scoped link-local address is the way to get here.
         return SPFResult::None;
      }

      auto lookup = std::make_shared<SPFDnsResolver>();

      SPFEvaluator evaluator(lookup);

      // The r and t macros of section 7.2, which only the text an exp modifier
      // points at may use. The host name is the one the Authentication-Results
      // header identifies this server by.
      evaluator.SetReceivingHost(AnsiString(Configuration::Instance()->GetHostName()));
      evaluator.SetTimestamp((__int64) ::time(0));

      AnsiString explanation;

      SPFResult result = evaluator.Check(clientAddress,
                                         AnsiString(GetCheckedDomain(sSenderEmail, sHeloHost)),
                                         AnsiString(sSenderEmail),
                                         AnsiString(sHeloHost),
                                         explanation);

      sExplanation = String(explanation);

      return result;
   }
}
