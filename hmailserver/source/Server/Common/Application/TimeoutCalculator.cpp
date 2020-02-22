// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com


#include "stdafx.h"
#include "TimeoutCalculator.h"

#include "SessionManager.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   TimeoutCalculator::TimeoutCalculator()
   {

   }

   

   int
   TimeoutCalculator::Calculate(int connectionCount, int minSecs, int maxSecs)
   {
      double loadPercentage = ((double) connectionCount / (double) MaxConnectionCountOptimized) * 100;

      double newTimeout = maxSecs;

      // If the load is too high, decrease the timeout to 60 seconds.
      if (loadPercentage >= 70)
         newTimeout = 60;

      // If we're below the minimal value, increase slightly.
      if (newTimeout < minSecs)
      {
         String message;
         message.Format(_T("Connection count has reached threshold. Count: %d, MinSecs: %d, MaxSecs: %d"), connectionCount, minSecs, maxSecs);
         LOG_DEBUG(message);

         newTimeout = minSecs;
      }  

      return (int) newTimeout;
   }

   int
   TimeoutCalculator::Calculate(int minSecs, int maxSecs)
   {
      int connectionCount = SessionManager::Instance()->GetNumberOfConnections();

      return Calculate(connectionCount, minSecs, maxSecs);
   }


   void
   TimeoutCalculatorTester::Test()
   {
      TimeoutCalculator calc;
      
      // Basic test. Since the connection count is so low, the default timeout should be used.
      int result = calc.Calculate(10, 5, 1800); // 30 minutes

      if (result != 1800)
         throw;

      // Basic test. Since the connection count equals to the threshold, the default timeout should be used.
      result = calc.Calculate(10000, 100, 1800); // 30 minutes
      result = calc.Calculate(11000, 5 * 30, 30 * 60); // 30 minutes
      result = calc.Calculate(8000, 100, 1800); // 30 minutes
      result = calc.Calculate(9000, 100, 1800); // 30 minutes
      result = calc.Calculate(10000, 100, 1800); // 30 minutes
      result = calc.Calculate(11000, 100, 1800); // 30 minutes

      result = calc.Calculate(10000, 1, 1800); // 30 minutes

      result = calc.Calculate(11000, 600, 1800); // 30 minutes

   }
}

