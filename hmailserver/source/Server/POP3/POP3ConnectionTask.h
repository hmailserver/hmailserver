#pragma once

#include "..\Common\Threading\Task.h"

namespace HM
{
   class ClientInfo;
   class POP3Connection;

   class POP3ConnectionTask : public Task
   {
   public:
      POP3ConnectionTask(shared_ptr<ClientInfo> pClientInfo);
      ~POP3ConnectionTask(void);

      virtual void DoWork();
      virtual void StopWork();

   private:

      shared_ptr<ClientInfo> m_pClientInfo;
      shared_ptr<POP3Connection> m_pPOP3Connection;
   };
}