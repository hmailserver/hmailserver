// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include "TCPConnection.h"

#include "../Util/ByteBuffer.h"
#include "../BO/TCPIPPorts.h"
#include "../Persistence/PersistentSecurityRange.h"

#include "LocalIPAddresses.h"
#include "IPAddress.h"
#include "IOOperation.h"
#include "CertificateVerifier.h"

#include <boost/scope_exit.hpp>

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
  

   TCPConnection::TCPConnection(ConnectionSecurity connection_security,
                                boost::asio::io_service& io_service, 
                                boost::asio::ssl::context& context,
                                shared_ptr<Event> disconnected,
                                AnsiString expected_remote_hostname) :
      connection_security_(connection_security),
      socket_(io_service),
      ssl_socket_(socket_, context),
      resolver_(io_service),
      timer_(io_service),
      receive_binary_(false),
      remote_port_(0),
      has_timeout_(false),
      receive_buffer_(250000),
      disconnected_(disconnected),
      context_(context),
      is_ssl_(false),
      expected_remote_hostname_(expected_remote_hostname),
      is_client_(false)
   {
      session_id_ = Application::Instance()->GetUniqueID();

      LOG_DEBUG("Creating session " + StringParser::IntToString(session_id_));

   }

   TCPConnection::~TCPConnection(void)
   {
      LOG_DEBUG("Ending session " + StringParser::IntToString(session_id_));

      if (disconnected_)
         disconnected_->Set();

      CancelLogoutTimer();
   }

   bool
   TCPConnection::Connect(const AnsiString &remote_ip_address, long remotePort, const IPAddress &localAddress)
   {
      try
      {
#if _DEBUG
         if (!StringParser::IsValidIPAddress(remote_ip_address))
         {
            ErrorManager::Instance()->ReportError(ErrorManager::High, 5506, "TCPConnection::Connect", 
               Formatter::Format("Attempting to connect to {0} - Not a valid IP address.", remote_ip_address));
         }
#endif

         remote_port_ = remotePort;
         remote_ip_address_ = remote_ip_address;
         is_client_ = true;

         LOG_TCPIP("Connecting to " + remote_ip_address_ + "...");

         if (!localAddress.IsAny())
         {
            try
            {
               if (localAddress.GetType() == IPAddress::IPV4)
                  socket_.open(boost::asio::ip::tcp::v4());
               else if (localAddress.GetType() == IPAddress::IPV6)
                  socket_.open(boost::asio::ip::tcp::v6());

               socket_.bind(boost::asio::ip::tcp::endpoint(localAddress.GetAddress(), 0));
            }
            catch (boost::system::system_error error)
            {
               try
               {
                  // this may throw...
                  socket_.close();
               }
               catch (...)
               {
                  // shouldn't be an error.
               }


               String errorMessage = Formatter::Format("Failed to bind to IP address {0}.", localAddress.ToString());
               ReportError(ErrorManager::Medium, 4330, "TCPConnection::Connect", errorMessage, error);

               return false;
            }
         }

   #ifdef _DEBUG
         String sMessage;
         sMessage.Format(_T("RESOLVE: %s\r\n"), String(remote_ip_address_));
   #endif 

         // Start an asynchronous resolve to translate the server and service names
         // into a list of endpoints.
         StartAsyncConnect_(remote_ip_address, remotePort);
         return true;
      }
      catch (...)
      {
         String errorMessage = Formatter::Format("An unknown error occurred while connecting to {0} on port {1}", remote_ip_address, remotePort);
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5321, "TCPConnection::Connect", errorMessage);
         throw;
      }
   }

   void
   TCPConnection::StartAsyncConnect_(const String &ip_adress, int port)
   {
      try
      {
         IPAddress adress;
         adress.TryParse(ip_adress, true);

         tcp::endpoint ep;
         ep.address(adress.GetAddress());
         ep.port(port);

         //// Check that we don't try to connect to a port we're listening on. Doing
         //// that could start evil loops.
         //tcp::endpoint endpoint = *endpoint_iterator;

         if (LocalIPAddresses::Instance()->IsLocalPort(ep.address(), remote_port_))
         {
            String sMessage; 
            sMessage.Format(_T("Could not connect to %s on port %d since this would mean connecting to myself."), remote_ip_address_, remote_port_);

            OnCouldNotConnect(sMessage);

            LOG_TCPIP(_T("TCPConnection - ") + sMessage);

            return;
         }

         // Attempt a connection to the first endpoint in the list. Each endpoint
         // will be tried until we successfully establish a connection.
         try
         {
            socket_.async_connect(ep,
               boost::bind(&TCPConnection::AsyncConnectCompleted, shared_from_this(), boost::asio::placeholders::error));
         }
         catch (boost::system::system_error error)
         {
            // We failed to send the data to the client. Log an message in the log, 
            // and switch of operation-in-progress flag. We don't log this as an 
            // error since it most likely isn't.
            String sMessage;
            sMessage.Format(_T("TCPConnection - Call to async_connect failed. Error code: %d, Message: %s"), error.code().value(), String(error.what()));
            LOG_TCPIP(sMessage);
         }
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5323, "TCPConnection::StartAsyncConnect_", "An unknown error occurred while starting asynchronous connect.");
         throw;
      }
   }

   void
   TCPConnection::AsyncConnectCompleted(const boost::system::error_code& err)
   {
      try
      {
         if (err)
         {
            // Are there more addresses we should attempt to connect to?
            AnsiString error = err.message();
            OnCouldNotConnect("Host name: " + remote_ip_address_ + ", message: " + error);

            return;
         }

         Start();
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5324, "TCPConnection::AsyncConnectCompleted", "An unknown error occurred while handling asynchronous connect.");
         throw;
      }
   }

   void 
   TCPConnection::Start()
   {
      LOG_DEBUG(Formatter::Format("TCP connection started for session {0}", session_id_));

      OnConnected();

      if (connection_security_ == CSSSL)
      {
         EnqueueHandshake();
      }
   }

   void
   TCPConnection::ProcessOperationQueue_()
   {
      int stage = 0;

      try
      {

         stage = 1;
         // Pick out the next item to process...
         shared_ptr<IOOperation> operation = operation_queue_.Front();

         stage = 2;
         if (!operation)
         {
            // We're no longer sending...
            stage = 20;
            return;
         }
         stage = 3;

        switch (operation->GetType())
         {
        case IOOperation::BCTHandshake:
           {
              stage = 4;
              AsyncHandshake();
              stage = 5;
              break;
           }

        case IOOperation::BCTWrite:
            {
               stage = 6;
               shared_ptr<ByteBuffer> pBuf = operation->GetBuffer();
               AsyncWrite(pBuf);
               stage = 7;
               break;
            }
         case IOOperation::BCTRead:
            {
               stage = 8;
               AsyncRead(operation->GetString());               
               stage = 9;
               break;
            }
         case IOOperation::BCTShutdownSend:
            {
               stage = 10;
               Shutdown(boost::asio::ip::tcp::socket::shutdown_send);
               operation_queue_.Pop(IOOperation::BCTShutdownSend);
               ProcessOperationQueue_();
               stage = 11;
               break;
            }
         case IOOperation::BCTDisconnect:
            {
               stage = 12;
               Disconnect();
               operation_queue_.Pop(IOOperation::BCTDisconnect);
               ProcessOperationQueue_();
               stage = 13;
               break;
            }

         }
      }
      catch (boost::system::system_error error)
      {
         ReportError(ErrorManager::Medium, 5138, "TCPConnection::ProcessOperationQueue_", 
            Formatter::Format("An error occured while processing the queue. Stage: {0}", stage), error);
      }
      catch (...)
      {
         ReportError(ErrorManager::Medium, 5138, "TCPConnection::ProcessOperationQueue_", 
            Formatter::Format("An error occured while processing the queue. Stage: {0}", stage));
      }

      
   }
   

   void 
   TCPConnection::Shutdown(boost::asio::socket_base::shutdown_type what)
   {
      try
      {
         try
         {
            socket_.shutdown(what);
         }
         catch (boost::system::system_error)
         {
            // hopefully should not matter.
         }      
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5330, "TCPConnection::Shutdown", "An unknown error occurred while shutting down socket.");
         throw;
      }

   }


   void 
   TCPConnection::EnqueueDisconnect()
   {
      try
      {
         shared_ptr<ByteBuffer> pBuf;
         shared_ptr<IOOperation> operation = shared_ptr<IOOperation>(new IOOperation(IOOperation::BCTDisconnect, pBuf));
         operation_queue_.Push(operation);

         ProcessOperationQueue_();
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5335, "TCPConnection::EnqueueDisconnect", "An unknown error occurred while posting disconnect.");
         throw;
      }
   }


   void 
   TCPConnection::Disconnect()
   {
      try
      {
         LOG_DEBUG("Closing TCP/IP socket");

         // Perform graceful shutdown. No more operations will be performed. 
         Shutdown(boost::asio::socket_base::shutdown_both);
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5336, "TCPConnection::Disconnect", "An unknown error occurred while disconnecting.");
         throw;
      }

   }

   void 
   TCPConnection::EnqueueHandshake()
   {
      try
      {
         shared_ptr<ByteBuffer> pBuf;
         shared_ptr<IOOperation> operation = shared_ptr<IOOperation>(new IOOperation(IOOperation::BCTHandshake, pBuf));
         operation_queue_.Push(operation);

         ProcessOperationQueue_();
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5335, "TCPConnection::EnqueueDisconnect", "An unknown error occurred while posting disconnect.");
         throw;
      }
   }


   void 
   TCPConnection::AsyncHandshake()
   {
      // To do peer verification, it must both be enabled globally and supported by the deriving class.
      bool enable_peer_verification = Configuration::Instance()->GetVerifyRemoteSslCertificate() && GetValidateRemoteCertificate();

      int verify_mode = 0;

      boost::system::error_code error_code;

      if (enable_peer_verification)
      {
         verify_mode = boost::asio::ssl::context::verify_peer;
                
         if (!expected_remote_hostname_.IsEmpty())
         {
            ssl_socket_.set_verify_callback(CertificateVerifier(expected_remote_hostname_), error_code);

            if (error_code.value() != 0)
            {
               String errorMessage = Formatter::Format(_T("Failed to set verification callback for host {0}"), expected_remote_hostname_);
               ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5510, "TCPConnection::AsyncHandshake", errorMessage, error_code);
               HandshakeFailed_(error_code);
               return;
            }
         }
      }
      else
      {
         verify_mode = boost::asio::ssl::context::verify_none;
      }

      ssl_socket_.set_verify_mode(verify_mode, error_code);
      if (error_code.value() != 0)
      {
         String error_message = Formatter::Format(_T("Failed to configure OpenSSL certificate verification: Mode: {0}"), verify_mode);
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5144, "TCPConnection::AsyncHandshake", error_message, error_code);
         HandshakeFailed_(error_code);
         return;
      }


      LOG_DEBUG(Formatter::Format("Performing SSL/TLS handshake for session {0}. Verify certificate: {1}, Verify mode: {2}, Expected remote: {3}", session_id_, enable_peer_verification, verify_mode, expected_remote_hostname_));

      boost::asio::ssl::stream_base::handshake_type handshakeType = IsClient() ?
         boost::asio::ssl::stream_base::client :
      boost::asio::ssl::stream_base::server;

      ssl_socket_.async_handshake(handshakeType,
         boost::bind(&TCPConnection::AsyncHandshakeCompleted, shared_from_this(),
         boost::asio::placeholders::error));
   }


   void 
   TCPConnection::AsyncHandshakeCompleted(const boost::system::error_code& error)
   {
      BOOST_SCOPE_EXIT(&operation_queue_, this_) {
         operation_queue_.Pop(IOOperation::BCTHandshake);
          this_->ProcessOperationQueue_();
      } BOOST_SCOPE_EXIT_END

      try
      {
         if (!error)
         {
            // Send welcome message to client.
            is_ssl_ = true;
            OnHandshakeCompleted();
         }
         else
         {
            HandshakeFailed_(error);
         }
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5326, "TCPConnection::AsyncHandshakeCompleted", "An unknown error occurred while handling handshake.");
         throw;
      }

   }

   void
   TCPConnection::HandshakeFailed_(const boost::system::error_code& error)
   {
      // The SSL handshake failed. This may happen for example if the user who has connected
      // to the TCP/IP port disconnects immediately without sending any data.
      String sMessage;
      sMessage.Format(_T("TCPConnection - SSL handshake with client failed. Error code: %d, Message: %s, Remote IP: %s"), error.value(), String(error.message()), SafeGetIPAddress());
      LOG_TCPIP(sMessage);

      OnHandshakeFailed();
   }



   void 
   TCPConnection::EnqueueShutdownSend()
   {
      shared_ptr<ByteBuffer> pBuf;
      shared_ptr<IOOperation> operation = shared_ptr<IOOperation>(new IOOperation(IOOperation::BCTShutdownSend, pBuf));
      operation_queue_.Push(operation);

      ProcessOperationQueue_();
   }

   void 
   TCPConnection::EnqueueRead()
   {
      EnqueueRead(GetCommandSeparator());
   }

   void 
   TCPConnection::EnqueueRead(const AnsiString &delimitor)
   {
      try
      {
         shared_ptr<IOOperation> operation = shared_ptr<IOOperation>(new IOOperation(IOOperation::BCTRead, delimitor));
         operation_queue_.Push(operation);

         ProcessOperationQueue_();
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5331, "TCPConnection::EnqueueRead", "An unknown error occurred while posting read buffer.");
         throw;
      }
   }

   void 
   TCPConnection::AsyncRead(const AnsiString &delimitor)
   {
      int stage = 1;

      try
      {
         function<void (const boost::system::error_code&, size_t)> AsyncReadCompletedFunction =
            boost::bind(&TCPConnection::AsyncReadCompleted, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred);

         stage = 2;
         try
         {
            if (is_ssl_)
            {
               stage = 3;
               if (delimitor.GetLength() == 0)
                  boost::asio::async_read(ssl_socket_, receive_buffer_, boost::asio::transfer_at_least(1), AsyncReadCompletedFunction);
               else
                  boost::asio::async_read_until(ssl_socket_, receive_buffer_,  delimitor, AsyncReadCompletedFunction);
            }
            else
            {
               stage = 4;
               if (delimitor.GetLength() == 0)
               {
                  stage = 41;
                  boost::asio::async_read(socket_, receive_buffer_, boost::asio::transfer_at_least(1), AsyncReadCompletedFunction);
                  stage = 42;
               }
               else
               {
                  stage = 43;
                  boost::asio::async_read_until(socket_, receive_buffer_, delimitor, AsyncReadCompletedFunction);
                  stage = 44;
               }
            }

            stage = 5;
            UpdateLogoutTimer();
         }
         catch (boost::system::system_error error)
         {
            // We failed to send the data to the client. Log an message in the log, 
            // and switch of operation-in-progress flag. We don't log this as an 
            // error since it most likely isn't.
            String sMessage;
            sMessage.Format(_T("TCPConnection - Receiving of data from client failed. Error code: %d, Message: %s, Remote IP: %s"), error.code().value(), String(error.what()), SafeGetIPAddress());
            LOG_TCPIP(sMessage);
         }
         catch (std::exception const& e)
         {
            String sErrorMessage = Formatter::Format("Start Async Read operation failed. Error: {0}", e.what());
            ErrorManager::Instance()->ReportError(ErrorManager::High, 4208, "TCPConnection::AsyncRead", sErrorMessage);
         }
         catch (...)
         {
            String message = Formatter::Format("Read operation failed. Stage: {0}", stage);
            ErrorManager::Instance()->ReportError(ErrorManager::High, 5000, "TCPConnection::AsyncRead", message);
            throw;
         }
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5332, "TCPConnection::AsyncRead", "An unknown error occurred while starting async reading.");
         throw;
      }
   }

   void 
   TCPConnection::AsyncReadCompleted(const boost::system::error_code& error,  size_t bytes_transferred)
   {
      BOOST_SCOPE_EXIT(&operation_queue_, this_) {
         operation_queue_.Pop(IOOperation::BCTRead);
         this_->ProcessOperationQueue_();
      } BOOST_SCOPE_EXIT_END

      try
      {
         // Remove the item we have just handled.

         try
         {
            if (error.value() != 0)
            {
               OnReadError(error.value());

               String message;
               message.Format(_T("The read operation failed. Bytes transferred: %d"), bytes_transferred);
               ReportDebugMessage(message, error);

               if (error.value() == boost::asio::error::not_found)
               {
                  // read buffer is full...
                  OnExcessiveDataReceived();
               }

               EnqueueDisconnect();

               

               return;
            }
         }
         catch (...)
         {
            ReportError(ErrorManager::Medium, 5141, "TCPConnection::AsyncReadCompleted", "An error occurred while checking error state");
            throw;
         }

         // Disable the logout timer while we're parsing data. We don't want to terminate
         // a client just because he has issued a long-running command. If we do this, we
         // would have to take care of the fact that we're dropping a connection despite it
         // still being active.
         try
         {
            CancelLogoutTimer();
         }
         catch (...)
         {
            ReportError(ErrorManager::Medium, 5141, "TCPConnection::AsyncReadCompleted", "An error occurred while cancelling logout timer.");
            throw;
         }

         if (receive_binary_)
         {
            try
            {
               shared_ptr<ByteBuffer> pBuffer = shared_ptr<ByteBuffer>(new ByteBuffer());
               pBuffer->Allocate(receive_buffer_.size());

               std::istream is(&receive_buffer_);
               is.read((char*) pBuffer->GetBuffer(), receive_buffer_.size());

               try
               {
                  ParseData(pBuffer);
               }
               catch (boost::system::system_error error)
               {
                  ReportError(ErrorManager::Medium, 5136, "TCPConnection::AsyncReadCompleted", "An error occured while parsing buffer.", error);
                  throw;
               }
               catch (...)
               {
                  String message;
                  message.Format(_T("An error occured while parsing buffer. Received bytes: %d, Buffer: %d, Buffer size: %d"), bytes_transferred, &pBuffer, pBuffer->GetSize());

                  ReportError(ErrorManager::Medium, 5136, "TCPConnection::AsyncReadCompleted", "An error occured while parsing buffer.");
                  throw;
               }
            }
            catch (...)
            {
               ReportError(ErrorManager::Medium, 5136, "TCPConnection::AsyncReadCompleted", "An error occured while parsing binary data.");
               throw;
            }
         }
         else
         {
            std::string s;
            std::istream is(&receive_buffer_);
            std::getline(is, s, '\r');

            // consume trailing \n on line.
            receive_buffer_.consume(1);

      #ifdef _DEBUG
            String sDebugOutput;
            sDebugOutput.Format(_T("RECEIVED: %s\r\n"), String(s));
            OutputDebugString(sDebugOutput);
      #endif

            try
            {
               ParseData(s);
            }
            catch (boost::system::system_error error)
            {
               String message;
               message.Format(_T("An error occured while parsing data. Bytes transferred: %d, ")
                               _T("Data length: %d, Data: %s."), 
                               bytes_transferred, 
                               s.size(), 
                               String(s));

               ReportError(ErrorManager::Medium, 5136, "TCPConnection::AsyncReadCompleted", message, error);
            }
            catch (...)
            {
               String message;
               message.Format(_T("An error occured while parsing data. Data length: %d, Data: %s."), s.size(), String(s));

               ReportError(ErrorManager::Medium, 5136, "TCPConnection::AsyncReadCompleted", message);
            }
         }
      }
      catch (...)
      {
         ReportError(ErrorManager::Medium, 5141, "TCPConnection::AsyncReadCompleted", "An error occurred while handling read operation.");
         throw;
      }
   }

   void 
   TCPConnection::EnqueueWrite(const AnsiString &sData)
   {
      try
      {
         AnsiString sTemp = sData;
         char *pBuf = sTemp.GetBuffer();

         shared_ptr<ByteBuffer> pBuffer = shared_ptr<ByteBuffer>(new ByteBuffer());
         pBuffer->Add((BYTE*) pBuf, sData.GetLength());

#ifdef _DEBUG
         String sDebugOutput;
         sDebugOutput.Format(_T("SENT: %s"), String(sTemp));
         OutputDebugString(sDebugOutput);
#endif

         EnqueueWrite(pBuffer);
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5327, "TCPConnection::EnqueueWrite", "An unknown error occurred while posting write buffer.");
         throw;
      }

   }

   void 
   TCPConnection::EnqueueWrite(shared_ptr<ByteBuffer> pBuffer)
   {
      try
      {
         shared_ptr<IOOperation> operation = shared_ptr<IOOperation>(new IOOperation(IOOperation::BCTWrite, pBuffer));

         operation_queue_.Push(operation);
         ProcessOperationQueue_();
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5328, "TCPConnection::EnqueueWrite", "An unknown error occurred while posting write buffer.");
         throw;
      }
   }

   void 
   TCPConnection::AsyncWrite(shared_ptr<ByteBuffer> buffer)
   {
      try
      {
         function<void (const boost::system::error_code&, size_t)> AsyncWriteCompletedFunction =
            boost::bind(&TCPConnection::AsyncWriteCompleted, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred);

         try
         {
            if (is_ssl_)
               boost::asio::async_write
               (ssl_socket_, boost::asio::buffer(buffer->GetCharBuffer(), buffer->GetSize()), AsyncWriteCompletedFunction);
            else
               boost::asio::async_write
               (socket_, boost::asio::buffer(buffer->GetCharBuffer(), buffer->GetSize()), AsyncWriteCompletedFunction);

            UpdateLogoutTimer();
         }
         catch (boost::system::system_error error)
         {
            // We failed to send the data to the client. Log an message in the log, 
            // and switch of operation-in-progress flag. We don't log this as an 
            // error since it most likely isn't.
            String sMessage;
            sMessage.Format(_T("TCPConnection - Sending of data to client failed. Error code: %d, Message: %s, Remote IP: %s"), error.code().value(), String(error.what()), SafeGetIPAddress());
            LOG_TCPIP(sMessage);
         }
         catch (std::exception const& e)
         {
            String sErrorMessage = Formatter::Format("Write operation failed. Error: {0}", e.what());
            ErrorManager::Instance()->ReportError(ErrorManager::High, 4208, "TCPConnection::AsyncWrite", sErrorMessage);
         }
         catch (...)
         {
            ErrorManager::Instance()->ReportError(ErrorManager::High, 5000, "TCPConnection::AsyncWrite", "Write operation failed.");
            throw;
         }
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5329, "TCPConnection::AsyncWrite", "An unknown error occurred while writing buffer.");
         throw;
      }

   }

   void 
   TCPConnection::AsyncWriteCompleted(const boost::system::error_code& error,  size_t bytes_transferred)
   {
      BOOST_SCOPE_EXIT(&operation_queue_, this_) {
         operation_queue_.Pop(IOOperation::BCTWrite);
         this_->ProcessOperationQueue_();
      } BOOST_SCOPE_EXIT_END

      try
      {
         if (error.value() != 0)
         {
            String message;
            message.Format(_T("The read operation failed. Bytes transferred: %d"), bytes_transferred);
            ReportDebugMessage(message, error);

            EnqueueDisconnect();
         }

         bool containsQueuedSendOperations = false;

         try
         {
            containsQueuedSendOperations = operation_queue_.ContainsQueuedSendOperation();
         }
         catch (...)
         {
            ErrorManager::Instance()->ReportError(ErrorManager::High, 5339, "TCPConnection::AsyncWriteCompleted", "An unknown error occurred while handling buffer write and checking if queue contained buffered operations.");
            throw;
         }

         if (!containsQueuedSendOperations)
         {
            try
            {
               OnDataSent();
            }
            catch (...)
            {
               ErrorManager::Instance()->ReportError(ErrorManager::High, 5339, "TCPConnection::AsyncWriteCompleted", "An unknown error occurred while handling buffer write and notifying protocol parser that data was sent.");
               throw;
            }

         }
      }
      catch (...)
      {
         String message;
         message.Format(_T("An unknown error occurred while handling buffer write. Session ID: %d, Transferred bytes: %d"),  GetSessionID(), bytes_transferred);

         ErrorManager::Instance()->ReportError(ErrorManager::High, 5339, "TCPConnection::AsyncWriteCompleted", message);
         throw;
      }
   }

   IPAddress 
   TCPConnection::GetRemoteEndpointAddress()
   {
      boost::asio::ip::tcp::endpoint remoteEndpoint = socket_.remote_endpoint();
      return IPAddress(remoteEndpoint.address());
   }

   unsigned long 
   TCPConnection::GetLocalEndpointPort() 
   {
      boost::asio::ip::tcp::endpoint localEndpoint = socket_.local_endpoint();

      return localEndpoint.port();

   }

   void
   TCPConnection::SetReceiveBinary(bool binary)
   {
      receive_binary_ = binary;
   }

   String 
   TCPConnection::SafeGetIPAddress()
   {
      try
      {
         IPAddress address = socket_.remote_endpoint().address();

         return address.ToString();
      }
      catch (...)
      {
         return "";
      }
   }

   bool
   TCPConnection::IsClient()
   {
      return is_client_;
   }

   void  
   TCPConnection::SetSecurityRange(shared_ptr<SecurityRange> securityRange)
   {
      security_range_ = securityRange;
   }

   shared_ptr<SecurityRange>
   TCPConnection::GetSecurityRange()
   {
      if (!security_range_)
      {
         IPAddress address(socket_.remote_endpoint().address());

         security_range_ = PersistentSecurityRange::ReadMatchingIP(address);
      }

      return security_range_;
   }

   int
   TCPConnection::GetSessionID()
   {
      try
      {
         int sessionID = session_id_;
         return sessionID;
      }
      catch (...)
      {
         return 0;
      }
   }


   AnsiString 
   TCPConnection::GetIPAddressString()
   {
      return GetRemoteEndpointAddress().ToString();
   }


   void 
   TCPConnection::SetTimeout(int seconds)
   {
      timeout_ = seconds;
   }


   void 
   TCPConnection::UpdateLogoutTimer()
   {
      try
      {
         // Put a timeout...
         timer_.expires_from_now(boost::posix_time::seconds(timeout_));
         timer_.async_wait(bind(&TCPConnection::OnTimeout, 
            boost::weak_ptr<TCPConnection>(shared_from_this()), _1));
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5333, "TCPConnection::UpdateLogoutTimer", "An unknown error occurred while updating logout timer.");
         throw;
      }
   }

   void 
   TCPConnection::CancelLogoutTimer()
   {
      try
      {
         timer_.cancel();
      }
      catch (boost::system::system_error error)
      {
         ReportError(ErrorManager::Low, 5211, "TCPConnection::CancelLogoutTimer", "Failed to logout timer", error);
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5334, "TCPConnection::CancelLogoutTimer", "An unknown error occurred while canceling logout timer.");
         throw;
      }

   }


   void
   TCPConnection::OnTimeout(boost::weak_ptr<TCPConnection> connection, boost::system::error_code const& err)
   {
      try
      {
         boost::shared_ptr<TCPConnection> conn = connection.lock();
         if (!conn)
         {
            return;
         }

         if (err == asio::error::operation_aborted) 
         {
            // the timeout operation was cancelled.
            return;
         }

         String message;
         message.Format(_T("The client has timed out. Session: %d"), conn->GetSessionID());
         LOG_DEBUG(message);

         conn->Timeout();
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5337, "TCPConnection::OnTimeout", "An unknown error occurred while handling timeout.");
         throw;
      }
   }

   void 
   TCPConnection::Timeout()
   {
      try
      {
         if (has_timeout_)
         {
            // We've already posted a timeout once. Disconnect now.
            Disconnect();
            return;
         }

         has_timeout_ = true;

         OnConnectionTimeout();

         EnqueueDisconnect();
      }
      catch (boost::system::system_error error)
      {
         ReportError(ErrorManager::Low, 5137, "TCPConnection::PostTimeout", "An error occured while sending a timeout message to the client.", error);
      }
      catch (...)
      {
         ReportError(ErrorManager::Low, 5137, "TCPConnection::PostTimeout", "An error occured while sending a timeout message to the client.");
      }
   }

   void 
   TCPConnection::ReportError(ErrorManager::eSeverity sev, int code, const String &context, const String &message, const boost::system::system_error &error)
   {
      String formattedMessage;
      formattedMessage.Format(_T("%s Remote IP: %s, Error code: %d, Message: %s"), message, SafeGetIPAddress(), error.code().value(), String(error.what()));
      ErrorManager::Instance()->ReportError(sev, code, context, formattedMessage);         
   }

   void 
   TCPConnection::ReportError(ErrorManager::eSeverity sev, int code, const String &context, const String &message)
   {
      String formattedMessage;
      formattedMessage.Format(_T("%s Remote IP: %s"), message, SafeGetIPAddress());
      ErrorManager::Instance()->ReportError(sev, code, context, formattedMessage);         
   }
   void 
   TCPConnection::ReportDebugMessage(const String &message, const boost::system::error_code &error)
   {
      String formattedMessage;
      formattedMessage.Format(_T("%s Remote IP: %s, Session: %d, Code: %d, Message: %s"), message, SafeGetIPAddress(), GetSessionID(), error.value(), String(error.message()));
      LOG_DEBUG(formattedMessage);
   }

}
