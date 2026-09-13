// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

#include "SPFDnsLookup.h"

namespace HM
{
   // A resolver over a table the test at hand writes. Test-only: nothing the
   // server runs constructs one.
   //
   // Separate from the conformance suite's ConformanceLookup, which answers out
   // of the vendored zone data and is shaped by what that file means. This is for
   // the cases the suite does not reach - a lookup that fails, a zone built to
   // cost an exact number of queries - where writing three records is clearer
   // than finding a section of someone else's suite that happens to hold them.
   //
   // Names are folded to lower case on the way in and on the way out, which is
   // how DNS compares them, RFC 4343.
   class SPFTestLookup : public SPFDnsLookup
   {
   public:

      SPFTestLookup();

      void AddTXT(const AnsiString &domain, const AnsiString &record);
      void AddA(const AnsiString &host, const AnsiString &address);
      void AddAAAA(const AnsiString &host, const AnsiString &address);

      // In the order they are added, which is the order a resolver hands them
      // over in: SPFDnsLookup asks for MX records in preference order.
      void AddMX(const AnsiString &domain, const AnsiString &hostName);

      // The reverse-mapping name rather than an address, which is what
      // SPFDnsLookup::GetPTRRecords takes.
      void AddPTR(const AnsiString &reverseName, const AnsiString &hostName);

      // A name whose queries cannot be answered, for the paths a server failure
      // or a timeout takes. Records added for it are still handed back, so that a
      // caller which ignored the failure is seen to.
      void Fail(const AnsiString &name);

      virtual bool GetTXTRecords(const AnsiString &domain, std::vector<AnsiString> &records);
      virtual bool GetARecords(const AnsiString &host, std::vector<AnsiString> &addresses);
      virtual bool GetAAAARecords(const AnsiString &host, std::vector<AnsiString> &addresses);
      virtual bool GetMXRecords(const AnsiString &domain, std::vector<AnsiString> &hostNames);
      virtual bool GetPTRRecords(const AnsiString &reverseName, std::vector<AnsiString> &hostNames);

      // How many queries have been answered, so that a test can say a record
      // cost what it should.
      int GetQueryCount() const { return query_count_; }

   private:

      static AnsiString Fold_(const AnsiString &name);

      bool Answer_(const std::map<AnsiString, std::vector<AnsiString> > &table,
                   const AnsiString &name, std::vector<AnsiString> &values);

      std::map<AnsiString, std::vector<AnsiString> > txt_;
      std::map<AnsiString, std::vector<AnsiString> > a_;
      std::map<AnsiString, std::vector<AnsiString> > aaaa_;
      std::map<AnsiString, std::vector<AnsiString> > mx_;
      std::map<AnsiString, std::vector<AnsiString> > ptr_;

      std::set<AnsiString> failing_;

      int query_count_;
   };
}
