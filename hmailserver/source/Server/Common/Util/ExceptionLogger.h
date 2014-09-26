// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "CustomStackWalker.h"

namespace HM
{
   class ExceptionLogger
   {
   private:
      ExceptionLogger();

   public:
      static void Log(int exception_code, EXCEPTION_POINTERS* pExp);

   private:

      static void CreateMiniDump_(EXCEPTION_POINTERS* pExp, const String &file_name);

      static CustomStackWalker stack_walker_;
      static boost::mutex stack_walker_mutex_;
   };
}