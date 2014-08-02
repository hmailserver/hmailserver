// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "Cacheable.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   Cacheable::Cacheable(void)
   {
      creation_time_ = GetTickCount();
   }

   Cacheable::~Cacheable(void)
   {
   }

   int 
   Cacheable::Seconds()
   {
      int iCurrentTime = GetTickCount();

      // Convert from milliseconds to seconds.
      int iAge = (iCurrentTime - creation_time_) / 1000;

      if (iAge < 0)
         iAge = 60*60*24;

      return iAge;
   }
}