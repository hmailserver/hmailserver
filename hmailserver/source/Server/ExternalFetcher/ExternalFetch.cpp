// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include ".\ExternalFetch.h"
#include "..\Common\BO\FetchAccount.h"
#include "..\Common\BO\Account.h"
#include "..\Common\BO\SecurityRange.h"
#include "../common/Util/Event.h"
#include "../Common/Util/Utilities.h"
#include "../Common/TCPIP/IOCPServer.h"
#include "../Common/Cache/CacheContainer.h"
#include "../common/TCPIP/TCPConnection.h"
#include "POP3ClientConnection.h"


#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   ExternalFetch::ExternalFetch(void)
   {
      
   }

   ExternalFetch::~ExternalFetch(void)
   {

   }

   void 
   ExternalFetch::Start(shared_ptr<FetchAccount> pFA)
   {  
      LOG_DEBUG("ExternalFetch::Start");
      
      shared_ptr<IOCPServer> pIOCPServer = Application::Instance()->GetIOCPServer();

      boost::asio::ssl::context ctx(pIOCPServer->GetIOService(), boost::asio::ssl::context::sslv23);

      shared_ptr<Event> disconnectEvent = shared_ptr<Event>(new Event()) ;
      shared_ptr<POP3ClientConnection> pClientConnection = shared_ptr<POP3ClientConnection> (new POP3ClientConnection(pFA, pFA->GetUseSSL() ? CSSSL : CSNone, pIOCPServer->GetIOService(), ctx, disconnectEvent));

      try
      {
         if (pClientConnection->Connect(pFA->GetServerAddress(), pFA->GetPort(), IPAddress()))
         {
            // Make sure we keep no references to the TCP connection so that it
            // can be terminated whenever. We're longer own the connection.
            pClientConnection.reset();

            disconnectEvent->Wait();
         }
         
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5077, "ExternalFetch::Start", "Start() threw an unhandled exception.");
      }

      LOG_DEBUG("ExternalFetch::~Start");
   }


};