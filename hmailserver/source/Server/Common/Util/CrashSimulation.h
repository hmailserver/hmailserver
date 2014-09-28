// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class CrashSimulation
   {
   public:

      static void Execute(int simulation_mode);

   private:
      CrashSimulation(void);

    
   };
}