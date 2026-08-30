// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class PublicSuffixListTester
   {
   public:

      void Test();

   private:

      void TestRegistrableDomain_();
      void TestDomainsWithoutRegistrableDomain_();
   };
}
