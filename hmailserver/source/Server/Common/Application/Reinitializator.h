#pragma once

#include "../Util/Event.h"

namespace HM
{
   class Reinitializator : public Singleton<Reinitializator>
   {
   public:
      Reinitializator(void);
      ~Reinitializator(void);

      void ThreadFunc();
      void ReInitialize();

   private:

      HANDLE m_hThreadHandle;

      Event m_evtReinitRequest;
      Event m_evtStopRequest;
   };
}