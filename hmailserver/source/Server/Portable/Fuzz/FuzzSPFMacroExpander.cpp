// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

// libFuzzer entry point for the macro expansion of RFC 7208 section 7. The input is
// split, because the expansion depends on the sender and the HELO argument as much
// as on the macro-string - README.md says how, and how to run this.

#include "stdafx.h"

#include "../../SMTP/SPF/SPFAddress.h"
#include "../../SMTP/SPF/SPFMacroExpander.h"
#include "../../SMTP/SPF/SPFTestLookup.h"

#include <stddef.h>
#include <stdint.h>

namespace
{
   // Reads a length-prefixed run of bytes off the front of the input. Returns an empty
   // string once there is nothing left, which is a case worth reaching: an empty sender
   // and an empty HELO argument are both things a client can send.
   HM::AnsiString TakeField(const uint8_t *&data, size_t &size)
   {
      if (size == 0)
         return "";

      size_t wanted = data[0];
      data++;
      size--;

      if (wanted > size)
         wanted = size;

      HM::AnsiString field((const char *) data, (int) wanted);

      data += wanted;
      size -= wanted;

      return field;
   }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
   HM::AnsiString clientText = TakeField(data, size);
   HM::AnsiString sender = TakeField(data, size);
   HM::AnsiString heloHost = TakeField(data, size);
   HM::AnsiString domain = TakeField(data, size);

   // What is left is the macro-string, which is the part being fuzzed.
   HM::AnsiString text((const char *) data, (int) size);

   HM::SPFAddress clientAddress;

   if (!HM::SPFAddress::TryParse(clientText, clientAddress))
   {
      // Most inputs will not be an address. Half the runs use a fixed one so
      // that the macros which read the client - i, c, v and p - are reached
      // rather than always seeing an address of no family.
      HM::SPFAddress::TryParse((size % 2) ? "192.0.2.1" : "cafe:babe::1", clientAddress);
   }

   // A resolver holding one reverse mapping, so the p macro of section 7.3 does the work
   // it would do in the server: a PTR lookup, then an address lookup to validate what
   // came back. Without it the macro would always expand to "unknown".
   auto lookup = std::make_shared<HM::SPFTestLookup>();

   lookup->AddPTR(clientAddress.GetReverseName(), "mail.example.com");
   lookup->AddA("mail.example.com", "192.0.2.1");
   lookup->AddAAAA("mail.example.com", "cafe:babe::1");

   HM::SPFMacroExpander expander(lookup, clientAddress, sender, heloHost);

   expander.SetReceivingHost("receiver.example.com");
   expander.SetTimestamp(1234567890);

   HM::AnsiString expanded;

   // Both entry points, because they differ in which macros they allow, in
   // whether a space is a literal, and in whether the result is shortened to the
   // length of a domain name.
   if (expander.ExpandDomainSpec(text, domain, expanded))
   {
      // Section 7.1 shortens a name that came out too long, so what it produced
      // has to be a name a query could be built from.
      (void) expanded.GetLength();
   }

   (void) expander.ExpandExplanation(text, domain, expanded);

   return 0;
}
