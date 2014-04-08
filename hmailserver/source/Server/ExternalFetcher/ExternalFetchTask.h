// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "..\Common\Threading\Task.h"

namespace HM
{
   class FetchAccount;

   class ExternalFetchTask : public Task
   {
   public:
      ExternalFetchTask(shared_ptr<FetchAccount> pFA);
      ~ExternalFetchTask(void);

      virtual void DoWork();
      virtual void StopWork();
   private:

      shared_ptr<FetchAccount> m_pFA;
   };
}