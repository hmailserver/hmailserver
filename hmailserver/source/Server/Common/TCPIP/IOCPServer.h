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
   class Socket;
   class SocketCompletionPort;
   class TCPConnection;

   class IOCPServer : public Task
   {
   public:
      IOCPServer(void);
      ~IOCPServer(void);

      virtual void DoWork();
      virtual void StopWork();

      // Session types
      bool RegisterSessionType(SessionType st);

      shared_ptr<TCPConnection> CreateConnection(boost::asio::ssl::context& context);
      shared_ptr<TCPConnection> CreateConnection();

      boost::asio::io_service &GetIOService();

   private:

      const String m_sAsynchronousTasksQueue;

      Event m_evtClose;
      
      std::set<SessionType> m_setSessionTypes;
      boost::asio::io_service _io_service;
      boost::asio::ssl::context _dummy_context;
      
   };


}