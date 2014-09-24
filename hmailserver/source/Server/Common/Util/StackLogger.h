// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "CustomStackWalker.h"

namespace HM
{
   class StackLogger
   {
   private:
      StackLogger();

   public:
      static void Log(int exception_code, const CONTEXT *context);

   private:

      static CustomStackWalker stack_walker_;
      static boost::mutex stack_walker_mutex_;
   };
}