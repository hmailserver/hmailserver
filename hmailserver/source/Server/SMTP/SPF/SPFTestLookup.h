// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

#include "SPFDnsLookup.h"

namespace HM
{
   // A resolver over a table the test writes. Test-only. Separate from the
   // conformance suite's, which answers out of the vendored zone data.
   // Names are folded to lower case, as DNS compares them.
   class SPFTestLookup : public SPFDnsLookup
   {
   public:

      SPFTestLookup();

      void AddTXT(const AnsiString &domain, const AnsiString &record);
      void AddA(const AnsiString &host, const AnsiString &address);
      void AddAAAA(const AnsiString &host, const AnsiString &address);

      // In the order added, which is the preference order GetMXRecords promises.
      void AddMX(const AnsiString &domain, const AnsiString &hostName);

      // The reverse-mapping name, which is what GetPTRRecords takes.
      void AddPTR(const AnsiString &reverseName, const AnsiString &hostName);

      // A name whose queries fail. Records added for it are still handed back, so a
      // caller that ignored the failure is seen to.
      void Fail(const AnsiString &name);

      virtual bool GetTXTRecords(const AnsiString &domain, std::vector<AnsiString> &records);
      virtual bool GetARecords(const AnsiString &host, std::vector<AnsiString> &addresses);
      virtual bool GetAAAARecords(const AnsiString &host, std::vector<AnsiString> &addresses);
      virtual bool GetMXRecords(const AnsiString &domain, std::vector<AnsiString> &hostNames);
      virtual bool GetPTRRecords(const AnsiString &reverseName, std::vector<AnsiString> &hostNames);

      // So that a test can say a record cost the queries it should.
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
