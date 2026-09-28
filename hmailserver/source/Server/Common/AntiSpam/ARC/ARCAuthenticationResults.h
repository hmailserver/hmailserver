// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

#include "../DMARC/DMARCRecord.h"

namespace HM
{
   // Reads the results a sealer recorded in its ARC-Authentication-Results, which use
   // the Authentication-Results syntax of RFC 8601.
   class ARCAuthenticationResults
   {
   public:

      struct MethodResult
      {
         AnsiString method;
         AnsiString result;
         std::map<AnsiString, AnsiString> properties;
      };

      // Parses the results that follow the authserv-id. Comments are dropped, and method,
      // result and property names are lowercased.
      static std::vector<MethodResult> Parse(const AnsiString &results);

      // True if the sealer saw the original message pass DMARC for headerFromDomain, or
      // saw an SPF or DKIM pass for a domain aligned with it.
      static bool ShowsAlignedPass(const AnsiString &results, const String &headerFromDomain,
                                   DMARCRecord::Alignment spfAlignment, DMARCRecord::Alignment dkimAlignment);

   private:

      static AnsiString RemoveComments_(const AnsiString &value);
      static std::vector<AnsiString> Split_(const AnsiString &value, char separator);
      static AnsiString Unquote_(AnsiString value);
   };
}
