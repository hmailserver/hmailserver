// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include ".\ExternalFetch.h"
#include "..\Common\BO\FetchAccount.h"
#include "../common/Util/Event.h"
#include "../Common/TCPIP/IOService.h"
#include "../Common/TCPIP/DNSResolver.h"
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
   ExternalFetch::Start(std::shared_ptr<FetchAccount> pFA)
   {  
      LOG_DEBUG(Formatter::Format("Retrieving messages from external account {0}", pFA->GetName()));
      
      std::shared_ptr<IOService> pIOService = Application::Instance()->GetIOService();

      std::shared_ptr<Event> disconnectEvent = std::shared_ptr<Event>(new Event()) ;
      std::shared_ptr<POP3ClientConnection> pClientConnection = std::shared_ptr<POP3ClientConnection> 
         (new POP3ClientConnection(pFA, 
                                   pFA->GetConnectionSecurity(), 
                                   pIOService->GetIOService(), 
                                   pIOService->GetClientContext(), 
                                   disconnectEvent,
                                   pFA->GetServerAddress()));

      DNSResolver resolver;

      std::vector<String> ip_addresses;
      resolver.GetIpAddresses(pFA->GetServerAddress(), ip_addresses, true);

      String ip_address;
      if (ip_addresses.size())
      {
         ip_address = *(ip_addresses.begin());
      }
      else
      {
         String error_message = Formatter::Format("The IP address for external account {0} could not be resolved. Aborting fetch.", pFA->GetName());
         LOG_APPLICATION(error_message);
         return;
      }

      if (pClientConnection->Connect(ip_address, pFA->GetPort(), IPAddress()))
      {
         // Make sure we keep no references to the TCP connection so that it
         // can be terminated whenever. We're longer own the connection.
         pClientConnection.reset();

         disconnectEvent->Wait();
      }

      LOG_DEBUG("Completed retrieval of messages from external account.");
   }


};