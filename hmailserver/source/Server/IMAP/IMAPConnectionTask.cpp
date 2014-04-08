#include "StdAfx.h"
#include "IMAPConnectionTask.h"
#include "IMAPConnection.h"

#include "..\Common\Application\ClientInfo.h"

#include "IMAPSocketServer.h"

namespace HM
{
   IMAPConnectionTask::IMAPConnectionTask(shared_ptr<ClientInfo> pClientInfo) :
      m_pClientInfo(pClientInfo)
   {
   }

   IMAPConnectionTask::~IMAPConnectionTask(void)
   {
   }

   void 
   IMAPConnectionTask::DoWork()
   {
      m_pIMAPConnection = shared_ptr<IMAPConnection> (new IMAPConnection());
      //m_pIMAPConnection->Init();

      //m_pIMAPConnection->HandleClient(m_pClientInfo);

      closesocket(m_pClientInfo->GetSocket());

      // Inform socket server that client has disconnected. 
      // We cannot be sure that the socket server still exists,
      // if the server is about to stop.
      shared_ptr<IMAPSocketServer> pSocketServer = Application::Instance()->GetIMAPSocketServer();
      if (pSocketServer)
         pSocketServer->OnDisconnected();
   }


   void 
   IMAPConnectionTask::StopWork()
   {
      //if (m_pIMAPConnection)
      //   m_pIMAPConnection->Stop();
   }
}