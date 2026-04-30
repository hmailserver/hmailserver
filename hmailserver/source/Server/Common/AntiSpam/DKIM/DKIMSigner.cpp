// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "DKIMSigner.h"
#include "DKIM.h"

#include "Canonicalization.h"

#include "../../BO/Message.h"
#include "../../BO/Domain.h"
#include "../../BO/DomainAliases.h"
#include "../../Application/ObjectCache.h"
#include "../../Cache/CacheContainer.h"
#include "../../Util/Hashing/HashCreator.h"
#include "../../MIME/Mime.h"
#include "../../Persistence/PersistentMessage.h"
#include "../../Util/Parsing/AddresslistParser.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   DKIMSigner::DKIMSigner()
   {
      
   }

   void
   DKIMSigner::Sign(std::shared_ptr<Message> message)
   {
      // Load the message header once. It will be reused for both domain lookup
      // and signing, avoiding a second read from disk inside DKIM::Sign.
      const String fileName = PersistentMessage::GetFileName(message);
      AnsiString header = PersistentMessage::LoadHeader(fileName);

      // Determine the signing domain from the RFC 5322 From: header, not the
      // envelope from (MAIL FROM). DMARC alignment requires d= to match the
      // From: header domain.
      MimeHeader mimeHeader;
      mimeHeader.Load(header.c_str(), header.GetLength(), false);

      AnsiString senderDomain;
      AnsiString senderAddress;
      MimeField *fromField = mimeHeader.GetField("From");
      if (fromField)
      {
         AddresslistParser parser;
         auto addresses = parser.ParseList(String(fromField->GetValue()));
         if (!addresses.empty())
         {
            senderAddress = addresses[0]->sMailboxName + "@" + addresses[0]->sDomainName;
            senderDomain = addresses[0]->sDomainName;
         }
      }

      if (senderDomain.IsEmpty())
      {
         senderAddress = message->GetFromAddress();
         senderDomain = StringParser::ExtractDomain(senderAddress);
      }

      std::shared_ptr<DomainAliases> pDA = ObjectCache::Instance()->GetDomainAliases();
      // try to get mailbox from the alias (if it is an alias actually)
      String sSender = pDA->ApplyAliasesOnAddress(senderAddress);
      AnsiString mbDomain = StringParser::ExtractDomain(sSender);

      // was the sender address from the main domain already?
      bool sameDomain = senderDomain.CompareNoCase(mbDomain) == 0;

      // Check if signing is enabled for this domain.
      std::shared_ptr<const Domain> pDomain = CacheContainer::Instance()->GetDomain(mbDomain);

      if (!pDomain || !pDomain->GetDKIMEnabled())
         return;
      // main domain signing enabled, do we have the sender address from the main domain or do we allow signing of the aliases?
      if (!(pDomain->GetDKIMAliasesEnabled() || sameDomain))
         return;

      LOG_DEBUG("Signing message using DKIM...");

      AnsiString selector = pDomain->GetDKIMSelector();
      // the senderDomain is either the main domain or it is allowed to sign using the key from the main domain
      AnsiString domain = senderDomain;
      AnsiString privateKeyFile = pDomain->GetDKIMPrivateKeyFile();

      if (selector.IsEmpty() || privateKeyFile.IsEmpty())
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5305, "DKIMSigner::Sign", "Either the selector or private key file was not specified.");
         return;
      }

      Canonicalization::CanonicalizeMethod headerMethod = (Canonicalization::CanonicalizeMethod) pDomain->GetDKIMHeaderCanonicalizationMethod();
      Canonicalization::CanonicalizeMethod bodyMethod = (Canonicalization::CanonicalizeMethod) pDomain->GetDKIMBodyCanonicalizationMethod();
      HashCreator::HashType algorithm = (HashCreator::HashType) pDomain->GetDKIMSigningAlgorithm();

      DKIM dkim;
      if (!dkim.Sign(message, header, domain, selector, privateKeyFile, algorithm, headerMethod, bodyMethod))
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5306, "DKIMSigner::Sign", "Message signing using DKIM failed.");
      }
   }
}