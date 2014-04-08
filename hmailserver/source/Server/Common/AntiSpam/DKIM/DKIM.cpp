// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "DKIM.h"
#include "DKIMParameters.h"

#include "../../Util/Hashing/HashCreator.h"
#include "../../Util/Encoding/Base64.h"
#include "../../BO/MessageData.h"
#include "../../BO/Message.h"
#include "../../MIME/MimeCode.h"
#include "../../MIME/Mime.h"
#include "../../TCPIP/DNSResolver.h"
#include "../../Util/TraceHeaderWriter.h"
#include "../../Persistence/PersistentMessage.h"

#include <openssl/rsa.h>
#include <openssl/obj_mac.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/evp.h>


#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   vector<AnsiString> DKIM::_recommendedHeaderFields;

   DKIM::DKIM()
   {

   }

   void 
   DKIM::Initialize()
   {
      OpenSSL_add_all_algorithms();
      ERR_load_crypto_strings();
      ERR_load_EVP_strings();

      _recommendedHeaderFields.push_back("From");
      _recommendedHeaderFields.push_back("Sender");
      _recommendedHeaderFields.push_back("Reply-To");
      _recommendedHeaderFields.push_back("Subject");
      _recommendedHeaderFields.push_back("Date");
      _recommendedHeaderFields.push_back("Message-ID");
      _recommendedHeaderFields.push_back("To");
      _recommendedHeaderFields.push_back("CC");
      _recommendedHeaderFields.push_back("MIME-Version");

      _recommendedHeaderFields.push_back("Content-Type");
      _recommendedHeaderFields.push_back("Content-Transfer-Encoding");
      _recommendedHeaderFields.push_back("Content-ID");
      _recommendedHeaderFields.push_back("Content-Description");

      _recommendedHeaderFields.push_back("Resent-Date");
      _recommendedHeaderFields.push_back("Resent-From");
      _recommendedHeaderFields.push_back("Resent-Sender");
      _recommendedHeaderFields.push_back("Resent-To");
      _recommendedHeaderFields.push_back("Resent-Cc");
      _recommendedHeaderFields.push_back("Resent-Message-ID");

      _recommendedHeaderFields.push_back("In-Reply-To");
      _recommendedHeaderFields.push_back("References");

      _recommendedHeaderFields.push_back("List-Id");
      _recommendedHeaderFields.push_back("List-Help");
      _recommendedHeaderFields.push_back("List-Unsubscribe");
      _recommendedHeaderFields.push_back("List-Subscribe");
      _recommendedHeaderFields.push_back("List-Post");
      _recommendedHeaderFields.push_back("List-Owner");
      _recommendedHeaderFields.push_back("List-Archive");
   }

   // helper.
   EVP_PKEY* 
   _GetPublicKey(const AnsiString &keyData)
   {
      // base64 decode the public key.
      AnsiString publicKeyData = Base64::Decode(keyData, keyData.GetLength());
      const unsigned char * publicKeyDataPointer = (const unsigned char*) publicKeyData.GetBuffer();

      EVP_PKEY *publicKey = d2i_PUBKEY(NULL, &publicKeyDataPointer, publicKeyData.GetLength());

      return publicKey;
   }

   bool 
   DKIM::Sign(shared_ptr<Message> message,
              const AnsiString &domain,
              const AnsiString &selector,
              const String &privateKey,
              HashCreator::HashType algorithm, 
              Canonicalization::CanonicalizeMethod headerMethod, 
              Canonicalization::CanonicalizeMethod bodyMethod)
   {

      shared_ptr<Canonicalization> bodyCanonicalization = _CreateCanonicalization(bodyMethod);
      shared_ptr<Canonicalization> headerCanonicalization = _CreateCanonicalization(headerMethod);

      if (!bodyCanonicalization || !headerCanonicalization)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5307, "DKIM::Sign", "Creation of canonicalization method failed.");
         return false;
      }

      const String fileName = PersistentMessage::GetFileName(message);

      if (FileUtilities::FileSize(fileName) > MaxFileSize)
      {
         LOG_DEBUG("Message was not signed using DKIM since the size of the message exceeded the max DKIM size of 10MB.");
         return true;
      }

      String messageBody = bodyCanonicalization->CanonicalizeBody(PersistentMessage::LoadBody(fileName));

      HashCreator shaer(algorithm);
      String bodyHash = shaer.GenerateHashNoSalt(messageBody, HashCreator::base64);

      AnsiString header = PersistentMessage::LoadHeader(fileName);

      pair<AnsiString, AnsiString> dummySignatureField;

      AnsiString fieldList;
      AnsiString canonicalizedHeader = headerCanonicalization->CanonicalizeHeader(header, dummySignatureField, _recommendedHeaderFields, fieldList);
   
      String tagV = "1";
      String tagA = algorithm == HashCreator::SHA1 ? "rsa-sha1" : "rsa-sha256";
      String tagC = headerMethod == Canonicalization::Simple ? "simple/" : "relaxed/";
      tagC.append(bodyMethod == Canonicalization::Simple ? _T("simple") : _T("relaxed"));
      String tagQ = "dns/txt";

      String tagDomain = domain;
      String tagSelector = selector;

      String headerValue = _BuildSignatureHeader(tagA, tagDomain, tagSelector, tagC, tagQ, fieldList, bodyHash, "");
      
      canonicalizedHeader += headerCanonicalization->CanonicalizeHeaderLine("dkim-signature", headerValue);

      AnsiString privateKeyContent = FileUtilities::ReadCompleteTextFile(String(privateKey));

      AnsiString signatureString = _SignHash(privateKeyContent, canonicalizedHeader, algorithm);
      if (signatureString == "")
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5308, "DKIM::Sign", "Failed to create siganture.");
         return false;
      }
      
      headerValue = _BuildSignatureHeader(tagA, tagDomain, tagSelector, tagC, tagQ, fieldList, bodyHash, signatureString);

      // output to file.
      std::vector<pair<AnsiString, AnsiString> > fieldsToWrite;
      fieldsToWrite.push_back(std::make_pair("dkim-signature", headerValue));

      TraceHeaderWriter writer;
      bool result = writer.Write(fileName, message, fieldsToWrite);

    
      //   debugging code.
      /*
         bool immediateVerification = false;
         if (immediateVerification)
         {
            if (!Verify(message))
            {
               assert(0);
            }
         }
      */

      return result;

   }

   AnsiString 
   DKIM::_SignHash(AnsiString &privateKey, AnsiString &canonicalizedHeader, HashCreator::HashType hashType)
   {
      // Sign the hash.
      BIO *private_bio = BIO_new_mem_buf(privateKey.GetBuffer(), -1);
      if(private_bio == NULL) 
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5309, "DKIM::_SignHash", "Unable to read the private key file into memory.");
         return "";
      }

      EVP_PKEY *private_key = PEM_read_bio_PrivateKey(private_bio, NULL, NULL, NULL);
      if(private_key == NULL) 
      {
         BIO_free(private_bio);
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5310, "DKIM::_SignHash", "Unable to parse the private key file.");
         return "";
      }
      BIO_free(private_bio);

      unsigned int siglen = EVP_PKEY_size(private_key);
      unsigned char *sig = (unsigned char*) OPENSSL_malloc(siglen);
      
      EVP_MD_CTX headerSigningContext;
      EVP_SignInit( &headerSigningContext, hashType == HashCreator::SHA256 ? EVP_sha256() : EVP_sha1());
      
      String result;

      if (EVP_SignUpdate( &headerSigningContext, canonicalizedHeader.GetBuffer(), canonicalizedHeader.GetLength() ) == 1)
      {
         if (EVP_SignFinal( &headerSigningContext, sig, &siglen, private_key) == 1)
         {
            result = Base64::Encode((const char*) sig, siglen);
         }
         else
         {
            ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5311, "DKIM::_SignHash", "Call to EVP_SignFinal failed.");
         }
      }
      else
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5312, "DKIM::_SignHash", "Call to EVP_SignUpdate failed.");
      }

      EVP_PKEY_free(private_key);
      EVP_MD_CTX_cleanup( &headerSigningContext );
      OPENSSL_free(sig);

      return result;
   }

   /*
      Returns one of the following
      Neutral - Undecided
      Pass - Signature verified properly.
      TempFail - Failed to verify signature, potentially a local problem.
      PermFail - Failed to verify signature.

   */
   DKIM::Result
   DKIM::Verify(const String &fileName)
   {
      if (FileUtilities::FileSize(fileName) > MaxFileSize)
         return Neutral;
      
      AnsiString messageHeader = PersistentMessage::LoadHeader(fileName);
      MimeHeader mimeHeader;
      mimeHeader.Load(messageHeader.GetBuffer(), messageHeader.GetLength(), false);

      vector<pair<AnsiString, AnsiString> > signatureFields = GetSignatureFields(mimeHeader);

      if (signatureFields.size() == 0)
      {
         // No signatures in message.
         return Neutral;
      }

      Result result = Neutral;

      typedef pair<AnsiString, AnsiString> HeaderField;
      boost_foreach (HeaderField signatureField, signatureFields)
      {
         result = _VerifySignature(fileName, messageHeader, signatureField);
         if (result == Pass)
            return Pass;
      };

      return result;


   }

   DKIM::Result 
   DKIM::_VerifySignature(const String &fileName, const AnsiString &messageHeader, pair<AnsiString, AnsiString> signatureField)
   {
      AnsiString headerValue = signatureField.second;

      // Unfold the value before trying to parse it. Otherwise it will contain
      // \t, \r\n which DKIMParameters doesn't take into account.
      MimeField::UnfoldField(headerValue);

      DKIMParameters signatureParams;
      signatureParams.Load (headerValue);

      if (!_ValidateHeaderContents(signatureParams))
      {
         // Skip this header.
         return Neutral;
      }

      shared_ptr<Canonicalization> headerCanonicalization;
      shared_ptr<Canonicalization> bodyCanonicalization;

      AnsiString method = signatureParams.GetValue("c");
      AnsiString headerMethod;
      AnsiString bodyMethod;

      if (method == "")
      {
         headerMethod = "simple";
         bodyMethod = "simple";
      }
      else
      {
         if (method.Find("/") > 0)
         {
            vector<AnsiString> vec = StringParser::SplitString(method, "/");    

            headerMethod = vec[0];
            bodyMethod = vec[1];
         }
         else
         {
            headerMethod = method;
            bodyMethod = "simple";
         }
      }

      if (headerMethod == "simple")
         headerCanonicalization = shared_ptr<SimpleCanonicalization>(new SimpleCanonicalization) ;
      else
         headerCanonicalization = shared_ptr<RelaxedCanonicalization>(new RelaxedCanonicalization) ;

      if (bodyMethod == "simple")
         bodyCanonicalization = shared_ptr<SimpleCanonicalization>(new SimpleCanonicalization) ;
      else
         bodyCanonicalization = shared_ptr<RelaxedCanonicalization>(new RelaxedCanonicalization) ;

      AnsiString publicKeyString;
      AnsiString flags;
      Result res = _RetrievePublicKey(signatureParams, publicKeyString, flags);
      if (res != Pass)
      {
         LOG_DEBUG("DKIM: Retrieval of public key failed.");
         return res;
      }

      bool testMode = flags.Find("y") >= 0;

      if (testMode)
      {
         LOG_DEBUG("DKIM: Domain is in test mode. Results of this signature test won't have any effect.");
      }

      if (!_ValidateBodyHash(fileName, signatureParams, bodyCanonicalization))
      {
         LOG_DEBUG("DKIM: Validation of body hash failed.");
         return testMode ? Pass : PermFail;
      }

      AnsiString tagH = signatureParams.GetValue("h");
      AnsiString tagA = signatureParams.GetValue("a");

      std::vector<AnsiString> headerFields = StringParser::SplitString(tagH,":");

      AnsiString fieldList;
      AnsiString canonicalizedHeader = headerCanonicalization->CanonicalizeHeader(messageHeader, signatureField, headerFields, fieldList);

      /*
         body-hash = hash-alg(canon_body)
         header-hash = hash-alg(canon_header || DKIM-SIG)
         signature = sig-alg(header-hash, key)
      */

      HashCreator shaer(tagA == "rsa-sha256" ? HashCreator::SHA256 : HashCreator::SHA1);
      AnsiString headerHash = shaer.GenerateHashNoSalt(canonicalizedHeader, HashCreator::base64);

      AnsiString tagB = signatureParams.GetValue("b");
      

      Result result = _VerifyHeaderHash(canonicalizedHeader, tagA, tagB, publicKeyString);

      return testMode ? Pass : result;
   }

   DKIM::Result
   DKIM::_VerifyHeaderHash(AnsiString canonicalizedHeader, const AnsiString &tagA, AnsiString &tagB, const AnsiString &publicKeyString)
   {
      Result result = PermFail;

      // base64 decode the public key.
      EVP_PKEY *publicKey = _GetPublicKey(publicKeyString);
      if (!publicKey)
      {
         // unable to extract public key from record. broken?
         LOG_DEBUG("DKIM: Unable to base64 decode public key found in DNS record. Key: " + publicKeyString);
         return result;
      }

      EVP_MD_CTX m_Hdr_ctx;
      EVP_MD_CTX_init( &m_Hdr_ctx );

      if (tagA == "rsa-sha256")
         EVP_VerifyInit( &m_Hdr_ctx, EVP_sha256() );
      else
         EVP_VerifyInit( &m_Hdr_ctx, EVP_sha1() );

      if (EVP_VerifyUpdate( &m_Hdr_ctx, canonicalizedHeader.GetBuffer(), canonicalizedHeader.GetLength() ) == 1)
      {
         // base64 decode the signature. we're working with binary
         // data here so we can't store it in a normal string. 
         MimeCodeBase64 encoder;
         encoder.SetInput(tagB.GetBuffer(), tagB.GetLength(), false);
         
         AnsiString signature;
         encoder.GetOutput(signature);

         if (EVP_VerifyFinal( &m_Hdr_ctx, (unsigned char *) signature.GetBuffer(), signature.GetLength(), publicKey) == 1)
         {
            LOG_DEBUG("DKIM: Message passed validation.");
            result = Pass;
         }
         else
         {
            LOG_DEBUG("DKIM: Header verification failed.");
         }
      }

      EVP_MD_CTX_cleanup( &m_Hdr_ctx );
      EVP_PKEY_free(publicKey);

      return result;
   }

   bool 
   DKIM::_ValidateBodyHash(const String &fileName, const DKIMParameters &signatureParams, shared_ptr<Canonicalization> canonicalization)
   {
      AnsiString tagA = signatureParams.GetValue("a");
      AnsiString tagBH = signatureParams.GetValue("bh");
      
      // Whitespace is ignored in this value and MUST be ignored when reassembling the original signature. 
      tagBH.Replace(" ", "");

      AnsiString messageBody = canonicalization->CanonicalizeBody(PersistentMessage::LoadBody(fileName));

      AnsiString tagBodyLengthCount = signatureParams.GetValue("l");
      if (!tagBodyLengthCount.IsEmpty())
      {
         if (!StringParser::IsNumeric(tagBodyLengthCount))
            return false;

         int trimmedBodyLength = atoi(tagBodyLengthCount);
         if (trimmedBodyLength > messageBody.GetLength())
            return false;

         messageBody = messageBody.Mid(0, trimmedBodyLength);
      }
      
      HashCreator shaer (tagA == "rsa-sha1" ? HashCreator::SHA1 : HashCreator::SHA256);
      AnsiString bodyHash = shaer.GenerateHashNoSalt(messageBody, HashCreator::base64);

      if (tagBH.IsEmpty() || tagBH.Compare(bodyHash) != 0)
         return false;

      return true;

   }

   bool
   DKIM::_ValidateHeaderContents(const DKIMParameters &signatureParams)
   {
      AnsiString tagH = signatureParams.GetValue("h");
      /*
         Verifiers MUST ignore DKIM-Signature header fields with a "v=" tag
         that is inconsistent with this specification and return PERMFAIL
         (incompatible version).
      */

      AnsiString tagV = signatureParams.GetValue("v");
      if (tagV != "1")
      {
         LOG_DEBUG("DKIM: Header in message incomplete. Unsupported version. Aborting DKIM test.");
         return false;
      }

      AnsiString tagQ = signatureParams.GetValue("q");
      if (tagQ != "" && tagQ != "dns/txt")
      {
         LOG_DEBUG("DKIM: Header in message incomplete. Unsupported query method. Aborting DKIM test.");
         return false; // unsupported method.
      }

      /*
         If any tag listed as "required" in Section 3.5 is omitted from the
         DKIM-Signature header field, the verifier MUST ignore the DKIM-
         Signature header field and return PERMFAIL (signature missing
         required tag).
      */

      AnsiString tagA = signatureParams.GetValue("a");

      if (tagA.IsEmpty()) return false;
      if (signatureParams.GetValue("b").IsEmpty()) return false;
      if (signatureParams.GetValue("bh").IsEmpty()) return false;
      if (signatureParams.GetValue("d").IsEmpty()) return false;
      if (tagH.IsEmpty()) return false;

      if (tagA != "rsa-sha1" && tagA != "rsa-sha256")
      {
         LOG_DEBUG("DKIM: Header in message incomplete. Unsupported algorithm. Aborting DKIM test.");
         return false;
      }

      /*
         Verifiers MUST confirm that the domain specified in the "d=" tag is
         the same as or a parent domain of the domain part of the "i=" tag.
         If not, the DKIM-Signature header field MUST be ignored and the
         verifier should return PERMFAIL (domain mismatch).
      */

      AnsiString tagD = signatureParams.GetValue("d");
      AnsiString tagI = signatureParams.GetValue("i");

      if (!tagI.IsEmpty())
      {
         AnsiString tagIDomain = StringParser::ExtractDomain(tagI);
         if (tagIDomain.CompareNoCase(tagD) != 0 && !tagIDomain.EndsWith("." + tagD))
         {
            String sMessage;
            sMessage.Format(_T("DKIM: Header in message incomplete. Tag I mismatch (%s - %s). Aborting DKIM test."), String(tagD), String(tagIDomain));
            LOG_DEBUG(sMessage);

            return false;
         }
      }

      /*
         If the "h=" tag does not include the From header field, the verifier
         MUST ignore the DKIM-Signature header field and return PERMFAIL (From
         field not signed).
      */

      bool found = false;
      vector<AnsiString> headerFields = StringParser::SplitString(tagH,":");
      boost_foreach(AnsiString headerField, headerFields)
      {
         headerField.Trim();
         headerField.ToLower();
         if (headerField == "from")
         {
            found = true;
            break;
         }
      }

      if (!found)
      {
         LOG_DEBUG("DKIM: Header in message incomplete. From field not found. Aborting DKIM test.");
         return false;
      }

      return true;
   }

   DKIM::Result
   DKIM::_RetrievePublicKey(const DKIMParameters &signatureParams, AnsiString &publicKey, AnsiString &flags)
   {
      // 6.1.2.  Get the Public Key
      AnsiString tagDomain = signatureParams.GetValue("d");
      AnsiString tagSelector = signatureParams.GetValue("s");
      AnsiString keyName = tagSelector + "._domainkey." + tagDomain;

      vector<String> results;
      DNSResolver resolver;
      if (!resolver.GetTXTRecords(keyName, results))
      {
         LOG_DEBUG("DKIM: Error when retrieving public key. Failed to do DNS/TXT lookup.");
         return TempFail;
      }

      if (results.size() == 0)
      {
         /*
            3.  If the query for the public key fails because the corresponding
            key record does not exist, the verifier MUST immediately return
            PERMFAIL (no key for signature).
         */

         LOG_DEBUG("DKIM: Error when retrieving public key. No key for signature.");
         return PermFail;
      }

      /* example:
         Line breaks won't actually exist.
         k=rsa; t=y; p=MHwwDQYJKoZIhvcNAQEBBQADawAwaAJhAOFzgIeFCw/TN5euR2O/oMHz+rv97OjqCxwt
                       Gk8BbiPnoNP3lYCF/147zz2B9gUWc9SFLAB1Dsrfd3yN5yiFdmK/KJ5ASv9oX0iNRJ9vGp
                       JyM2IRZ8qSOCeQscnre5iVjwIDAQAB;
      */
      
      AnsiString result = results[0];

      DKIMParameters dnsKeyParams;
      dnsKeyParams.Load(result);

      if (!_ValidateDNSEntry(dnsKeyParams, signatureParams))
      {
         LOG_DEBUG("DKIM: Error when retrieving public key. Validation of DNS entry failed.");
         return PermFail;
      }

      publicKey = dnsKeyParams.GetValue("p");

      // An empty value means that this public key has been revoked. 
      if (publicKey.IsEmpty())
      {
         LOG_DEBUG("DKIM: Error when retrieving public key. Public key has been revoked.");
         return PermFail;
      }

      flags = dnsKeyParams.GetValue("t");

      if (flags.Find("s") >= 0)
      {
         /*
            Flag: s
            Any DKIM-Signature header fields using the "i=" tag MUST have
            the same domain value on the right-hand side of the "@" in
            the "i=" tag and the value of the "d=" tag. 
         */
         
         AnsiString tagI = signatureParams.GetValue("i");

         if (!tagI.IsEmpty())
         {
            AnsiString tagD = signatureParams.GetValue("d");
            AnsiString tagIDomain = StringParser::ExtractDomain(tagI);
            if (tagIDomain.CompareNoCase(tagD) != 0)
            {
               
               String sMessage;
               sMessage.Format(_T("DKIM: Header in message incomplete. Tag I mismatch (%s - %s). Aborting."), String(tagD), String(tagIDomain));
               LOG_DEBUG(sMessage);

               return PermFail;
            }
         }
      }
      
      AnsiString allowedHashes = dnsKeyParams.GetValue("h");
      if (allowedHashes.GetLength() > 0)
      {
         AnsiString tagA = signatureParams.GetValue("a");

         AnsiString usedHash = tagA == "rsa-sha256" ? "sha256" : "sha1";

         if (allowedHashes.Find(usedHash) < 0)
         {
            LOG_DEBUG("DKIM: Error when retrieving public key. Hash not allowed: " + usedHash);
            return PermFail;
         }
      }

      return Pass;
   }

   bool 
   DKIM::_ValidateDNSEntry(const DKIMParameters &entryParams, const DKIMParameters &headerParams)
   {
      if (entryParams.GetParamCount() == 0)
         return false;

      AnsiString tagV = entryParams.GetValue("v");
      if (tagV != "" && tagV != "DKIM1")
         return false;

      AnsiString tagP = entryParams.GetValue("p");
      if (tagP == "")
         return false;

      /*
         If the "g=" tag in the public key does not match the Local-part
         of the "i=" tag in the message signature header field, the
         verifier MUST ignore the key record and return PERMFAIL
         (inapplicable key).
      */
      AnsiString tagI = headerParams.GetValue("i");
      AnsiString tagILocal = StringParser::ExtractAddress(tagI);
      
      if (entryParams.GetIsSet("g"))
      {
         AnsiString tagG = entryParams.GetValue("g");

         if (tagILocal.IsEmpty())
         {
            /*
               If the Local-part of the "i=" tag on the
               message signature is not present, the "g=" tag must be "*" (valid
               for all addresses in the domain) or the entire g= tag must be
               omitted (which defaults to "g=*")
            */

            if (tagG != "*")
               return false;
         }
         else
         {
            // case sensitive!
            if (!StringParser::WildcardMatch(tagG, tagILocal))
               return false;
         }
      }

      /*
         If the "h=" tag exists in the public key record and the hash
         algorithm implied by the a= tag in the DKIM-Signature header
         field is not included in the contents of the "h=" tag, the
         verifier MUST ignore the key record and return PERMFAIL
         (inappropriate hash algorithm).
      */
      AnsiString tagH = entryParams.GetValue("h");
      if (!tagH.IsEmpty())
      {
         AnsiString tagA = entryParams.GetValue("a");
         if (tagH.Find(tagH) < 0)
            return false;
      }

      return true;
   }

   shared_ptr<Canonicalization> 
   DKIM::_CreateCanonicalization(Canonicalization::CanonicalizeMethod method)
   {
      switch (method)
      {
      case Canonicalization::Simple:
         return shared_ptr<Canonicalization>(new SimpleCanonicalization);
      case Canonicalization::Relaxed:
         return shared_ptr<Canonicalization>(new RelaxedCanonicalization);
      }

      shared_ptr<Canonicalization> pEmpty;
      return pEmpty;
   }

   String 
   DKIM::_BuildSignatureHeader(const String &tagA, const String &tagD, const String &tagS, const String &tagC, const String &tagQ, const String &fieldList, const String &bodyHash, const String &signatureString)
   {
      String headerValue;

      if (signatureString.IsEmpty())
      {
         headerValue.Format(_T("v=1; a=%s; d=%s; s=%s;\r\n")
            _T("\tc=%s; q=%s; h=%s;\r\n")
            _T("\tbh=%s;\r\n")
            _T("\tb="), tagA, tagD, tagS, tagC, tagQ, String(fieldList), bodyHash);
      }
      else
      {
         String splitSignatureString;
         int lineLength = 250;
         for (int i = 0; i < signatureString.GetLength(); i += lineLength)
         {
            if (splitSignatureString.GetLength() > 0)
               splitSignatureString += "\r\n\t";

            splitSignatureString += signatureString.Mid(i, lineLength);
         }

         headerValue.Format(_T("v=1; a=%s; d=%s; s=%s;\r\n")
            _T("\tc=%s; q=%s; h=%s;\r\n")
            _T("\tbh=%s;\r\n")
            _T("\tb=%s"), tagA, tagD, tagS, tagC, tagQ, String(fieldList), bodyHash, splitSignatureString);
      }

      return headerValue;
   }

   vector<pair<AnsiString, AnsiString> > 
   DKIM::GetSignatureFields(MimeHeader &mimeHeader)
   {
      vector<pair<AnsiString, AnsiString>> result;
      vector<MimeField> &fields = mimeHeader.Fields();

      boost_foreach(MimeField f, fields)
      {
         AnsiString name = f.GetName();
         if (name.CompareNoCase("DKIM-Signature") == 0)
         {
            AnsiString headerValue = f.GetValue();
            result.push_back(std::make_pair(name, headerValue));

            if (result.size() >= 5)
               break;
         }
      };

      return result;
   }

}
