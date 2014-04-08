// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "Stopwatch.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   Stopwatch::Stopwatch()
   {
      _Initialize();
   }

   Stopwatch::Stopwatch(bool start)
   {
      _Initialize();

      Start();
   }


   Stopwatch::~Stopwatch()
   {

   }

   void 
   Stopwatch::_Initialize()
   {
      _startTickCount = 0;
      _stopTickCount = 0;
   }

   void
   Stopwatch::Start() 
   {
      _startTickCount = GetTickCount();
   }

   void
   Stopwatch::Stop()
   {
      _stopTickCount = GetTickCount();
   }

   DWORD 
   Stopwatch::GetElapsedMilliseconds()
   {
      DWORD stopTickCount = _stopTickCount;

      if (stopTickCount == 0)
         stopTickCount = GetTickCount();

      if (stopTickCount < _startTickCount)
         return 0;

      return stopTickCount - _startTickCount;
   }

   DWORD
   Stopwatch::GetElapsedSeconds()
   {
      return GetElapsedMilliseconds() / 1000;
   }
   
}
