// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "SpamAssassinTestConnect.h"
#include "SpamAssassinClient.h"

#include "../../TCPIP/IOService.h"
#include "../../TCPIP/TCPConnection.h"
#include "../../TCPIP/DNSResolver.h"
#include "../../Util/FileUtilities.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   bool
   SpamAssassinTestConnect::TestConnect(const String &hostName, int port, String &message)
   {
     
      String bodyText = 
         "From: SpamAssassinTest@example.com\r\n"
         "\r\n"
         "XJS*C4JDBQADN1.NSBN3*2IDNEN*GTUBE-STANDARD-ANTI-UBE-TEST-EMAIL*C.34X.\r\n";

      String tempFile = FileUtilities::GetTempFileName();
      FileUtilities::WriteToFile(tempFile, bodyText, false);

      std::shared_ptr<IOService> pIOService = Application::Instance()->GetIOService();

      bool testCompleted;

      std::shared_ptr<Event> disconnectEvent = std::shared_ptr<Event>(new Event());
      std::shared_ptr<SpamAssassinClient> pSAClient = std::shared_ptr<SpamAssassinClient>(new SpamAssassinClient(tempFile, pIOService->GetIOService(), pIOService->GetClientContext(), disconnectEvent, testCompleted));

      DNSResolver resolver;

      std::vector<String> ip_addresses;
      resolver.GetIpAddresses(hostName, ip_addresses, true);

      String ip_address;
      if (ip_addresses.size())
      {
         ip_address = *(ip_addresses.begin());
      }
      else
      {
         message = "The IP address for SpamAssassin could not be resolved. Aborting tests.";
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5507, "SpamAssassinTestConnect::TestConnect", message);
         return false;
      }

      // Here we handle of the ownership to the TCPIP-connection layer.
      if (pSAClient->Connect(ip_address, port, IPAddress()))
      {
         // Make sure we keep no references to the TCP connection so that it
         // can be terminated whenever. We're longer own the connection.
         pSAClient.reset();

         disconnectEvent->Wait();
      }

      if (testCompleted)
         message = FileUtilities::ReadCompleteTextFile(tempFile);
      else
      {
         message = "Unable to connect to the specified SpamAssassin server.";
      }

      FileUtilities::DeleteFile(tempFile);

      return testCompleted;

   }

}