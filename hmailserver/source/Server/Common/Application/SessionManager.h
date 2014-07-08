// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../TCPIP/SocketConstants.h"
#include "../TCPIP/IPAddress.h"

namespace HM
{
   class SocketConnection;
   class Socket;
   class SecurityRange;

   class SessionManager : public Singleton<SessionManager>
   {
   public:
      SessionManager(void);
       ~SessionManager(void);

       int Count();

       bool CreateConnection(SessionType t, shared_ptr<SecurityRange> securityRange);
      
       void OnDisconnect(SessionType st);

       void RegisterPreCreatedConnection(Socket* pSocket);

       long GetNumberOfConnections(SessionType st);
       // Returns the number of connections for a specific connection timeout

       unsigned long GetNumberOfConnections();

   private:

      

      long m_iNoOfSMTPConnections;
      long m_iNoOfPOP3Connections;
      long m_iNoOfIMAPConnections;

   private:

   };
}