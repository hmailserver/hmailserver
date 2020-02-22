// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "DKIMSigner.h"
#include "DKIM.h"

#include "Canonicalization.h"

#include "../../BO/Message.h"
#include "../../BO/Domain.h"
#include "../../Cache/CacheContainer.h"
#include "../../Util/Hashing/HashCreator.h"

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
     
      AnsiString senderAddress = message->GetFromAddress();
      AnsiString senderDomain = StringParser::ExtractDomain(senderAddress);

      // Check if signing is enabled for this domain.
      std::shared_ptr<const Domain> pDomain = CacheContainer::Instance()->GetDomain(senderDomain);

      if (!pDomain || !pDomain->GetDKIMEnabled())
         return;

      LOG_DEBUG("Signing message using DKIM...");

      AnsiString selector = pDomain->GetDKIMSelector();
      AnsiString domain = pDomain->GetName();
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
      if (!dkim.Sign(message, domain, selector, privateKeyFile, algorithm, headerMethod, bodyMethod))
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5306, "DKIMSigner::Sign", "Message signing using DKIM failed.");
      }
   }
}