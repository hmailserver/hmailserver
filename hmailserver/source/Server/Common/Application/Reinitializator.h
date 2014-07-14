#pragma once

#include "../Util/Event.h"

#include <boost/thread/thread.hpp>

namespace HM
{
   class Reinitializator : public Singleton<Reinitializator>
   {
   public:
      Reinitializator(void);
      ~Reinitializator(void);

     
      void ReInitialize();

   private:

      void WorkerFunc();

      boost::thread _workerThread;

      bool _isRunning;
   };
}