// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

// libFuzzer entry point for the SPF record parser, whose input is a TXT record
// published by whichever domain a message claims to be from. README.md says what
// this adds to the suites beside it, and how to run it.

#include "stdafx.h"

#include "../../SMTP/SPF/SPFRecord.h"
#include "../../SMTP/SPF/SPFSyntax.h"

#include <stddef.h>
#include <stdint.h>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
   // Length-counted rather than NUL-terminated, because a TXT record may hold a
   // NUL and the parser has to be able to reject one rather than stop at it.
   HM::AnsiString text((const char *) data, (int) size);

   HM::SPFRecord record;
   HM::AnsiString error;

   if (HM::SPFRecord::Parse(text, record, error))
   {
      // A record that parsed has to be readable, which is what an evaluation
      // does with it. Walking it is what would notice a term left holding a
      // dangling reference or a length nobody set.
      for (const HM::SPFMechanism &mechanism : record.GetMechanisms())
      {
         (void) mechanism.GetType();
         (void) mechanism.GetQualifier();
         (void) mechanism.GetDomainSpec().GetLength();
         (void) mechanism.GetAddress().GetLength();
         (void) mechanism.GetIP4PrefixLength();
         (void) mechanism.GetIP6PrefixLength();
      }

      (void) record.GetRedirectDomainSpec().GetLength();
      (void) record.GetExplanationDomainSpec().GetLength();
   }

   // The version check reads the same bytes by a different route, and is what
   // picks an SPF record out of a domain's other TXT records.
   (void) HM::SPFRecord::HasVersionTag(text);

   // The rules the parser is built on, over the same input, so that a crash in
   // one of them is found here rather than only through a record that reaches it.
   (void) HM::SPFSyntax::IsAsciiPrintable(text);
   (void) HM::SPFSyntax::IsValidDomainSpec(text);
   (void) HM::SPFSyntax::IsValidDomainName(text);
   (void) HM::SPFSyntax::IsValidIP4Network(text);
   (void) HM::SPFSyntax::IsValidIP6Network(text);
   (void) HM::SPFSyntax::IsValidModifierName(text);
   (void) HM::SPFSyntax::IsValidMacroString(text, HM::SPFSyntax::MacroSet::RecordTerm);
   (void) HM::SPFSyntax::IsValidMacroString(text, HM::SPFSyntax::MacroSet::ExplanationText);

   return 0;
}
