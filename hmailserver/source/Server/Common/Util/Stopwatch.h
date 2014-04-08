// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Stopwatch
   {
   public:
	   Stopwatch();
      Stopwatch(bool start);
	   virtual ~Stopwatch();

      void Start();
      void Stop();

      DWORD GetElapsedMilliseconds();
      DWORD GetElapsedSeconds();

   private:

      void _Initialize();

      DWORD _startTickCount;
      DWORD _stopTickCount;
   };


}