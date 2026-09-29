// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

#include "SPFRecord.h"

namespace HM
{
   class SPFDnsLookup;

   // Finds the SPF record a domain publishes and parses it: RFC 7208 sections 4.3 to
   // 4.5, everything an evaluation does before the first mechanism. Its own step
   // because an include and a redirect each do the same work again.
   class SPFRecordLocator
   {
   public:

      enum class Result
      {
         // The domain publishes no SPF record, or no query could be built from
         // the name. Section 4.3 makes both a "none" - the domain has said
         // nothing, which is different from having said the client is wrong.
         NoRecord = 0,

         // A single record, and it parses.
         Found = 1,

         // More than one record, which section 4.5 makes a permerror rather
         // than a matter of picking one.
         Ambiguous = 2,

         // A single record which does not parse: a permerror, section 4.6.
         SyntaxError = 3,

         // The lookup could not be answered. Section 4.4 makes this a
         // temperror, so the same message may get a different answer later.
         TemporaryError = 4
      };

      explicit SPFRecordLocator(std::shared_ptr<SPFDnsLookup> lookup);

      // Looks up domain and parses what it finds. On SyntaxError the error says
      // what was wrong with the record; it is empty otherwise.
      Result Locate(const AnsiString &domain, SPFRecord &record, AnsiString &error);

   private:

      std::shared_ptr<SPFDnsLookup> lookup_;
   };
}
