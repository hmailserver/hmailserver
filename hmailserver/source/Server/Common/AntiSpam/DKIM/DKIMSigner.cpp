// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "DKIMSigner.h"
#include "DKIM.h"

#include "Canonicalization.h"

#include "../../BO/Message.h"
#include "../../BO/Domain.h"
#include "../../BO/DomainAliases.h"
#include "../../Application/ObjectCache.h"
#include "../../Cache/CacheContainer.h"
#include "../../Util/Hashing/HashCreator.h"
#include "../../Mime/Mime.h"
#include "../../Mime/SevenBitConverter.h"
#include "../../Persistence/PersistentMessage.h"
#include "../../Util/FileUtilities.h"
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

      // RFC 6376 section 5.3: convert the message to a 7-bit form before signing it. Signed
      // as 8-bit, the signature breaks at the first hop that downgrades the body to
      // quoted-printable, because that rewrite changes the bytes the body hash covers.
      if (ConvertToSevenBit_(message, fileName, mimeHeader, domain))
      {
         // The message on disk changed, and its Content-Transfer-Encoding with it. The
         // header is signed, so the signature has to be taken over the new one.
         header = PersistentMessage::LoadHeader(fileName);
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

   bool
   DKIMSigner::ConvertToSevenBit_(std::shared_ptr<Message> message, const String &fileName, MimeHeader &mimeHeader, const AnsiString &domain)
   {
      // The same rule DKIM::Sign applies: a message already signed for this domain is not
      // signed again, so there is nothing to convert it for. A signature for some other
      // domain does not hold the conversion back - the downgrade that would break ours
      // breaks that one just the same, converted or not.
      if (DKIM::HasSignatureForDomain(mimeHeader, domain))
         return false;

      // DKIM::Sign leaves a message above this size unsigned, so converting one would be a
      // rewrite of the largest messages there are in exchange for nothing.
      if (FileUtilities::FileSize(fileName) > DKIM::MaxFileSize)
         return false;

      // A body holding no 8-bit octets cannot need converting. Checking that first keeps
      // the message from being parsed at all in the case nearly every message falls into.
      if (!SevenBitConverter::ContainsEightBitOctets(PersistentMessage::LoadBody(fileName)))
         return false;

      // A load cut short by an exception still returns true, holding whatever was parsed
      // before it. Writing that back would drop the rest of the message.
      MimeBody mimeBody;
      if (!mimeBody.LoadFromFile(fileName) || !mimeBody.WasLoadedFromFile())
         return false;

      if (!SevenBitConverter::Convert(mimeBody))
         return false;

      AnsiString converted;
      mimeBody.Store(converted);

      // Encoding grows the content, and DKIM::Sign measures the file again before signing.
      // A message the conversion pushes past the limit would be rewritten and then left
      // unsigned anyway, so it is better off signed as it is.
      if (converted.size() > (size_t) DKIM::MaxFileSize)
      {
         LOG_DEBUG("Message was not converted to 7-bit before DKIM signing since the converted message would exceed the max DKIM size of 50MB.");
         return false;
      }

      // Written beside the message and moved over it, as TraceHeaderWriter does, so that a
      // failed write leaves the original in place to be signed as it was received.
      String tempFile = fileName + ".tmp";

      if (!FileUtilities::WriteToFile(tempFile, converted) || !FileUtilities::Move(tempFile, fileName))
      {
         FileUtilities::DeleteFile(tempFile);

         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5313, "DKIMSigner::ConvertToSevenBit_",
            "Failed to write the message back after converting it to a 7-bit content-transfer-encoding. The message is signed as it was received.");

         return false;
      }

      message->SetSize((int) converted.size());

      LOG_DEBUG("Converted message to a 7-bit content-transfer-encoding before DKIM signing.");

      return true;
   }
}