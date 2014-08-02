// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../TCPIP/SocketConstants.h"
#include "../TCPIP/IPAddress.h"

namespace HM
{
   class SecurityRange;

   class SessionManager : public Singleton<SessionManager>
   {
   public:
      SessionManager(void);
       ~SessionManager(void);

       int Count();

       bool GetAllow(SessionType t, shared_ptr<SecurityRange> security_range);
       void OnCreate(SessionType t);
       void OnDestroy(SessionType st);

       long GetNumberOfConnections(SessionType st);
       // Returns the number of connections for a specific connection timeout

       unsigned long GetNumberOfConnections();

   private:

      

      long no_of_smtpconnections_;
      long no_of_pop3connections_;
      long no_of_imapconnections_;

   private:

   };
}