// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../BO/ScheduledTask.h"

namespace HM
{
   class RemoveExpiredRecords : public ScheduledTask
   {
   public:
      RemoveExpiredRecords(void);
      ~RemoveExpiredRecords(void);

      virtual void DoWork();

   private:
   };
}