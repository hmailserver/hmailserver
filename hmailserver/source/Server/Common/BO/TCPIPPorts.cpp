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
   // Reads all SURBL servers from the database.
   //---------------------------------------------------------------------------()
   {
      String sSQL = "select * from hm_tcpipports order by portaddress1 asc, portaddress2 asc, portnumber asc";
      _DBLoad(sSQL);
   }

   void 
   TCPIPPorts::SetDefault()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Reads all SURBL servers from the database.
   //---------------------------------------------------------------------------()
   {
      Refresh();

      // check if the settings are already default.
      if (vecObjects.size() == 3)
      {
         if (vecObjects[0]->GetPortNumber() == 25 && vecObjects[0]->GetProtocol() == STSMTP &&
             vecObjects[1]->GetPortNumber() == 110 && vecObjects[1]->GetProtocol() == STPOP3 &&
             vecObjects[2]->GetPortNumber() == 143 && vecObjects[2]->GetProtocol() == STIMAP) 
         {
            // no changes are needed. 
            return;
         }
      }
    
      // Delete all existing ports and then add new ones.
      DeleteAll();

      shared_ptr<TCPIPPort> pTCPIPPort = shared_ptr<TCPIPPort>(new TCPIPPort);
      pTCPIPPort->SetPortNumber(25);
      pTCPIPPort->SetProtocol(STSMTP);
      PersistentTCPIPPort::SaveObject(pTCPIPPort);

      pTCPIPPort = shared_ptr<TCPIPPort>(new TCPIPPort);
      pTCPIPPort->SetPortNumber(110);
      pTCPIPPort->SetProtocol(STPOP3);
      PersistentTCPIPPort::SaveObject(pTCPIPPort);

      pTCPIPPort = shared_ptr<TCPIPPort>(new TCPIPPort);
      pTCPIPPort->SetPortNumber(143);
      pTCPIPPort->SetProtocol(STIMAP);
      PersistentTCPIPPort::SaveObject(pTCPIPPort);

      Refresh();
   }

   shared_ptr<TCPIPPort> 
   TCPIPPorts::GetPort(const IPAddress &iIPAddress, int iPort)
   {
      vector<shared_ptr<TCPIPPort> >::iterator iter = vecObjects.begin();   
      vector<shared_ptr<TCPIPPort> >::iterator iterEnd = vecObjects.end();   

      std::vector<int> vecResult;
      for (; iter != iterEnd; iter++)
      {
         shared_ptr<TCPIPPort> pTCPIPPort = (*iter);
         if (pTCPIPPort->GetPortNumber() == iPort && 
            (pTCPIPPort->GetAddress() == iIPAddress || pTCPIPPort->GetAddress().IsAny()))
         {
            return pTCPIPPort;
         }
      }

      shared_ptr<TCPIPPort> pEmpty;
      return pEmpty;
   }

}