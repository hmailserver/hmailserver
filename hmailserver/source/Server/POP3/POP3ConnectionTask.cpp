#include "StdAfx.h"
#include ".\pop3connectiontask.h"
#include "POP3Connection.h"

#include "..\Common\Application\ClientInfo.h"

#include "POP3SocketServer.h"

namespace HM
{
   POP3ConnectionTask::POP3ConnectionTask(shared_ptr<ClientInfo> pClientInfo) :
      m_pClientInfo(pClientInfo)
   {


   }

   POP3ConnectionTask::~POP3ConnectionTask(void)
   {

   }

   void 
   POP3ConnectionTask::DoWork()
   {
      m_pPOP3Connection = shared_ptr<POP3Connection> (new POP3Connection());
      //m_pPOP3Connection->HandleClient(m_pClientInfo);

      closesocket(m_pClientInfo->GetSocket());

      // Inform socket server that client has disconnected. 
      // We cannot be sure that the socket server still exists,
      // if the server is about to stop.
      shared_ptr<POP3SocketServer> pSocketServer = Application::Instance()->GetPOP3SocketServer();
      if (pSocketServer)
         pSocketServer->OnDisconnected();
   }

   void 
   POP3ConnectionTask::StopWork()
   {
      //if (m_pPOP3Connection)
     //    m_pPOP3Connection->Stop();
   }

}