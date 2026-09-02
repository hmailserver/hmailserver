// Copyright (c) 2026 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "StdAfx.h"

#include "SenderRewriteScheme.h"

#include "SRS.h"

#include "../SMTPConfiguration.h"

#include "../../Common/Application/ObjectCache.h"
#include "../../Common/BO/Domain.h"
#include "../../Common/BO/DomainAliases.h"
#include "../../Common/Cache/CacheContainer.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   bool
   SenderRewriteScheme::GetIsEnabled()
   {
      return Configuration::Instance()->GetSMTPConfiguration()->GetSRSEnabled();
   }

   String
   SenderRewriteScheme::CreateForwardingSender(const String &originalSender, const String &forwardingAddress, const String &targetAddress)
   {
      if (!GetIsEnabled())
         return "";

      if (originalSender.IsEmpty())
      {
         // A bounce is sent with an empty envelope sender, and must stay that way.
         return "";
      }

      if (IsLocalDomain_(targetAddress))
      {
         // The message is not leaving this server, so there is no SPF check ahead of it
         // to pass, and a rewritten sender would only make the message harder to read.
         return "";
      }

      if (IsLocalDomain_(originalSender))
      {
         // We are the sender's own mail server. Whatever its domain permits as a sender
         // already covers us, and leaving the sender alone keeps the message aligned
         // with its From-header for DMARC.
         return "";
      }

      String forwardingDomain = StringParser::ExtractDomain(forwardingAddress);

      if (forwardingDomain.IsEmpty())
         return "";

      std::shared_ptr<SRS> srs = Create_();

      if (!srs)
         return "";

      String rewrittenSender = srs->Forward(originalSender, forwardingDomain);

      if (rewrittenSender.IsEmpty())
      {
         String message = Formatter::Format("The envelope sender {0} could not be rewritten using SRS when forwarding to {1}. The message is forwarded with its original sender.",
            originalSender, targetAddress);

         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5732, "SenderRewriteScheme::CreateForwardingSender", message);

         return "";
      }

      LOG_DEBUG(Formatter::Format("SRS: Rewrote envelope sender {0} to {1}.", originalSender, rewrittenSender));

      return rewrittenSender;
   }

   bool
   SenderRewriteScheme::IsSrsRecipient(const String &recipientAddress)
   {
      if (!GetIsEnabled())
         return false;

      return SRS::IsSrsAddress(recipientAddress);
   }

   bool
   SenderRewriteScheme::TryReverse(const String &recipientAddress, String &originalSender, String &errorMessage)
   {
      std::shared_ptr<SRS> srs = Create_();

      if (!srs)
      {
         errorMessage = SRS::GetResultDescription(SRS::ResultNotConfigured);
         return false;
      }

      SRS::ReverseResult result = srs->Reverse(recipientAddress, originalSender);

      if (result != SRS::ResultSuccess)
      {
         errorMessage = SRS::GetResultDescription(result);

         LOG_DEBUG(Formatter::Format("SRS: The address {0} was not accepted. {1}", recipientAddress, errorMessage));

         return false;
      }

      LOG_DEBUG(Formatter::Format("SRS: The address {0} was reversed to {1}.", recipientAddress, originalSender));

      return true;
   }

   std::shared_ptr<SRS>
   SenderRewriteScheme::Create_()
   {
      std::shared_ptr<SMTPConfiguration> configuration = Configuration::Instance()->GetSMTPConfiguration();

      AnsiString secret = configuration->GetSRSSecret();

      if (secret.IsEmpty())
      {
         // The server generates one when it starts, so this means either that the
         // database could not be written to, or that someone has cleared it.
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5733, "SenderRewriteScheme::Create_",
            "SRS is enabled, but no SRS secret has been configured. Addresses can neither be rewritten nor reversed.");

         std::shared_ptr<SRS> empty;
         return empty;
      }

      return std::shared_ptr<SRS>(new SRS(secret, configuration->GetSRSMaxAgeDays(), configuration->GetSRSHashLength()));
   }

   bool
   SenderRewriteScheme::IsLocalDomain_(const String &address)
   {
      std::shared_ptr<DomainAliases> domainAliases = ObjectCache::Instance()->GetDomainAliases();

      String domainName = StringParser::ExtractDomain(domainAliases->ApplyAliasesOnAddress(address));

      if (domainName.IsEmpty())
         return false;

      return CacheContainer::Instance()->GetDomain(domainName) != nullptr;
   }
}
