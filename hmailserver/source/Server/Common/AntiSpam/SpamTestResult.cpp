/* Copyright (c) Martin Knafve / The hMailServer Community Developers (HCD) hMailServer.com */

#include <stdafx.h>

#include "SpamTestResult.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   SpamTestResult::SpamTestResult(const String &testName, Result result, int iSpamScore, const String &sMessage) :
      spam_score_(iSpamScore),
      message_(sMessage),
      testName_(testName),
      result_(result)
   {

   }

   SpamTestResult::~SpamTestResult(void)
   {

   }

}