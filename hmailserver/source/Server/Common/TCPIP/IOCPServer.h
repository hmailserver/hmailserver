// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include <Boost\function.hpp>
#include "..\Application\SessionManager.h"
#include "..\Threading\Task.h"
#include "..\Util\Event.h"

#include "SocketConstants.h"



namespace HM
{
   class TCPServer;

   class IOCPServer : public Task
   {
   public:
      IOCPServer(void);
      ~IOCPServer(void);

      void DoWork();

      // Session types
      bool RegisterSessionType(SessionType st);

      boost::asio::io_service &GetIOService();

   private:

      const String m_sAsynchronousTasksQueue;

      std::set<SessionType> m_setSessionTypes;
      boost::asio::io_service io_service_;
      boost::asio::ssl::context dummy_context_;

      vector<shared_ptr<TCPServer> > tcp_servers_;

      boost::condition_variable do_work_dummy;
   };


}