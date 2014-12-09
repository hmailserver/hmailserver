// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "Test64Bit.h"


#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   Test64Bit::Test64Bit()
   {

   }

   Test64Bit::~Test64Bit()
   {

   }

   DiagnosticResult
   Test64Bit::PerformTest()
   {
      DiagnosticResult diagResult;
      diagResult.SetName("Test IPv6");
      diagResult.SetDescription("Checks whether 64-bit version is running...");

#ifdef _WIN64
      diagResult.SetDetails("64-bit version of hMailServer is running.");
#else
      diagResult.SetDetails("32-bit version of hMailServer is running.");
#endif

      diagResult.SetSuccess(true);

      return diagResult;
   }


   
      
}
