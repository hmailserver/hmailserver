// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

namespace HM
{
   // An IP address in the forms an SPF evaluation needs it in. Not a replacement
   // for IPAddress, which answers none of them - see README.md.
   class SPFAddress
   {
   public:

      enum class Family
      {
         // Default-constructed, or what TryParse leaves on failure. Matches nothing.
         None,
         IP4,
         IP6
      };

      SPFAddress();

      // One of the address literals of RFC 7208 section 12. An IPv4-mapped IPv6
      // address becomes the IPv4 it carries: sections 5.6 and 5.7 select by the
      // family the client connected over.
      static bool TryParse(const AnsiString &text, SPFAddress &address);

      Family GetFamily() const { return family_; }

      // The i macro of section 7.2: dotted-quad, or 32 dotted nibbles for IPv6.
      // Upper case hex, which shows only in an explanation - the suite's
      // v-macro-ip6 compares the text.
      AnsiString GetDottedForm() const;

      // The c macro of section 7.2: dotted-quad, or the shortened form of RFC 5952.
      AnsiString GetReadableForm() const;

      // The v macro of section 7.2: "in-addr" for IPv4 and "ip6" for IPv6, the
      // label under .arpa which the family's reverse mapping lives under.
      AnsiString GetArpaLabel() const;

      // What %{ir}.%{v}.arpa expands to. Here rather than at its two callers - the
      // ptr mechanism and the p macro - so that it is spelled one way.
      AnsiString GetReverseName() const;

      // Whether the two agree over the first prefixLength bits, which is how
      // sections 5.3 to 5.7 decide a match. Never true between families.
      bool MatchesPrefix(const SPFAddress &other, int prefixLength) const;

      // The same address. Validates a name the ptr mechanism found, section 5.5.
      bool Equals(const SPFAddress &other) const;

      // The family's bit count, and so the default prefix length.
      int GetBitCount() const;

   private:

      Family family_;

      // Network order. Four bytes for IPv4, sixteen for IPv6; the rest zero.
      unsigned char bytes_[16];
   };
}
