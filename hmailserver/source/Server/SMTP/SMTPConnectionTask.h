// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "..\Common\Threading\Task.h"

namespace HM
{
   class ClientInfo;
   class SMTPConnection;


   class SMTPConnectionTask : public Task
   {
   public:
      SMTPConnectionTask(shared_ptr<ClientInfo> pClientInfo);
      ~SMTPConnectionTask(void);

      virtual void DoWork();
      virtual void StopWork();
   private:

      shared_ptr<ClientInfo> m_pClientInfo;
      shared_ptr<SMTPConnection> m_pSMTPConnection;
   };
}