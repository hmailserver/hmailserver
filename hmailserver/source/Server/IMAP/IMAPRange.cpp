// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IMAPRange.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IMAPRange::IMAPRange() :
      start_(0),
      end_(0)
   {
      
   }

   void
   IMAPRange::SetRange(unsigned int start, unsigned int end)
   {
      start_ = start;
      end_ = end;
   }
}
