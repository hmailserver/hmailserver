// Copyright (c) 2026 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "StdAfx.h"

#include "SRS.h"

#include "../../Common/Util/Encoding/Base64.h"

#include <openssl/crypto.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   namespace
   {
      // The character the fields of an address are separated by. Only the one directly
      // following the tag varies between implementations - '+' and '-' are as valid as
      // '=' there - so those are accepted on the way in, and '=' is what we write.
      const wchar_t FieldSeparator = '=';
      const wchar_t * const FieldSeparatorString = _T("=");

      // Base32, in the alphabet SRS uses for its day counter.
      const AnsiString TimestampCharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
      const int TimestampLength = 2;
      const int TimestampBits = 5;
      const int TimestampMask = (1 << TimestampBits) - 1;
      const int TimestampCycle = 1 << (TimestampBits * TimestampLength);
      const int SecondsPerDay = 60 * 60 * 24;

      // How far ahead of us the clock of the server which created an address may run
      // before the day it stamped into the address stops being one we recognize.
      const int ClockSkewTolerance = 60 * 60;
   }

   const int SRS::DefaultMaxAgeDays = 21;
   const int SRS::DefaultHashLength = 8;
   // Base64 has 64 characters, but a hash is compared without regard to case, so it is
   // really 38 of them a character can be. Eight of those are the 42 bits which make
   // guessing an address hopeless; four would be 21 bits, which is not out of reach for
   // someone willing to spend a few million RCPT TO commands on it.
   const int SRS::MinHashLength = 8;
   const int SRS::MaxHashLength = 20;
   const int SRS::MinMaxAgeDays = 1;
   // The day counter wraps after TimestampCycle days, and an address must not stay valid
   // long enough for the counter to catch up with itself.
   const int SRS::MaxMaxAgeDays = TimestampCycle / 2;
   // RFC 5321 section 4.5.3.1.3 limits a path to 256 octets, which leaves 254 characters
   // for the address inside it. This is the same limit StringParser enforces, so that an
   // address we create is one we will also accept when the bounce comes back.
   const int SRS::MaxAddressLength = 254;
   const int SRS::SecretLength = 32;

   SRS::SRS(const String &secret, int maxAgeDays, int hashLength) :
      max_age_days_(ClampMaxAgeDays(maxAgeDays)),
      hash_length_(ClampHashLength(hashLength))
   {
      // The secret goes into the hash as the bytes it is written in rather than as
      // whatever the code page of the machine happens to make of it. Two servers sharing
      // a database must arrive at the same key from the same secret, whichever code page
      // each of them runs under.
      if (!ToUTF8_(secret, secret_))
         secret_ = "";
   }

   SRS::~SRS()
   {

   }

   String
   SRS::Forward(const String &sender, const String &forwardingDomain) const
   {
      return Forward(sender, forwardingDomain, time(0));
   }

   String
   SRS::Forward(const String &sender, const String &forwardingDomain, time_t now) const
   {
      if (secret_.IsEmpty() || sender.IsEmpty() || forwardingDomain.IsEmpty())
         return "";

      String senderLocalPart;
      String senderDomain;

      if (!SplitAddress_(sender, senderLocalPart, senderDomain))
         return "";

      String localPart;

      int version = 0;
      String payload;

      if (ParseTag_(senderLocalPart, version, payload) && version == 0 && IsChainableSrs0Payload_(payload))
      {
         // The sender has been rewritten once already, by whoever forwarded it to us.
         // That server becomes the first hop of the address we create.
         localPart = BuildSrs1_(senderDomain, payload);
      }
      else if (version == 1)
      {
         // It has been rewritten at least twice. The first hop is the one to bounce
         // back to, so it is kept as it is and only the hash is replaced with ours.
         String hash;
         String firstHop;
         String srs0Payload;

         if (SplitSrs1Payload_(payload, hash, firstHop, srs0Payload) && IsChainableSrs0Payload_(srs0Payload))
            localPart = BuildSrs1_(firstHop, srs0Payload);
      }

      if (localPart.IsEmpty())
      {
         // Either the sender was never rewritten, or the tag is all it has in common with
         // an address one of us created - a mailing list posting as srs0-bounces@ is not
         // a rewritten sender, and chaining it would yield an address which reverses to
         // one that never existed, losing its bounces. It is rewritten as any other
         // sender is instead.
         localPart = BuildSrs0_(senderLocalPart, senderDomain, now);

         if (localPart.IsEmpty())
            return "";
      }

      String result = localPart + _T("@") + forwardingDomain;

      if (!StringParser::IsValidEmailAddress(result))
      {
         // The rewrite would produce an address the next server may well refuse, and one
         // our own RCPT TO validation would refuse the bounce for: too long, or built
         // around a local part which only an address in quotes may hold. Better to
         // forward with the sender we were given.
         return "";
      }

      return result;
   }

   SRS::ReverseResult
   SRS::Reverse(const String &address, String &originalAddress) const
   {
      return Reverse(address, time(0), originalAddress);
   }

   SRS::ReverseResult
   SRS::Reverse(const String &address, time_t now, String &originalAddress) const
   {
      String localPart;
      String domain;

      if (!SplitAddress_(address, localPart, domain))
         return ResultNotAnSrsAddress;

      int version = 0;
      String payload;

      if (!ParseTag_(localPart, version, payload))
         return ResultNotAnSrsAddress;

      if (secret_.IsEmpty())
         return ResultNotConfigured;

      if (version == 0)
      {
         String hash;
         String timestamp;
         String senderDomain;
         String senderLocalPart;

         if (!SplitSrs0Payload_(payload, hash, timestamp, senderDomain, senderLocalPart))
            return ResultMalformed;

         if (!ValidateHash_(timestamp + FieldSeparator + senderDomain + FieldSeparator + senderLocalPart, hash))
            return ResultInvalidHash;

         if (!ValidateTimestamp_(timestamp, now, max_age_days_))
            return ResultExpired;

         originalAddress = senderLocalPart + _T("@") + senderDomain;

         return ResultSuccess;
      }

      String hash;
      String firstHop;
      String srs0Payload;

      if (!SplitSrs1Payload_(payload, hash, firstHop, srs0Payload))
         return ResultMalformed;

      if (!ValidateHash_(firstHop + srs0Payload, hash))
         return ResultInvalidHash;

      // The embedded SRS0 payload was signed by us when we created the address, so its
      // timestamp is as much ours to go by as the one in an SRS0 address. Without this an
      // SRS1 address never expires, and a single forward triggered by an attacker yields
      // an address which relays mail to a domain of their choosing for good.
      String embeddedHash;
      String embeddedTimestamp;
      String embeddedDomain;
      String embeddedLocalPart;

      if (!SplitSrs0Payload_(srs0Payload, embeddedHash, embeddedTimestamp, embeddedDomain, embeddedLocalPart))
         return ResultMalformed;

      if (!ValidateTimestamp_(embeddedTimestamp, now, max_age_days_))
         return ResultExpired;

      originalAddress = String(_T("SRS0")) + srs0Payload + _T("@") + firstHop;

      return ResultSuccess;
   }

   bool
   SRS::IsSrsAddress(const String &address)
   {
      String localPart;
      String domain;

      if (!SplitAddress_(address, localPart, domain))
         return false;

      int version = 0;
      String payload;

      return ParseTag_(localPart, version, payload);
   }

   AnsiString
   SRS::GenerateSecret()
   {
      std::vector<unsigned char> buffer;
      buffer.resize((size_t) SecretLength);

      if (RAND_bytes(&buffer[0], (int) buffer.size()) != 1)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5730, "SRS::GenerateSecret",
            "Failed to generate a random SRS secret.");

         return "";
      }

      return Base64::EncodeUnpadded((const char*) &buffer[0], (int) buffer.size());
   }

   String
   SRS::GetResultDescription(ReverseResult result)
   {
      switch (result)
      {
      case ResultSuccess:
         return "The address was reversed.";
      case ResultNotAnSrsAddress:
         return "The address is not an SRS address.";
      case ResultMalformed:
         return "The SRS address is malformed.";
      case ResultInvalidHash:
         return "The SRS address has an invalid hash.";
      case ResultExpired:
         return "The SRS address has expired.";
      case ResultNotConfigured:
         return "No SRS secret has been configured.";
      }

      return "Unknown result.";
   }

   int
   SRS::ClampMaxAgeDays(int maxAgeDays)
   {
      if (maxAgeDays < MinMaxAgeDays)
         return MinMaxAgeDays;

      if (maxAgeDays > MaxMaxAgeDays)
         return MaxMaxAgeDays;

      return maxAgeDays;
   }

   int
   SRS::ClampHashLength(int hashLength)
   {
      if (hashLength < MinHashLength)
         return MinHashLength;

      if (hashLength > MaxHashLength)
         return MaxHashLength;

      return hashLength;
   }

   String
   SRS::BuildSrs0_(const String &senderLocalPart, const String &senderDomain, time_t now) const
   {
      String timestamp = CreateTimestamp_(now);

      String hash = CreateHash_(timestamp + FieldSeparator + senderDomain + FieldSeparator + senderLocalPart);

      if (hash.IsEmpty())
         return "";

      return String(_T("SRS0")) + FieldSeparator + hash +
                                  FieldSeparator + timestamp +
                                  FieldSeparator + senderDomain +
                                  FieldSeparator + senderLocalPart;
   }

   String
   SRS::BuildSrs1_(const String &firstHop, const String &srs0Payload) const
   {
      if (firstHop.IsEmpty() || srs0Payload.GetLength() < 2 || srs0Payload.GetAt(0) != FieldSeparator)
         return "";

      String hash = CreateHash_(firstHop + srs0Payload);

      if (hash.IsEmpty())
         return "";

      // srs0Payload opens with a separator of its own, which is where the double
      // separator in an SRS1 address comes from.
      return String(_T("SRS1")) + FieldSeparator + hash +
                                  FieldSeparator + firstHop +
                                  FieldSeparator + srs0Payload;
   }

   String
   SRS::CreateHash_(const String &data) const
   {
      return CreateFullHash_(data).Mid(0, hash_length_);
   }

   String
   SRS::CreateFullHash_(const String &data) const
   {
      // Everything hashed is lower-cased first. Mail servers are allowed to preserve the
      // case of a local part but not all of them do, and an address which comes back with
      // its case changed must still validate.
      String lowerCased = data;
      lowerCased.ToLower();

      AnsiString input;

      if (!ToUTF8_(lowerCased, input))
         return "";

      unsigned char digest[EVP_MAX_MD_SIZE];
      unsigned int digestLength = 0;

      if (HMAC(EVP_sha1(), secret_.c_str(), secret_.GetLength(),
               (const unsigned char*) input.c_str(), (size_t) input.GetLength(),
               digest, &digestLength) == 0)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5731, "SRS::CreateFullHash_",
            "Failed to calculate the SRS hash.");

         return "";
      }

      return Base64::EncodeUnpadded((const char*) digest, (int) digestLength);
   }

   bool
   SRS::ValidateHash_(const String &data, const String &hash) const
   {
      int length = hash.GetLength();

      // The hash was created at whatever length was configured when the address was handed
      // out, which is not necessarily the length configured now. Only as many characters as
      // the address carries are compared, so that changing the setting does not invalidate
      // every address already out there - but never fewer than the minimum, since a short
      // hash is an easy one to guess and the sender chooses how long the one they send is.
      if (length < MinHashLength || length > MaxHashLength)
         return false;

      String expected = CreateFullHash_(data);

      if (expected.GetLength() < length)
         return false;

      // Compared without regard to case, for the same reason the input is lower-cased
      // before it is hashed. The comparison takes the same time whether the first
      // character differs or the last one does, so that how long a rejection takes
      // tells the sender nothing about how much of the hash they got right.
      String expectedPrefix = expected.Mid(0, length);
      expectedPrefix.ToLower();

      String candidate = hash;
      candidate.ToLower();

      AnsiString expectedBytes;
      AnsiString candidateBytes;

      if (!ToUTF8_(expectedPrefix, expectedBytes) || !ToUTF8_(candidate, candidateBytes))
         return false;

      // A hash is Base64, so a candidate with characters outside ASCII cannot match. Its
      // length in bytes then differs from the expected one, which is public knowledge and
      // safe to bail out on early.
      if (expectedBytes.GetLength() != candidateBytes.GetLength())
         return false;

      return CRYPTO_memcmp(expectedBytes.c_str(), candidateBytes.c_str(), (size_t) expectedBytes.GetLength()) == 0;
   }

   bool
   SRS::SplitAddress_(const String &address, String &localPart, String &domain)
   {
      int position = address.ReverseFind('@');

      if (position <= 0)
         return false;

      localPart = address.Mid(0, position);
      domain = address.Mid(position + 1);

      return !localPart.IsEmpty() && !domain.IsEmpty();
   }

   bool
   SRS::ParseTag_(const String &localPart, int &version, String &payload)
   {
      // "SRS0" or "SRS1", a separator, and at least one character of content.
      if (localPart.GetLength() < 6)
         return false;

      String tag = localPart.Mid(0, 4);

      if (tag.CompareNoCase(_T("SRS0")) == 0)
         version = 0;
      else if (tag.CompareNoCase(_T("SRS1")) == 0)
         version = 1;
      else
         return false;

      wchar_t separator = localPart.GetAt(4);

      if (separator != '=' && separator != '+' && separator != '-')
         return false;

      // The payload keeps a leading separator, normalized to ours: an SRS1 address
      // embeds the payload of the SRS0 address it was created from, separator and all.
      payload = String(FieldSeparatorString) + localPart.Mid(5);

      return true;
   }

   bool
   SRS::SplitSrs0Payload_(const String &payload, String &hash, String &timestamp, String &domain, String &localPart)
   {
      if (payload.GetLength() < 2 || payload.GetAt(0) != FieldSeparator)
         return false;

      String remainder = payload.Mid(1);

      int position = remainder.Find(FieldSeparator);
      if (position <= 0)
         return false;

      hash = remainder.Mid(0, position);
      remainder = remainder.Mid(position + 1);

      position = remainder.Find(FieldSeparator);
      if (position <= 0)
         return false;

      timestamp = remainder.Mid(0, position);
      remainder = remainder.Mid(position + 1);

      position = remainder.Find(FieldSeparator);
      if (position <= 0)
         return false;

      domain = remainder.Mid(0, position);

      // Whatever is left is the local part of the original address, separators included:
      // it is the last field, so it is not split any further.
      localPart = remainder.Mid(position + 1);

      return !localPart.IsEmpty();
   }

   bool
   SRS::SplitSrs1Payload_(const String &payload, String &hash, String &firstHop, String &srs0Payload)
   {
      if (payload.GetLength() < 2 || payload.GetAt(0) != FieldSeparator)
         return false;

      String remainder = payload.Mid(1);

      int position = remainder.Find(FieldSeparator);
      if (position <= 0)
         return false;

      hash = remainder.Mid(0, position);
      remainder = remainder.Mid(position + 1);

      position = remainder.Find(FieldSeparator);
      if (position <= 0)
         return false;

      firstHop = remainder.Mid(0, position);

      // The embedded SRS0 payload, which starts with a separator of its own.
      srs0Payload = remainder.Mid(position + 1);

      if (srs0Payload.GetLength() < 2 || srs0Payload.GetAt(0) != FieldSeparator)
         return false;

      return true;
   }

   bool
   SRS::IsChainableSrs0Payload_(const String &payload)
   {
      // Whether the payload really holds the fields an SRS0 address carries. How long the
      // hash is, is not part of it: other implementations write one as short as four
      // characters, and an address one of them created must still chain.
      String hash;
      String timestamp;
      String domain;
      String localPart;

      if (!SplitSrs0Payload_(payload, hash, timestamp, domain, localPart))
         return false;

      int day = 0;

      return ParseTimestamp_(timestamp, day);
   }

   bool
   SRS::ToUTF8_(const String &input, AnsiString &output)
   {
      output = "";

      int inputLength = input.GetLength();

      if (inputLength == 0)
         return true;

      int requiredLength = WideCharToMultiByte(CP_UTF8, 0, input.c_str(), inputLength, NULL, 0, NULL, NULL);

      if (requiredLength <= 0)
         return false;

      std::vector<char> buffer;
      buffer.resize((size_t) requiredLength);

      if (WideCharToMultiByte(CP_UTF8, 0, input.c_str(), inputLength, &buffer[0], requiredLength, NULL, NULL) != requiredLength)
         return false;

      output.assign(&buffer[0], (size_t) requiredLength);

      return true;
   }

   AnsiString
   SRS::CreateTimestamp_(time_t now)
   {
      __int64 days = (__int64) (now / SecondsPerDay);

      AnsiString result;
      result += TimestampCharacters.GetAt((int) ((days >> TimestampBits) & TimestampMask));
      result += TimestampCharacters.GetAt((int) (days & TimestampMask));

      return result;
   }

   bool
   SRS::ParseTimestamp_(const String &timestamp, int &day)
   {
      if (timestamp.GetLength() != TimestampLength)
         return false;

      day = 0;

      for (int i = 0; i < TimestampLength; i++)
      {
         wchar_t character = timestamp.GetAt(i);

         if (character >= 'a' && character <= 'z')
            character = character - 'a' + 'A';

         int index = -1;

         for (int j = 0; j < TimestampCharacters.GetLength(); j++)
         {
            if (TimestampCharacters.GetAt(j) == (char) character)
            {
               index = j;
               break;
            }
         }

         if (index < 0)
            return false;

         day = (day << TimestampBits) | index;
      }

      return true;
   }

   bool
   SRS::ValidateTimestamp_(const String &timestamp, time_t now, int maxAgeDays)
   {
      int then = 0;

      if (!ParseTimestamp_(timestamp, then))
         return false;

      // A server whose clock runs a little ahead of ours stamps an address with tomorrow's
      // day in the last moments of today. Our own clock is moved forward by the tolerance
      // to cover that, rather than accepting any address which looks a day old less than
      // the whole cycle: modulo the cycle, one day in the future and TimestampCycle - 1
      // days in the past are the same timestamp, and the latter must not be let through.
      int today = (int) (((now + ClockSkewTolerance) / SecondsPerDay) % TimestampCycle);

      // The day counter wraps, so the age is calculated modulo the cycle: a timestamp
      // which looks like it lies far in the future is really one from before the wrap.
      int age = (today - then) % TimestampCycle;

      if (age < 0)
         age += TimestampCycle;

      return age <= maxAgeDays;
   }
}
