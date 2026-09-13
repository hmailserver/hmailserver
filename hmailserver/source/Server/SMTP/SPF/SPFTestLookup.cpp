// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "stdafx.h"

#include "SPFTestLookup.h"

#include "SPFSyntax.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   SPFTestLookup::SPFTestLookup() :
      query_count_(0)
   {

   }

   void
   SPFTestLookup::AddTXT(const AnsiString &domain, const AnsiString &record)
   {
      txt_[Fold_(domain)].push_back(record);
   }

   void
   SPFTestLookup::AddA(const AnsiString &host, const AnsiString &address)
   {
      a_[Fold_(host)].push_back(address);
   }

   void
   SPFTestLookup::AddAAAA(const AnsiString &host, const AnsiString &address)
   {
      aaaa_[Fold_(host)].push_back(address);
   }

   void
   SPFTestLookup::AddMX(const AnsiString &domain, const AnsiString &hostName)
   {
      mx_[Fold_(domain)].push_back(hostName);
   }

   void
   SPFTestLookup::AddPTR(const AnsiString &reverseName, const AnsiString &hostName)
   {
      ptr_[Fold_(reverseName)].push_back(hostName);
   }

   void
   SPFTestLookup::Fail(const AnsiString &name)
   {
      failing_.insert(Fold_(name));
   }

   bool
   SPFTestLookup::GetTXTRecords(const AnsiString &domain, std::vector<AnsiString> &records)
   {
      return Answer_(txt_, domain, records);
   }

   bool
   SPFTestLookup::GetARecords(const AnsiString &host, std::vector<AnsiString> &addresses)
   {
      return Answer_(a_, host, addresses);
   }

   bool
   SPFTestLookup::GetAAAARecords(const AnsiString &host, std::vector<AnsiString> &addresses)
   {
      return Answer_(aaaa_, host, addresses);
   }

   bool
   SPFTestLookup::GetMXRecords(const AnsiString &domain, std::vector<AnsiString> &hostNames)
   {
      return Answer_(mx_, domain, hostNames);
   }

   bool
   SPFTestLookup::GetPTRRecords(const AnsiString &reverseName, std::vector<AnsiString> &hostNames)
   {
      return Answer_(ptr_, reverseName, hostNames);
   }

   AnsiString
   SPFTestLookup::Fold_(const AnsiString &name)
   {
      AnsiString folded;

      for (int i = 0; i < name.GetLength(); i++)
         folded += SPFSyntax::ToLowerAscii(name[i]);

      return folded;
   }

   bool
   SPFTestLookup::Answer_(const std::map<AnsiString, std::vector<AnsiString> > &table,
                          const AnsiString &name, std::vector<AnsiString> &values)
   {
      query_count_++;
      values.clear();

      if (name.IsEmpty())
      {
         // What DNSResolver does with one: there is no query to make, so it
         // reports a failure. Modelled here because an expansion that comes out
         // empty is the way an evaluation reaches it.
         return false;
      }

      AnsiString folded = Fold_(name);

      auto found = table.find(folded);

      if (found != table.end())
         values = found->second;

      // A name which is not in the table exists as far as this resolver is
      // concerned and holds no record of the type asked for, which is an answer
      // rather than a failure. A failing name hands back whatever was added for
      // it: SPFDnsLookup promises nothing about the vector when it returns false,
      // and a caller which read it anyway would look right against a resolver
      // that emptied it.
      return failing_.find(folded) == failing_.end();
   }
}
