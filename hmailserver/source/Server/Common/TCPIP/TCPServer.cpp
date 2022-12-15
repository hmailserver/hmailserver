// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include "TCPServer.h"
#include "TCPConnection.h"

#include <ctime>
#include <iostream>
#include <string>

#include "../Scripting/ScriptServer.h"
#include "../Scripting/ScriptObjectContainer.h"
#include "../Scripting/Result.h"
#include "../Scripting/ClientInfo.h"
#include "../Persistence/PersistentSecurityRange.h"

#include "../Application/SessionManager.h"

#include "../Util/Encoding/Base64.h"

#include "SslContextInitializer.h"

using boost::asio::ip::tcp;

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   TCPServer::TCPServer(boost::asio::io_service& io_service, const IPAddress &ipaddress, int port, SessionType sessionType, std::shared_ptr<SSLCertificate> certificate, std::shared_ptr<TCPConnectionFactory> connectionFactory, ConnectionSecurity connection_security) :
      acceptor_(io_service),
      context_(boost::asio::ssl::context::sslv23),
      io_service_(io_service),
      ipaddress_(ipaddress),
      port_(port),
      connection_security_(connection_security)
   {
      sessionType_ = sessionType;
      certificate_ = certificate;
      connectionFactory_ = connectionFactory;
   }

   TCPServer::~TCPServer(void)
   {
      
   }

   bool
   TCPServer::InitAcceptor()
   {
      boost::asio::ip::tcp::endpoint endpoint(ipaddress_.GetAddress(), port_);

      try
      {
         acceptor_.open(endpoint.protocol());
         acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(false));
      }
      catch (boost::system::system_error error)
      {
         String msg = error.what();

         String sErrorMessage;
         sErrorMessage.Format(_T("Failed to initialize local acceptor. Error: %s. Address: %s, Port: %d."), 
            msg.c_str(), String(ipaddress_.ToString()).c_str(), port_);

         ErrorManager::Instance()->ReportError(ErrorManager::High, 4316, "TCPServer::Run()", sErrorMessage);

         return false;
      }

      try
      {
         acceptor_.bind(endpoint);

      }
      catch (boost::system::system_error error)
      {
         String msg = error.what();

         String sErrorMessage = Formatter::Format("Failed to bind to local port. Error: {0}. Address: {1}, Error code: {2}, Port: {3}. This is often caused by another server listening on the same port."
                                                  "To determine which server is listening on the port, telnet your server on port {4}. Make sure that no other email server is running "
                                                  "and listening on this port, and then restart the hMailServer service.", 
                                                   msg, String(ipaddress_.ToString()), error.code().value(), port_, port_);

         ErrorManager::Instance()->ReportError(ErrorManager::High, 4316, "TCPServer::Run()", sErrorMessage);

         return false;
      }

      try
      {
         acceptor_.listen();
      }
      catch (boost::system::system_error error)
      {
         String msg = error.what();

         String sErrorMessage = Formatter::Format("Failed to listen on local port. Error: {0}. Error code: {1}, Address: {2}, Port: {3}",
                                          msg, error.code().value(), String(ipaddress_.ToString()), port_);

         ErrorManager::Instance()->ReportError(ErrorManager::High, 4317, "TCPServer::Run()", sErrorMessage);

         return false;
      }

      return true;


   }
  

   void
   TCPServer::Run()
   {
      if (connection_security_ == CSSSL ||
          connection_security_ == CSSTARTTLSOptional ||
          connection_security_ == CSSTARTTLSRequired)
      {
         if (!SslContextInitializer::InitServer(context_, certificate_, ipaddress_.ToString(), port_))
            return;
      }

      if (!InitAcceptor())
         return;

      StartAccept();
   }

   void
   TCPServer::StartAccept()
   {
      if (acceptor_.is_open())
      {
        
         std::shared_ptr<TCPConnection> pNewConnection = connectionFactory_->Create(connection_security_, io_service_, context_);

         acceptor_.async_accept(pNewConnection->GetSocket(),
            std::bind(&TCPServer::HandleAccept, this, pNewConnection,
            std::placeholders::_1));
      }
   }
   
   void
   TCPServer::StopAccept()
   {
      // eat any errors thrown by cancel:
      boost::system::error_code error;
      acceptor_.cancel(error);

      acceptor_.close();
   }

   void 
   TCPServer::HandleAccept(std::shared_ptr<TCPConnection> connection,
      const boost::system::error_code &error)
   {
      if (error.value() == 995)
      {
         String sMessage;
         sMessage.Format(_T("TCP - AcceptEx failed. Error code: %d, Message: %s"), error.value(), String(error.message()).c_str());
         LOG_DEBUG(sMessage);

         /*
             995: The I/O operation has been aborted because of either a thread exit or an application request
             
             This happens when the servers are stopped. We shouldn't post any new accepts since we're shutting down.
        */

         return;
      }

      // Post another AcceptEx. We should always have outstanding unless the 
      // server is stopping, which we are taking care of above.
      StartAccept();

      if (!error)
      {
         boost::asio::ip::tcp::endpoint localEndpoint = connection->GetSocket().local_endpoint();
         boost::asio::ip::tcp::endpoint remoteEndpoint = connection->GetSocket().remote_endpoint();

         IPAddress localAddress (localEndpoint.address());
         IPAddress remoteAddress (remoteEndpoint.address());

         String sMessage = Formatter::Format("TCP - {0} connected to {1}:{2}.", remoteAddress.ToString(), localAddress.ToString(), port_);
         LOG_TCPIP(sMessage);

         std::shared_ptr<SecurityRange> securityRange = PersistentSecurityRange::ReadMatchingIP(remoteAddress);

         bool allow = SessionManager::Instance()->CreateSession(sessionType_, securityRange);
        
         if (!allow)
         {
            // Session creation failed. May not be matching IP range, or enough connections have been created.
            String message;
            message.Format(_T("Client connection from %s was not accepted. Blocked either by IP range or by connection limit."), String(remoteAddress.ToString()).c_str());
            LOG_DEBUG(message);

            // Give option to hold connection for anti-pounding & hopefully minimize DoS
            // NOTE: We really need max connections per IP as well
            int iBlockedIPHoldSeconds = IniFileSettings::Instance()->GetBlockedIPHoldSeconds();

            if (iBlockedIPHoldSeconds > 0)
            {
               Sleep(iBlockedIPHoldSeconds * 1000);
               message.Format(_T("Held connection from %s for %i seconds before dropping."), String(remoteAddress.ToString()).c_str(), iBlockedIPHoldSeconds);
               LOG_DEBUG(message);
            }

            return;
         }

         if (!FireOnAcceptEvent(connection, remoteAddress, localEndpoint.port()))
         {
            // Session has been created, but is now terminated by a custom script. Since we haven't started the
            // TCPConnection yet, we are still responsible for tracking connection count.
            SessionManager::Instance()->OnSessionEnded(sessionType_);
            return;
         }

         connection->SetSecurityRange(securityRange);
         connection->Start();

         // Now TCPConnection is responsible for decreasing the session count when the connection ends.
      }
      else
      {
         if (error.value() == 10009 || error.value() == 995)
         {
            // Not really errors..
            return;
         }

         // The outstanding accept-ex failed. This may or may not be an error. Default to being positive.
         String sMessage;
         sMessage.Format(_T("TCP - AcceptEx failed. Error code: %d, Message: %s"), error.value(), String(error.message()).c_str());
         LOG_TCPIP(sMessage);
      }
   }

   bool
   TCPServer::FireOnAcceptEvent(std::shared_ptr<TCPConnection> connection, const IPAddress &remoteAddress, int port)
   {
      // Fire an event...
      if (!Configuration::Instance()->GetUseScriptServer())
         return true;

      std::shared_ptr<ClientInfo> pCliInfo = std::shared_ptr<ClientInfo>(new ClientInfo);
      pCliInfo->SetIPAddress(remoteAddress.ToString());
      pCliInfo->SetPort(port);
      pCliInfo->SetSessionID(connection->GetSessionID());

      std::shared_ptr<ScriptObjectContainer> pContainer = std::shared_ptr<ScriptObjectContainer>(new ScriptObjectContainer);
      std::shared_ptr<Result> pResult = std::shared_ptr<Result>(new Result);

      pContainer->AddObject("Result", pResult, ScriptObject::OTResult);
      pContainer->AddObject("HMAILSERVER_CLIENT", pCliInfo, ScriptObject::OTClient);

      String sEventCaller;

      String sScriptLanguage = Configuration::Instance()->GetScriptLanguage();

      if (sScriptLanguage == _T("VBScript"))
         sEventCaller.Format(_T("OnClientConnect(HMAILSERVER_CLIENT)"));
      else if (sScriptLanguage == _T("JScript"))
         sEventCaller.Format(_T("OnClientConnect(HMAILSERVER_CLIENT);"));

      ScriptServer::Instance()->FireEvent(ScriptServer::EventOnClientConnect, sEventCaller, pContainer);

      switch (pResult->GetValue())
      {
      case 1:
         {
            // Disconnect the socket immediately.
            return false;
         }
      }

      return true;
   }
}