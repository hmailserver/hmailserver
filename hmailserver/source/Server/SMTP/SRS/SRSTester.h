// Copyright (c) 2026 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

#include "SRS.h"

namespace HM
{
   class SRSTester
   {
   public:
      void Test();

   private:
      void TestForwardFormat_();
      void TestRoundTrip_();
      void TestCaseHandling_();
      void TestSeparators_();
      void TestTampering_();
      void TestMalformedAddresses_();
      void TestTimestamps_();
      void TestChaining_();
      void TestLimits_();
      void TestSecretGeneration_();

      static SRS CreateSRS_();
      static void AssertTrue_(bool value);
      static void AssertEqual_(const String &expected, const String &actual);
      static void AssertReverse_(const SRS &srs, const String &address, SRS::ReverseResult expectedResult);
   };
}
