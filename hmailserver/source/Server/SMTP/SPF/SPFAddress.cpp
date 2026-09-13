// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "stdafx.h"

#include "SPFAddress.h"

#include "SPFSyntax.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   namespace
   {
      const char *UPPER_HEX_DIGITS = "0123456789ABCDEF";
      const char *LOWER_HEX_DIGITS = "0123456789abcdef";

      int HexValue(char character)
      {
         if (character >= '0' && character <= '9')
            return character - '0';

         if (character >= 'a' && character <= 'f')
            return character - 'a' + 10;

         if (character >= 'A' && character <= 'F')
            return character - 'A' + 10;

         return -1;
      }

      std::vector<AnsiString> Split(const AnsiString &text, char separator)
      {
         std::vector<AnsiString> parts;
         AnsiString current;

         for (int i = 0; i < text.GetLength(); i++)
         {
            if (text[i] == separator)
            {
               parts.push_back(current);
               current = "";
               continue;
            }

            current += text[i];
         }

         parts.push_back(current);

         return parts;
      }

      // The four octets of a dotted-quad. The text is known to be one, because
      // SPFSyntax has already said so.
      void ReadIP4(const AnsiString &text, unsigned char *bytes)
      {
         std::vector<AnsiString> octets = Split(text, '.');

         for (int i = 0; i < (int) octets.size() && i < 4; i++)
         {
            int value = 0;

            for (int at = 0; at < octets[i].GetLength(); at++)
               value = value * 10 + (octets[i][at] - '0');

            bytes[i] = (unsigned char) value;
         }
      }

      // The 16-bit groups of one side of an IPv6 address. A group written as a
      // dotted-quad stands for two of them, which is what the last group of an
      // address of the form ::ffff:192.0.2.1 is.
      void ReadIP6Groups(const AnsiString &side, std::vector<unsigned short> &groups)
      {
         if (side.IsEmpty())
         {
            // An empty side of a "::" contributes no groups. Splitting it would
            // produce one empty group instead.
            return;
         }

         std::vector<AnsiString> written = Split(side, ':');

         for (int i = 0; i < (int) written.size(); i++)
         {
            if (written[i].Find('.') >= 0)
            {
               unsigned char quad[4] = { 0, 0, 0, 0 };

               ReadIP4(written[i], quad);

               groups.push_back((unsigned short) ((quad[0] << 8) | quad[1]));
               groups.push_back((unsigned short) ((quad[2] << 8) | quad[3]));
               continue;
            }

            int value = 0;

            for (int at = 0; at < written[i].GetLength(); at++)
               value = (value << 4) | HexValue(written[i][at]);

            groups.push_back((unsigned short) value);
         }
      }

      // The sixteen bytes of an IPv6 address which SPFSyntax has accepted.
      // Returns false only for an address whose groups do not add up, which
      // SPFSyntax should already have rejected.
      bool ReadIP6(const AnsiString &text, unsigned char *bytes)
      {
         AnsiString before = text;
         AnsiString after;
         bool compressed = false;

         int doubleColon = text.Find("::");

         if (doubleColon >= 0)
         {
            compressed = true;
            before = text.Left(doubleColon);
            after = text.Mid(doubleColon + 2);
         }

         std::vector<unsigned short> head;
         std::vector<unsigned short> tail;

         ReadIP6Groups(before, head);
         ReadIP6Groups(after, tail);

         int written = (int) (head.size() + tail.size());

         if (written > 8 || (!compressed && written != 8))
            return false;

         for (int i = 0; i < 16; i++)
            bytes[i] = 0;

         for (int i = 0; i < (int) head.size(); i++)
         {
            bytes[i * 2] = (unsigned char) (head[i] >> 8);
            bytes[i * 2 + 1] = (unsigned char) (head[i] & 0xFF);
         }

         // The groups written after the "::" sit at the end of the address; the
         // zeros the "::" stands for are the gap between the two runs.
         int offset = 8 - (int) tail.size();

         for (int i = 0; i < (int) tail.size(); i++)
         {
            bytes[(offset + i) * 2] = (unsigned char) (tail[i] >> 8);
            bytes[(offset + i) * 2 + 1] = (unsigned char) (tail[i] & 0xFF);
         }

         return true;
      }

      // RFC 4291 section 2.5.5.2: eighty zero bits, then sixteen one bits, then
      // the IPv4 address.
      bool IsIP4Mapped(const unsigned char *bytes)
      {
         for (int i = 0; i < 10; i++)
         {
            if (bytes[i] != 0)
               return false;
         }

         return bytes[10] == 0xFF && bytes[11] == 0xFF;
      }

      AnsiString ToDecimal(unsigned int value)
      {
         // Written from the least significant digit, then turned around: an
         // octet is three digits at most, so there is nothing to be gained from
         // being cleverer.
         char digits[12];
         int count = 0;

         do
         {
            digits[count++] = (char) ('0' + (value % 10));
            value /= 10;
         }
         while (value > 0 && count < (int) sizeof(digits));

         AnsiString text;

         while (count > 0)
            text += digits[--count];

         return text;
      }

      // One 16-bit group of an IPv6 address. RFC 5952 section 4.1: lower case,
      // and no leading zeros.
      AnsiString WriteGroup(unsigned short group)
      {
         AnsiString text;

         for (int shift = 12; shift >= 0; shift -= 4)
         {
            int nibble = (group >> shift) & 0x0F;

            if (nibble == 0 && text.IsEmpty() && shift > 0)
               continue;

            text += LOWER_HEX_DIGITS[nibble];
         }

         return text;
      }

      AnsiString ToDottedQuad(const unsigned char *bytes)
      {
         AnsiString text;

         for (int i = 0; i < 4; i++)
         {
            if (i > 0)
               text += ".";

            text += ToDecimal(bytes[i]);
         }

         return text;
      }
   }

   SPFAddress::SPFAddress()
      : family_(Family::None)
   {
      for (int i = 0; i < 16; i++)
         bytes_[i] = 0;
   }

   bool
   SPFAddress::TryParse(const AnsiString &text, SPFAddress &address)
   {
      address = SPFAddress();

      if (SPFSyntax::IsValidIP4Network(text))
      {
         ReadIP4(text, address.bytes_);
         address.family_ = Family::IP4;

         return true;
      }

      if (!SPFSyntax::IsValidIP6Network(text))
         return false;

      unsigned char bytes[16];

      if (!ReadIP6(text, bytes))
         return false;

      if (IsIP4Mapped(bytes))
      {
         for (int i = 0; i < 4; i++)
            address.bytes_[i] = bytes[12 + i];

         address.family_ = Family::IP4;

         return true;
      }

      for (int i = 0; i < 16; i++)
         address.bytes_[i] = bytes[i];

      address.family_ = Family::IP6;

      return true;
   }

   int
   SPFAddress::GetBitCount() const
   {
      switch (family_)
      {
      case Family::IP4:
         return 32;

      case Family::IP6:
         return 128;

      case Family::None:
         break;
      }

      return 0;
   }

   AnsiString
   SPFAddress::GetDottedForm() const
   {
      if (family_ == Family::IP4)
         return ToDottedQuad(bytes_);

      if (family_ != Family::IP6)
         return "";

      AnsiString text;

      for (int i = 0; i < 16; i++)
      {
         if (i > 0)
            text += ".";

         text += UPPER_HEX_DIGITS[bytes_[i] >> 4];
         text += '.';
         text += UPPER_HEX_DIGITS[bytes_[i] & 0x0F];
      }

      return text;
   }

   AnsiString
   SPFAddress::GetReadableForm() const
   {
      if (family_ == Family::IP4)
         return ToDottedQuad(bytes_);

      if (family_ != Family::IP6)
         return "";

      unsigned short groups[8];

      for (int i = 0; i < 8; i++)
         groups[i] = (unsigned short) ((bytes_[i * 2] << 8) | bytes_[i * 2 + 1]);

      // RFC 5952 section 4.2: the longest run of zero groups is written as "::",
      // the leftmost of them where two are the same length, and a run of one is
      // left as it is.
      int bestStart = -1;
      int bestLength = 0;

      for (int i = 0; i < 8; i++)
      {
         if (groups[i] != 0)
            continue;

         int length = 0;

         while (i + length < 8 && groups[i + length] == 0)
            length++;

         if (length > bestLength)
         {
            bestStart = i;
            bestLength = length;
         }

         i += length - 1;
      }

      if (bestLength < 2)
      {
         // RFC 5952 section 4.2.2: a run of a single zero group is written out.
         bestStart = -1;
         bestLength = 0;
      }

      // The groups before the run and the groups after it, with "::" standing
      // for the run between them. Written this way round because the "::" is one
      // token rather than a separator either side owns: "::1" and "cafe::" have
      // groups on only one side of it, and "::" has none on either.
      AnsiString text;

      int runEnd = (bestStart < 0) ? 8 : bestStart;

      for (int i = 0; i < runEnd; i++)
      {
         if (i > 0)
            text += ':';

         text += WriteGroup(groups[i]);
      }

      if (bestStart < 0)
         return text;

      text += "::";

      for (int i = bestStart + bestLength; i < 8; i++)
      {
         if (i > bestStart + bestLength)
            text += ':';

         text += WriteGroup(groups[i]);
      }

      return text;
   }

   AnsiString
   SPFAddress::GetArpaLabel() const
   {
      switch (family_)
      {
      case Family::IP4:
         return "in-addr";

      case Family::IP6:
         return "ip6";

      case Family::None:
         break;
      }

      return "";
   }

   AnsiString
   SPFAddress::GetReverseName() const
   {
      if (family_ == Family::None)
         return "";

      // The same parts the i macro produces, in the other order, which is what
      // %{ir} is.
      std::vector<AnsiString> parts = Split(GetDottedForm(), '.');

      AnsiString name;

      for (int i = (int) parts.size(); i > 0; i--)
      {
         name += parts[i - 1];
         name += ".";
      }

      name += GetArpaLabel();
      name += ".arpa";

      return name;
   }

   bool
   SPFAddress::MatchesPrefix(const SPFAddress &other, int prefixLength) const
   {
      if (family_ == Family::None || family_ != other.family_)
         return false;

      if (prefixLength < 0 || prefixLength > GetBitCount())
         return false;

      int wholeBytes = prefixLength / 8;

      for (int i = 0; i < wholeBytes; i++)
      {
         if (bytes_[i] != other.bytes_[i])
            return false;
      }

      int remainingBits = prefixLength % 8;

      if (remainingBits == 0)
         return true;

      unsigned char mask = (unsigned char) (0xFF << (8 - remainingBits));

      return (bytes_[wholeBytes] & mask) == (other.bytes_[wholeBytes] & mask);
   }

   bool
   SPFAddress::Equals(const SPFAddress &other) const
   {
      return MatchesPrefix(other, GetBitCount());
   }
}
