// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "stdafx.h"

#include "SPFRecordLocator.h"

#include "SPFDnsLookup.h"
#include "SPFSyntax.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   SPFRecordLocator::SPFRecordLocator(std::shared_ptr<SPFDnsLookup> lookup) :
      lookup_(lookup)
   {

   }

   SPFRecordLocator::Result
   SPFRecordLocator::Locate(const AnsiString &domain, SPFRecord &record, AnsiString &error)
   {
      error = "";

      // Section 4.3: a name no query can be built from produces no result at
      // all, rather than an error. Asking anyway would be asking about a
      // different name than the one the message carried.
      if (!SPFSyntax::IsValidDomainName(domain))
         return Result::NoRecord;

      std::vector<AnsiString> textRecords;

      if (!lookup_->GetTXTRecords(domain, textRecords))
         return Result::TemporaryError;

      // Section 4.5: of everything the domain publishes as TXT, the records beginning
      // with the SPF version are the candidates. The rest are none of an SPF check's
      // business, so a TXT record that is not even ASCII leaves the SPF one usable.
      std::vector<AnsiString> candidates;

      for (size_t i = 0; i < textRecords.size(); i++)
      {
         if (SPFRecord::HasVersionTag(textRecords[i]))
            candidates.push_back(textRecords[i]);
      }

      if (candidates.empty())
         return Result::NoRecord;

      if (candidates.size() > 1)
         return Result::Ambiguous;

      if (!SPFRecord::Parse(candidates[0], record, error))
         return Result::SyntaxError;

      return Result::Found;
   }
}
