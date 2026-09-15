// Copyright (c) 2026 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "StdAfx.h"

#include "SenderRewriteScheme.h"

#include "SRS.h"

#include "../SMTPConfiguration.h"

#include "../../Common/Application/ObjectCache.h"
#include "../../Common/BO/Account.h"
#include "../../Common/BO/Alias.h"
#include "../../Common/BO/DistributionList.h"
#include "../../Common/BO/DistributionListRecipient.h"
#include "../../Common/BO/DistributionListRecipients.h"
#include "../../Common/BO/Domain.h"
#include "../../Common/BO/DomainAliases.h"
#include "../../Common/BO/Message.h"
#include "../../Common/Cache/CacheContainer.h"

// DistributionListRecipients is a Collection over this, and instantiating the
// template needs the whole type rather than the forward declaration it carries.
#include "../../Common/Persistence/PersistentDistributionListRecipient.h"

#include <atomic>

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   namespace
   {
      // With SRS switched on and no secret in the database, every RCPT TO and every
      // forwarded message ends up without one. That is worth reporting, but not once per
      // message, so it is reported again only after a secret has been seen in between.
      std::atomic<bool> missing_secret_reported(false);
   }

   bool
   SenderRewriteScheme::GetIsEnabled()
   {
      return Configuration::Instance()->GetSMTPConfiguration()->GetSRSEnabled();
   }

   void
   SenderRewriteScheme::ApplyToForwardedMessage(std::shared_ptr<Message> message, const String &forwardingAccount,
                                                const String &forwardingDomain, const String &targetAddress)
   {
      // Rewrite the envelope sender so that the message passes the SPF check at the server
      // we forward it to, while a bounce still finds its way back to whoever sent the
      // message. Where SRS does not rewrite it, the ini-file setting below is the older,
      // blunter version of the same thing: it passes SPF, but the bounce ends up with the
      // account which forwarded the message rather than with the sender.
      String rewrittenSender = CreateForwardingSender(message->GetFromAddress(), forwardingDomain, targetAddress);

      if (!rewrittenSender.IsEmpty())
      {
         message->SetFromAddress(rewrittenSender);
         return;
      }

      if (forwardingAccount.IsEmpty() || message->GetFromAddress().IsEmpty())
         return;

      if (IniFileSettings::Instance()->GetRewriteEnvelopeFromWhenForwarding())
         message->SetFromAddress(forwardingAccount);
   }

   String
   SenderRewriteScheme::CreateForwardingSender(const String &originalSender, const String &forwardingDomain, const String &targetAddress)
   {
      if (!GetIsEnabled())
         return "";

      if (originalSender.IsEmpty())
      {
         // A bounce is sent with an empty envelope sender, and must stay that way.
         return "";
      }

      if (forwardingDomain.IsEmpty())
         return "";

      if (!LeavesThisServer_(targetAddress, 0))
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

      std::shared_ptr<SRS> srs = Create_();

      if (!srs)
         return "";

      String rewrittenSender = srs->Forward(originalSender, forwardingDomain);

      if (rewrittenSender.IsEmpty())
      {
         // Nothing went wrong with the server; the address simply cannot be rewritten -
         // it is not one we can take apart, or rewriting it would make it longer than an
         // address may be. The message is forwarded with the sender it arrived with,
         // which is what hMailServer did before SRS existed.
         LOG_DEBUG(Formatter::Format("SRS: The envelope sender {0} was not rewritten when forwarding to {1}.",
            originalSender, targetAddress));

         return "";
      }

      LOG_DEBUG(Formatter::Format("SRS: Rewrote envelope sender {0} to {1}.", originalSender, rewrittenSender));

      return rewrittenSender;
   }

   SenderRewriteScheme::ReverseOutcome
   SenderRewriteScheme::TryReverse(const String &recipientAddress, String &originalSender, String &errorMessage)
   {
      // The cheap test comes first: this runs for every recipient of every message, and
      // almost none of them are addresses we have handed out.
      if (!SRS::IsSrsAddress(recipientAddress))
         return NotAnSrsAddress;

      if (!GetIsEnabled())
      {
         // With SRS switched off, an address which looks like one of ours is an ordinary
         // address which may well belong to an account.
         return NotAnSrsAddress;
      }

      std::shared_ptr<SRS> srs = Create_();

      if (!srs)
      {
         errorMessage = SRS::GetResultDescription(SRS::ResultNotConfigured);
         return ReversalFailed;
      }

      SRS::ReverseResult result = srs->Reverse(recipientAddress, originalSender);

      if (result != SRS::ResultSuccess)
      {
         errorMessage = SRS::GetResultDescription(result);

         LOG_DEBUG(Formatter::Format("SRS: The address {0} was not accepted. {1}", recipientAddress, errorMessage));

         return ReversalFailed;
      }

      LOG_DEBUG(Formatter::Format("SRS: The address {0} was reversed to {1}.", recipientAddress, originalSender));

      return Reversed;
   }

   std::shared_ptr<SRS>
   SenderRewriteScheme::Create_()
   {
      std::shared_ptr<SMTPConfiguration> configuration = Configuration::Instance()->GetSMTPConfiguration();

      String secret = configuration->GetSRSSecret();

      if (secret.IsEmpty())
      {
         // The server generates one when it starts, so this means either that the
         // database could not be written to, or that someone has cleared it.
         if (!missing_secret_reported.exchange(true))
         {
            ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5732, "SenderRewriteScheme::Create_",
               "SRS is enabled, but no SRS secret has been configured. Addresses can neither be rewritten nor reversed.");
         }

         std::shared_ptr<SRS> empty;
         return empty;
      }

      missing_secret_reported = false;

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

   bool
   SenderRewriteScheme::LeavesThisServer_(const String &address, int recursionLevel)
   {
      if (recursionLevel > 25)
      {
         // Extreme aliasing. Whatever is at the end of it, the sender is left alone.
         return false;
      }

      std::shared_ptr<DomainAliases> domainAliases = ObjectCache::Instance()->GetDomainAliases();

      String resolvedAddress = domainAliases->ApplyAliasesOnAddress(address);
      String domainName = StringParser::ExtractDomain(resolvedAddress);

      if (domainName.IsEmpty())
         return false;

      if (!CacheContainer::Instance()->GetDomain(domainName))
         return true;

      if (CacheContainer::Instance()->GetAccount(resolvedAddress))
         return false;

      // An alias or a distribution list in one of our domains is only where the message
      // is handed on; where it ends up is what decides whether it leaves the server, and
      // whether the SPF check at the next server is one we have to pass.
      std::shared_ptr<const Alias> alias = CacheContainer::Instance()->GetAlias(resolvedAddress);

      if (alias)
         return LeavesThisServer_(alias->GetValue(), recursionLevel + 1);

      std::shared_ptr<const DistributionList> list = CacheContainer::Instance()->GetDistributionList(resolvedAddress);

      if (list)
      {
         const std::vector<std::shared_ptr<DistributionListRecipient> > members = list->GetMembers()->GetConstVector();

         for (auto member = members.begin(); member != members.end(); member++)
         {
            if (LeavesThisServer_((*member)->GetAddress(), recursionLevel + 1))
               return true;
         }

         return false;
      }

      // Nothing in the domain answers to the address, so it is the catch-all account or
      // nothing at all. Either way the message stays here.
      return false;
   }
}
