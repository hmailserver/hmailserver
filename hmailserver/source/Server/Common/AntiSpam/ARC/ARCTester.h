// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class ARCTester
   {
   public:

      void Test();

   private:

      void TestInstanceParsing_();
      void TestAuthenticationResults_();
      void TestChainValidation_();
   };
}
