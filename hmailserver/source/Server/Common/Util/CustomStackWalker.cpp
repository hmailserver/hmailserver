// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "CustomStackWalker.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   CustomStackWalker::CustomStackWalker()
   {

   }

   void
   CustomStackWalker::OnOutput(LPCSTR szText)
   {
      stack_data_.append(szText);
      stack_data_.append("\r\n");
   } 
}
