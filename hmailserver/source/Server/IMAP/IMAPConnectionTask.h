#pragma once

#include "..\Common\Threading\Task.h"

namespace HM
{
   class ClientInfo;
   class IMAPConnection;

   class IMAPConnectionTask : public Task
   {
   public:
      IMAPConnectionTask(shared_ptr<ClientInfo> pClientInfo);
      ~IMAPConnectionTask(void);

      virtual void DoWork();
      virtual void StopWork();

   private:

      shared_ptr<ClientInfo> m_pClientInfo;
      shared_ptr<IMAPConnection> m_pIMAPConnection;
   };
}