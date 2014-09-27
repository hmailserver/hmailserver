// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include "EventTester.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   void
   EventTester::Test()
   {
      TestSetBeforeWait();
      TestWaitTimeoutNotSet();
      TestWaitTimeoutSet();
   }

   void
   EventTester::TestSetBeforeWait()
   {
      Event ev;
      ev.Set();
      ev.Wait();
   }

   void
   EventTester::TestWaitTimeoutNotSet()
   {
      Event ev;
      ev.WaitFor(boost::chrono::milliseconds(1));
   }

   void
   EventTester::TestWaitTimeoutSet()
   {
      Event ev;
      ev.Set();
      ev.WaitFor(boost::chrono::milliseconds(1));
   }

}