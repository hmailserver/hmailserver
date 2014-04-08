// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "DiagnosticResult.h"

namespace HM
{

   class TestIPv6
   {
   public:
	   TestIPv6();
	   virtual ~TestIPv6();

      DiagnosticResult PerformTest();

   private:
   };


}
