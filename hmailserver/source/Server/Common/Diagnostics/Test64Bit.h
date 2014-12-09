// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "DiagnosticResult.h"

namespace HM
{

   class Test64Bit
   {
   public:
      Test64Bit();
      virtual ~Test64Bit();

      DiagnosticResult PerformTest();

   private:
   };


}
