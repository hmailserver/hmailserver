// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

namespace HM
{
   // An IP address in the forms an SPF evaluation needs it in: the client
   // address the check is made for, and the literal an ip4 or ip6 mechanism
   // carries.
   //
   // Not a replacement for HM::IPAddress and not a second one. IPAddress stays
   // the server's address type; this exists because SPF asks three things of an
   // address which IPAddress does not answer:
   //
   //   - The i macro of RFC 7208 section 7.2 wants an IPv6 address written as 32
   //     dotted nibbles, and the c macro wants the shortened form of RFC 5952.
   //     IPAddress::ToLongString produces neither.
   //   - The address literals of section 12 are spelled more strictly than a
   //     resolver would insist on. SPFSyntax already holds that grammar, and
   //     what this parses is what SPFSyntax accepts, so a record and the
   //     evaluation of it agree about which literals exist.
   //   - The record parser, this, and the macro expander are built by the
   //     portable build as well as by the server, so that they can be run under
   //     the address and undefined-behaviour sanitizers and fuzzed. IPAddress
   //     cannot be: it is built on Boost.Asio.
   class SPFAddress
   {
   public:

      enum class Family
      {
         // A default-constructed address, and what TryParse leaves behind when
         // it fails. Matches nothing.
         None,
         IP4,
         IP6
      };

      SPFAddress();

      // Reads one of the address literals of RFC 7208 section 12. Returns false
      // if the text is not one.
      //
      // An IPv4-mapped IPv6 address - ::ffff:192.0.2.1 - becomes the IPv4
      // address it carries. The mechanisms of sections 5.6 and 5.7 select by
      // family, so a client which reached the server over IPv4 has to be treated
      // as IPv4 however the address was written.
      static bool TryParse(const AnsiString &text, SPFAddress &address);

      Family GetFamily() const { return family_; }

      // The i macro of section 7.2. An IPv4 address in dotted-quad form; an IPv6
      // address as its 32 nibbles, most significant first, separated by dots,
      // which is what makes %{ir} the reverse-mapping name.
      //
      // The hex digits are upper case. Neither RFC 7208 nor DNS cares - section
      // 4.3 compares names without regard to case - but the conformance suite's
      // v-macro-ip6 puts a reverse-mapping name in an explanation and compares
      // the text of it, and the suite writes it in upper case.
      AnsiString GetDottedForm() const;

      // The c macro of section 7.2: the address in the form a person would read.
      // Dotted-quad for IPv4; for IPv6 the shortened form of RFC 5952, in lower
      // case, with the longest run of zero groups replaced by "::".
      AnsiString GetReadableForm() const;

      // The v macro of section 7.2: "in-addr" for IPv4 and "ip6" for IPv6, the
      // label under .arpa which the family's reverse mapping lives under.
      AnsiString GetArpaLabel() const;

      // The reverse-mapping name, which is what %{ir}.%{v}.arpa expands to:
      // 40.218.168.192.in-addr.arpa, or the nibbles followed by ip6.arpa.
      //
      // Built here rather than where it is needed, because it is needed twice -
      // the ptr mechanism of section 5.5 and the p macro of section 7.3 both
      // look it up - and an implementation which built it in two places could
      // build it two ways.
      AnsiString GetReverseName() const;

      // Whether the two addresses agree over the first prefixLength bits, which
      // is how sections 5.3, 5.4, 5.6 and 5.7 decide a match. False between
      // families: an ip4 mechanism never matches an IPv6 client, whatever the
      // addresses look like.
      bool MatchesPrefix(const SPFAddress &other, int prefixLength) const;

      // Whether the two are the same address. This is what validates a name the
      // ptr mechanism found, section 5.5.
      bool Equals(const SPFAddress &other) const;

      // How many bits an address of this family has, and so the prefix length a
      // mechanism which names none gets.
      int GetBitCount() const;

   private:

      Family family_;

      // Network order, most significant byte first. Four bytes are used for
      // IPv4 and all sixteen for IPv6; the rest are zero.
      unsigned char bytes_[16];
   };
}
