// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include "TCPServer.h"
#include "TCPConnection.h"

#include <ctime>
#include <iostream>
#include <string>


#include "../Application/ScriptingHost/ScriptServer.h"
#include "../Application/ScriptingHost/ScriptObjectContainer.h"
#include "../Application/ScriptingHost/Result.h"
#include "../Application/ClientInfo.h"
#include "../Persistence/PersistentSecurityRange.h"

#include "../Application/SessionManager.h"

using boost::asio::ip::tcp;

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   TCPServer::TCPServer(boost::asio::io_service& io_service, const IPAddress &ipaddress, int port, SessionType sessionType, shared_ptr<SSLCertificate> certificate) :
      _acceptor(io_service),
      _context(io_service, boost::asio::ssl::context::sslv23),
      _ipaddress(ipaddress),
      _port(port)
   {
      _sessionType = sessionType;
      _certificate = certificate;
   }

   TCPServer::~TCPServer(void)
   {
   }

   bool
   TCPServer::InitAcceptor()
   {
      boost::asio::ip::tcp::endpoint endpoint(_ipaddress.GetAddress(), _port);

      try
      {
         _acceptor.open(endpoint.protocol());
         _acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(false));
      }
      catch (boost::system::system_error error)
      {
         String msg = error.what();

         String sErrorMessage;
         sErrorMessage.Format(_T("Failed to initialize local acceptor. Error: %s. Address: %s, Port: %d."), 
            msg, String(_ipaddress.ToString()), _port);

         ErrorManager::Instance()->ReportError(ErrorManager::High, 4316, "TCPServer::Run()", sErrorMessage);

         return false;
      }

      try
      {
         _acceptor.bind(endpoint);

      }
      catch (boost::system::system_error error)
      {
         String msg = error.what();

         String sErrorMessage = Formatter::Format("Failed to bind to local port. Error: {0}. Address: {1}, Error code: {2}, Port: {3}. This is often caused by another server listening on the same port."
                                                  "To determine which server is listening on the port, telnet your server on port {4}. Make sure that no other email server is running "
                                                  "and listening on this port, and then restart the hMailServer service.", 
                                                   msg, String(_ipaddress.ToString()), error.code().value(), _port, _port);

         ErrorManager::Instance()->ReportError(ErrorManager::High, 4316, "TCPServer::Run()", sErrorMessage);

         return false;
      }

      try
      {
         _acceptor.listen();
      }
      catch (boost::system::system_error error)
      {
         String msg = error.what();

         String sErrorMessage = Formatter::Format("Failed to listen on local port. Error: {0}. Error code: {1}, Address: {2}, Port: {3}",
                                          msg, error.code().value(), String(_ipaddress.ToString()), _port);

         ErrorManager::Instance()->ReportError(ErrorManager::High, 4317, "TCPServer::Run()", sErrorMessage);

         return false;
      }

      return true;


   }

   bool 
   TCPServer::InitSSL()
   {
      try
      {
         _context.set_options(boost::asio::ssl::context::default_workarounds |
                              boost::asio::ssl::context::no_sslv2);
      }
      catch (boost::system::system_error ec)
      {
         String asioError = ec.what();

         String errorMessage;
         errorMessage.Format(_T("Failed to set SSL context options. Address: %s, Port: %i, Error: %s"), 
            String(_ipaddress.ToString()), _port, asioError);

         ErrorManager::Instance()->ReportError(ErrorManager::High, 5113, "TCPServer::InitSSL()", errorMessage);

         return false;

      }

      AnsiString certificateFile = _certificate->GetCertificateFile();
      AnsiString privateKeyFile = _certificate->GetPrivateKeyFile();


      try
      {
         _context.use_certificate_file(certificateFile, boost::asio::ssl::context::pem);
      }
      catch (boost::system::system_error ec)
      {
         String asioError = ec.what();

         String errorMessage;
         errorMessage.Format(_T("Failed to load certificate file. Path: %s, Address: %s, Port: %i, Error: %s"), 
            String(certificateFile), String(_ipaddress.ToString()), _port, asioError);

         ErrorManager::Instance()->ReportError(ErrorManager::High, 5113, "TCPServer::InitSSL()", errorMessage);

         return false;
      }

      try
      {
         _context.use_certificate_chain_file(certificateFile);
      }
      catch (boost::system::system_error ec)
      {
         String asioError = ec.what();

         String errorMessage;
         errorMessage.Format(_T("Failed to load certificate chain from certificate file. Path: %s, Address: %s, Port: %i, Error: %s"), 
            String(certificateFile), String(_ipaddress.ToString()), _port, asioError);

         ErrorManager::Instance()->ReportError(ErrorManager::High, 5113, "TCPServer::InitSSL()", errorMessage);

         return false;
      }
      
      try
      {
         _context.set_password_callback(boost::bind(&TCPServer::GetPassword, this));
         _context.use_private_key_file(privateKeyFile, boost::asio::ssl::context::pem);
      }
      catch (boost::system::system_error ec)
      {
         String asioError = ec.what();

         String errorMessage;
         errorMessage.Format(_T("Failed to load private key file. Path: %s, Address: %s, Port: %i, Error: %s"), 
            String(privateKeyFile), String(_ipaddress.ToString()), _port, asioError);

         ErrorManager::Instance()->ReportError(ErrorManager::High, 5113, "TCPServer::InitSSL()", errorMessage);

         return false;
      }
      catch (...)
      {
         String errorMessage = "Error initializing SSL";
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5113, "TCPServer::InitSSL()", errorMessage);

      }
     
      return true;
   }

   std::string 
   TCPServer::GetPassword() const
   {
      ErrorManager::Instance()->ReportError(ErrorManager::High, 5143, "TCPServer::GetPassword()", "The private key file has a password. hMailServer does not support this.");
      return "";
   }


   void
   TCPServer::Run()
   {
      if (_certificate)
      {
         if (!InitSSL())
            return;
      }

      if (!InitAcceptor())
         return;

      StartAccept();
   }

   void
   TCPServer::StartAccept()
   {
      if (_acceptor.is_open())
      {
         bool useSSL = _certificate != 0;
         
         shared_ptr<TCPConnection> pNewConnection = shared_ptr<TCPConnection> (new TCPConnection(useSSL, _acceptor.get_io_service(), _context));

         _acceptor.async_accept(pNewConnection->GetSocket(),
            boost::bind(&TCPServer::HandleAccept, this, pNewConnection,
            boost::asio::placeholders::error));
      }
   }

   void
   TCPServer::StopAccept()
   {
      // eat any errors thrown by cancel:
      boost::system::error_code error;
      _acceptor.cancel(error);

      _acceptor.close();
   }


   void 
   TCPServer::HandleAccept(shared_ptr<TCPConnection> pConnection,
      const boost::system::error_code& error)
   {
      if (error.value() == 995)
      {
         /*
             995: The I/O operation has been aborted because of either a thread exit or an application request
             
             This happens when the servers are stopped. We shouldn't post any new accepts or do anything
             else in this situation.
        */

         return;
      }

      // Post another AcceptEx. We should always have outstanding unless the 
      // server is stopping, which we are taking care of above.
      StartAccept();

      if (!error)
      {
         boost::asio::ip::tcp::endpoint localEndpoint = pConnection->GetSocket().local_endpoint();
         boost::asio::ip::tcp::endpoint remoteEndpoint = pConnection->GetSocket().remote_endpoint();

         IPAddress localAddress (localEndpoint.address());
         IPAddress remoteAddress (remoteEndpoint.address());

         String sMessage = Formatter::Format("TCP - {0} connected to {1}:{2}.", remoteAddress.ToString(), localAddress.ToString(), _port);
         LOG_TCPIP(sMessage);

         shared_ptr<SecurityRange> securityRange = PersistentSecurityRange::ReadMatchingIP(remoteAddress);

         if (!securityRange)
         {
            LOG_TCPIP("TCP - Connection dropped - No matching IP range.");
            return;
         }

         shared_ptr<ProtocolParser> pProtocolParser = 
            SessionManager::Instance()->CreateConnection(_sessionType, securityRange);

         if (!pProtocolParser)
         {
            // Session creation failed. May not be matching IP range, or enough connections have been created.
            String message;
            message.Format(_T("Client connection from %s was not accepted. Blocked either by IP range or by connection limit."), String(remoteAddress.ToString()));
            LOG_DEBUG(message);

            // Give option to hold connection for anti-pounding & hopefully minimize DoS
            // NOTE: We really need max connections per IP as well
            int iBlockedIPHoldSeconds = IniFileSettings::Instance()->GetBlockedIPHoldSeconds();

            if (iBlockedIPHoldSeconds > 0)
            {
               Sleep(iBlockedIPHoldSeconds * 1000);
               message.Format(_T("Held connection from %s for %i seconds before dropping."), String(remoteAddress.ToString()), iBlockedIPHoldSeconds);
               LOG_DEBUG(message);
            }

            return;
         }

         if (!FireOnAcceptEvent(remoteAddress, localEndpoint.port()))
            return;

         pConnection->SetSecurityRange(securityRange);
         pConnection->Start(pProtocolParser);
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
         sMessage.Format(_T("TCP - AcceptEx failed. Error code: %d, Message: %s"), error.value(), String(error.message()));
         LOG_TCPIP(sMessage);
      }
   }

   bool
   TCPServer::FireOnAcceptEvent(const IPAddress &remoteAddress, int port)
   {
      // Fire an event...
      if (!Configuration::Instance()->GetUseScriptServer())
         return true;

      shared_ptr<ClientInfo> pCliInfo = shared_ptr<ClientInfo>(new ClientInfo);
      pCliInfo->SetIPAddress(remoteAddress.ToString());
      pCliInfo->SetPort(port);

      shared_ptr<ScriptObjectContainer> pContainer = shared_ptr<ScriptObjectContainer>(new ScriptObjectContainer);
      shared_ptr<Result> pResult = shared_ptr<Result>(new Result);

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
   
   bool 
   TCPServer::HasIPV6()
   {
      IPAddress address;
      return address.TryParse("::F", false);
   }

}