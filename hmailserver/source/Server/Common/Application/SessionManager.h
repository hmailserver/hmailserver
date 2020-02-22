// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../TCPIP/SocketConstants.h"
#include "../TCPIP/IPAddress.h"

#include <boost/atomic.hpp>

namespace HM
{
   class SecurityRange;

   class SessionManager : public Singleton<SessionManager>
   {
   public:
      SessionManager(void);
       ~SessionManager(void);

       int Count();

       bool CreateSession(SessionType t, std::shared_ptr<SecurityRange> security_range);
       void OnSessionEnded(SessionType st);

       int GetNumberOfConnections(SessionType st);
       // Returns the number of connections for a specific connection timeout

       int GetNumberOfConnections();

   private:

      boost::atomic<int> no_of_smtpconnections_;
      boost::atomic<int> no_of_pop3connections_;
      boost::atomic<int> no_of_imapconnections_;

   };
}