// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "SpamAssassinTestConnect.h"
#include "SpamAssassinClient.h"

#include "../../TCPIP/IOCPServer.h"
#include "../../TCPIP/TCPConnection.h"

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

      shared_ptr<SpamAssassinClient> pSAClient = shared_ptr<SpamAssassinClient>(new SpamAssassinClient(tempFile));

      shared_ptr<TCPConnection> pClientConnection = Application::Instance()->GetIOCPServer()->CreateConnection();
      pClientConnection->Start(pSAClient);

      // Copy the event so that we know when we've disconnected.
      Event disconnectEvent(pClientConnection->GetConnectionTerminationEvent());

      // Here we handle of the ownership to the TCPIP-connection layer.
      if (pClientConnection->Connect(hostName, port, IPAddress()))
      {
         // Make sure we keep no references to the TCP connection so that it
         // can be terminated whenever. We're longer own the connection.
         pClientConnection.reset();

         disconnectEvent.Wait();
      }

      // Copy back the file...
      if (pSAClient->FinishTesting())
      {
         message = FileUtilities::ReadCompleteTextFile(tempFile);
         FileUtilities::DeleteFile(tempFile);
         return true;
      }
      else
      {
         message = "Unable to connect to the specified SpamAssassin server.";
         FileUtilities::DeleteFile(tempFile);
         return false;
      }
   }

}