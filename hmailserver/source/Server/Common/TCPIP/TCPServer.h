// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include <ctime>
#include <iostream>

#include "SocketConstants.h"

#include "../BO/SSLCertificates.h"
#include "../BO/SSLCertificate.h"

#include "TCPConnectionFactory.h"

using boost::asio::ip::tcp;

namespace HM
{
   class TCPConnection;

   class TCPServer
   {
   public:
      TCPServer(boost::asio::io_service& io_service, const IPAddress &ipaddress, int port, SessionType sessionType, std::shared_ptr<SSLCertificate> certificate, std::shared_ptr<TCPConnectionFactory> connectionFactory, ConnectionSecurity connection_security);
      ~TCPServer(void);

      void Run();
      void StopAccept();

   private:
      
      std::string GetPassword() const;

      bool InitAcceptor();
      void StartAccept();
      void HandleAccept(std::shared_ptr<TCPConnection> connection, const boost::system::error_code &error);

      bool FireOnAcceptEvent(std::shared_ptr<TCPConnection> connection, const IPAddress &remoteAddress, int port);
      
      std::shared_ptr<TCPConnectionFactory> connectionFactory_;

      boost::asio::ip::tcp::acceptor acceptor_;
      boost::asio::ssl::context context_;
      boost::asio::io_service& io_service_;
      SessionType sessionType_;
      std::shared_ptr<SSLCertificate> certificate_;

      IPAddress ipaddress_;
      int port_;

      ConnectionSecurity connection_security_;
   };
}