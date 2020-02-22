// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include ".\heapchecker.h"

#ifdef _DEBUG
   #define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
   #define new DEBUG_NEW
#endif

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   HeapChecker::HeapChecker(void)
   {
#ifdef _DEBUG
      _CrtMemCheckpoint( &start_ );
#endif
   }

   HeapChecker::~HeapChecker(void)
   {
   }

   
   void
   HeapChecker::CheckHeapOnAllocation()
   {
#ifdef _DEBUG
      int tmp = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

      tmp = tmp | _CRTDBG_CHECK_ALWAYS_DF;

      // Set the new bits
      _CrtSetDbgFlag(tmp);
#endif

   }

   void 
   HeapChecker::Reset()
   {
#ifdef _DEBUG
       _CrtMemCheckpoint( &start_ );
#endif
   }

   void
   HeapChecker::Report()
   {
#ifdef _DEBUG
      _CrtMemState now_;
      _CrtMemCheckpoint( &now_ );

      _CrtMemState diff_;
      if (_CrtMemDifference(&diff_, &start_, &now_))
      {
         // Memory leaks was found. Report them now.
         _CrtMemDumpAllObjectsSince(&start_);
         _CrtMemDumpStatistics(&diff_);

         throw 0; // memory leak

      }
#endif
   }


}