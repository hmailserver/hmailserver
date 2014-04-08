// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include ".\smtpconnectiontask.h"
#include "SMTPConnection.h"

#include "..\Common\Application\ClientInfo.h"

#include "SMTPSocketServer.h"

namespace HM
{
   SMTPConnectionTask::SMTPConnectionTask(shared_ptr<ClientInfo> pClientInfo) :
      m_pClientInfo(pClientInfo)
   {

   }

   SMTPConnectionTask::~SMTPConnectionTask(void)
   {
   }


   void 
   SMTPConnectionTask::DoWork()
   {
      m_pSMTPConnection = shared_ptr<SMTPConnection> (new SMTPConnection());
      //m_pSMTPConnection->Init();
      //m_pSMTPConnection->HandleClient(m_pClientInfo);

      closesocket(m_pClientInfo->GetSocket());

      // Inform socket server that client has disconnected. 
      // We cannot be sure that the socket server still exists,
      // if the server is about to stop.
      shared_ptr<SMTPSocketServer> pSocketServer = Application::Instance()->GetSMTPSocketServer();
      if (pSocketServer)
         pSocketServer->OnDisconnected();

   }

   void 
   SMTPConnectionTask::StopWork()
   {
      //if (m_pSMTPConnection)
      //   m_pSMTPConnection->Stop();
   }
}
