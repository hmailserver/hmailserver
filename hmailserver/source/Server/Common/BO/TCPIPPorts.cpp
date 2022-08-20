// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "TCPIPPorts.h"

#include "../Persistence/PersistentBlockedAttachment.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   TCPIPPorts::TCPIPPorts()
   {
   }

   TCPIPPorts::~TCPIPPorts(void)
   {
   }

   void 
   TCPIPPorts::Refresh()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Reads all TCP/IP Ports from the database.
   //---------------------------------------------------------------------------()
   {
      String sSQL = "select * from hm_tcpipports order by portaddress1 asc, portaddress2 asc, portnumber asc";
      DBLoad_(sSQL);
   }

   void 
   TCPIPPorts::SetDefault()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Resets all TCP/IP Ports to their default values.
   //---------------------------------------------------------------------------()
   {
      Refresh();

      // check if the settings are already default.
      if (vecObjects.size() == 4)
      {
         /* Shouldn't we reset SSL/TLS settings as well?
         https://github.com/hmailserver/hmailserver/pull/441#issuecomment-1221246664
         if (vecObjects[0]->GetPortNumber() == 25 && vecObjects[0]->GetProtocol() == STSMTP &&
            vecObjects[1]->GetPortNumber() == 110 && vecObjects[1]->GetProtocol() == STPOP3 &&
            vecObjects[2]->GetPortNumber() == 143 && vecObjects[2]->GetProtocol() == STIMAP &&
            vecObjects[3]->GetPortNumber() == 587 && vecObjects[3]->GetProtocol() == STSMTP)
         */
         if (vecObjects[0]->GetPortNumber() == 25 && vecObjects[0]->GetProtocol() == STSMTP && vecObjects[0]->GetConnectionSecurity() == CSNone &&
            vecObjects[1]->GetPortNumber() == 110 && vecObjects[1]->GetProtocol() == STPOP3 && vecObjects[1]->GetConnectionSecurity() == CSNone &&
            vecObjects[2]->GetPortNumber() == 143 && vecObjects[2]->GetProtocol() == STIMAP && vecObjects[2]->GetConnectionSecurity() == CSNone &&
            vecObjects[3]->GetPortNumber() == 587 && vecObjects[3]->GetProtocol() == STSMTP && vecObjects[3]->GetConnectionSecurity() == CSNone)
         {
            // no changes are needed. 
            return;
         }
      }
    
      // Delete all existing ports and then add new ones.
      DeleteAll();

      String error_message;

      std::shared_ptr<TCPIPPort> pTCPIPPort = std::shared_ptr<TCPIPPort>(new TCPIPPort);
      pTCPIPPort->SetPortNumber(25);
      pTCPIPPort->SetProtocol(STSMTP);
      PersistentTCPIPPort::SaveObject(pTCPIPPort, error_message, PersistenceModeNormal);

      pTCPIPPort = std::shared_ptr<TCPIPPort>(new TCPIPPort);
      pTCPIPPort->SetPortNumber(110);
      pTCPIPPort->SetProtocol(STPOP3);
      PersistentTCPIPPort::SaveObject(pTCPIPPort, error_message, PersistenceModeNormal);

      pTCPIPPort = std::shared_ptr<TCPIPPort>(new TCPIPPort);
      pTCPIPPort->SetPortNumber(143);
      pTCPIPPort->SetProtocol(STIMAP);
      PersistentTCPIPPort::SaveObject(pTCPIPPort, error_message, PersistenceModeNormal);

      pTCPIPPort = std::shared_ptr<TCPIPPort>(new TCPIPPort);
      pTCPIPPort->SetPortNumber(587);
      pTCPIPPort->SetProtocol(STSMTP);
      PersistentTCPIPPort::SaveObject(pTCPIPPort, error_message, PersistenceModeNormal);

      Refresh();
   }

   std::shared_ptr<TCPIPPort> 
   TCPIPPorts::GetPort(const IPAddress &iIPAddress, int iPort)
   {
      auto iter = vecObjects.begin();   
      auto iterEnd = vecObjects.end();   

      std::vector<int> vecResult;
      for (; iter != iterEnd; iter++)
      {
         std::shared_ptr<TCPIPPort> pTCPIPPort = (*iter);
         if (pTCPIPPort->GetPortNumber() == iPort && 
            (pTCPIPPort->GetAddress() == iIPAddress || pTCPIPPort->GetAddress().IsAny()))
         {
            return pTCPIPPort;
         }
      }

      std::shared_ptr<TCPIPPort> pEmpty;
      return pEmpty;
   }

}