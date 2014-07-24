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
      TCPServer(boost::asio::io_service& io_service, const IPAddress &ipaddress, int port, SessionType sessionType, shared_ptr<SSLCertificate> certificate, shared_ptr<TCPConnectionFactory> connectionFactory, ConnectionSecurity connection_security);
      ~TCPServer(void);

      void Run();
      void StopAccept();

      static bool HasIPV6();

   private:
      
      std::string GetPassword() const;

      bool InitSSL();

      bool InitAcceptor();
      void StartAccept();
      void HandleAccept(shared_ptr<TCPConnection> pConnection, const boost::system::error_code& error);

      bool FireOnAcceptEvent(const IPAddress &remoteAddress, int port);
      
      shared_ptr<TCPConnectionFactory> connectionFactory_;

      boost::asio::ip::tcp::acceptor acceptor_;
      boost::asio::ssl::context context_;
      SessionType sessionType_;
      shared_ptr<SSLCertificate> certificate_;

      IPAddress ipaddress_;
      int port_;

      ConnectionSecurity connection_security_;
   };
}