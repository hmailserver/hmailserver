// Copyright (c) 2026 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "StdAfx.h"

#include "SRS.h"

#include "../../Common/Util/Encoding/Base64.h"

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

   SRS::SRS(const AnsiString &secret, int maxAgeDays, int hashLength) :
      secret_(secret),
      max_age_days_(maxAgeDays),
      hash_length_(hashLength)
   {
      if (max_age_days_ < MinMaxAgeDays)
         max_age_days_ = MinMaxAgeDays;
      else if (max_age_days_ > MaxMaxAgeDays)
         max_age_days_ = MaxMaxAgeDays;

      if (hash_length_ < MinHashLength)
         hash_length_ = MinHashLength;
      else if (hash_length_ > MaxHashLength)
         hash_length_ = MaxHashLength;
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

      if (ParseTag_(senderLocalPart, version, payload))
      {
         if (version == 0)
         {
            // The sender has been rewritten once already, by whoever forwarded it to us.
            // That server becomes the first hop of the address we create.
            localPart = BuildSrs1_(senderDomain, payload);
         }
         else
         {
            // It has been rewritten at least twice. The first hop is the one to bounce
            // back to, so it is kept as it is and only the hash is replaced with ours.
            String hash;
            String firstHop;
            String srs0Payload;

            if (!SplitSrs1Payload_(payload, hash, firstHop, srs0Payload))
               return "";

            localPart = BuildSrs1_(firstHop, srs0Payload);
         }
      }
      else
      {
         localPart = BuildSrs0_(senderLocalPart, senderDomain, now);
      }

      if (localPart.IsEmpty())
         return "";

      String result = localPart + _T("@") + forwardingDomain;

      if (result.GetLength() > MaxAddressLength)
      {
         // Rewriting it would produce an address the next server may well refuse, and
         // one we could not store. Better to forward with the sender we were given.
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

      // The timestamp in the address belongs to the first hop, which is the server that
      // created it and the only one which can tell whether it is still valid. Ours is
      // the hash, and that is what we check.
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
      // before it is hashed.
      return expected.Mid(0, length).CompareNoCase(hash) == 0;
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
   SRS::ValidateTimestamp_(const String &timestamp, time_t now, int maxAgeDays)
   {
      if (timestamp.GetLength() != TimestampLength)
         return false;

      int then = 0;

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

         then = (then << TimestampBits) | index;
      }

      int today = (int) ((now / SecondsPerDay) % TimestampCycle);

      // The day counter wraps, so the age is calculated modulo the cycle: a timestamp
      // which looks like it lies far in the future is really one from before the wrap.
      int age = (today - then) % TimestampCycle;

      if (age < 0)
         age += TimestampCycle;

      // Except by a single day, which is the clock difference between us and the server
      // that created the address rather than an address from almost three years ago.
      if (age == TimestampCycle - 1)
         return true;

      return age <= maxAgeDays;
   }
}
