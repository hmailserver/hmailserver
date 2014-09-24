// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "StackWalker.h"

namespace HM
{
   class CustomStackWalker : public StackWalker
   {
   public:
	   CustomStackWalker();

      AnsiString GetStackData() {return stack_data_; }
   protected:
      virtual void OnOutput(LPCSTR szText);
   private:

      AnsiString stack_data_;
   };
}
