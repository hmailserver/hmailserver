// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include <ctime>
#include <iostream>

#include "SocketConstants.h"

#include "../BO/SSLCertificates.h"
#include "../BO/SSLCertificate.h"

using boost::asio::ip::tcp;

namespace HM
{
   class TCPConnection;

   class TCPServer
   {
   public:
      TCPServer(boost::asio::io_service& io_service, const IPAddress &ipaddress, int port, SessionType sessionType, shared_ptr<SSLCertificate> certificate);
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

      boost::asio::ip::tcp::acceptor _acceptor;
      boost::asio::ssl::context _context;
      SessionType _sessionType;
      shared_ptr<SSLCertificate> _certificate;

      IPAddress _ipaddress;
      int _port;
   };
}