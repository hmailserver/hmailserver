// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../Util/Event.h"

#include "IOOperationQueue.h"
using boost::asio::ip::tcp;

typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket;

namespace HM
{
   class ProtocolParser;
   class ByteBuffer;
   class SecurityRange;

   class TCPConnection :
      public boost::enable_shared_from_this<TCPConnection>
   {
   public:
      TCPConnection(bool useSSL,
                    boost::asio::io_service& io_service,    
                    boost::asio::ssl::context& context);
      ~TCPConnection(void);

      enum ShutdownOption
      {
         ShutdownSend,
         ShutdownReceive,
      };

      enum Consts
      {
         BufferSize = 60000
      };

      ssl_socket::lowest_layer_type& GetSocket();

      int GetBufferSize() {return BufferSize; }
      bool Connect(const AnsiString &remoteServer, long remotePort, const IPAddress &localAddress);
      void Start(shared_ptr<ProtocolParser> protocolParser);
      void SetReceiveBinary(bool binary);

      void PostWrite(const AnsiString &sData);
      void PostWrite(shared_ptr<ByteBuffer> pByteBuffer);
      void PostRead(const AnsiString &delimitor);

      void PostShutdown(ShutdownOption what);
      void PostDisconnect();
      void PostTimeout();
      
      IPAddress GetRemoteEndpointAddress();
      unsigned long GetRemoteEndpointPort();
      unsigned long GetLocalEndpointPort();

      void UpdateLogoutTimer();
      void CancelLogoutTimer();

      static bool PrepareSSLContext(boost::asio::ssl::context &ctx);

      void SetSecurityRange(shared_ptr<SecurityRange> securityRange);
      shared_ptr<SecurityRange> GetSecurityRange();

      shared_ptr<TCPConnection> GetSharedFromThis();

      Event GetConnectionTerminationEvent() {return _connectionTermination;}

      int GetSessionID();

      bool ReportReadErrors(bool newValue);
   private:
      
      void _StartAsyncConnect(tcp::resolver::iterator endpoint_iterator);

      static void OnTimeout(boost::weak_ptr<TCPConnection> connection, boost::system::error_code const& err);

      String SafeGetIPAddress();

      bool IsClient();

      void _ProcessOperationQueue();

      void Disconnect();
      void Shutdown(boost::asio::socket_base::shutdown_type, bool removeFromQueue);
      void Write(shared_ptr<ByteBuffer> buffer);
      void Read(const AnsiString &delimitor);

      void HandleResolve(const boost::system::error_code& err, tcp::resolver::iterator endpoint_iterator);
      void HandleConnect(const boost::system::error_code& err, tcp::resolver::iterator endpoint_iterator);
      void HandleHandshake(const boost::system::error_code& error);
      void HandleRead(const boost::system::error_code& /*error*/,  size_t bytes_transferred);
      void HandleWrite(const boost::system::error_code& /*error*/,  size_t bytes_transferred);

      void ReportDebugMessage(const String &message, const boost::system::error_code &error);
      void ReportError(ErrorManager::eSeverity sev, int code, const String &context, const String &message, const boost::system::system_error &error);
      void ReportError(ErrorManager::eSeverity sev, int code, const String &context, const String &message);
      static void ReportInitError(ErrorManager::eSeverity sev, int code, const String &context, const String &message, const boost::system::system_error &error);

      boost::asio::ip::tcp::socket _socket;
      ssl_socket _sslSocket;

      boost::asio::ip::tcp::resolver _resolver;
      boost::asio::deadline_timer _timer;
      boost::asio::streambuf _receiveBuffer;
      shared_ptr<ProtocolParser> _protocolParser;
      
      IOOperationQueue _operationQueue;

      bool _receiveBinary;
      bool _useSSL;
      long _remotePort;
      bool _hasTimeout;
      String _remoteServer;
      Event _connectionTermination;

      shared_ptr<SecurityRange> _securityRange;

   };

}