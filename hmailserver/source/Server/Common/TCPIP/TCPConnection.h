// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../Util/Event.h"

#include "SocketConstants.h"
#include "IOOperationQueue.h"
using boost::asio::ip::tcp;

typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket;

namespace HM
{
   class ByteBuffer;
   class SecurityRange;

   class TCPConnection :
      public boost::enable_shared_from_this<TCPConnection>
   {
   public:
      TCPConnection(ConnectionSecurity connection_security,
                    boost::asio::io_service& io_service,    
                    boost::asio::ssl::context& context,
                    shared_ptr<Event> disconnected);
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
      void Start();
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

      void SetSecurityRange(shared_ptr<SecurityRange> securityRange);
      shared_ptr<SecurityRange> GetSecurityRange();

      int GetSessionID();

      bool ReportReadErrors(bool newValue);

   protected:

      void SetTimeout(int seconds);
      AnsiString GetIPAddressString();

      virtual void OnCouldNotConnect(const AnsiString &sErrorDescription) {};
      virtual void OnConnected() = 0;
      virtual void OnHandshakeCompleted() = 0;
      virtual void OnConnectionTimeout() = 0;
      virtual void OnExcessiveDataReceived() = 0;
      virtual void OnDataSent() {};
      virtual void OnReadError(int errorCode) {};

      /* PARSING METHODS */
      virtual void ParseData(const AnsiString &sAnsiString) = 0;
      virtual void ParseData(shared_ptr<ByteBuffer> pByteBuffer) = 0;

      void Handshake();

      ConnectionSecurity GetConnectionSecurity() {return connection_security_; }
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
      

      ssl_socket _sslSocket;

      boost::asio::ip::tcp::resolver _resolver;
      boost::asio::deadline_timer _timer;
      boost::asio::streambuf _receiveBuffer;
      boost::asio::ssl::context& context_;

      IOOperationQueue _operationQueue;

      bool _receiveBinary;
      ConnectionSecurity connection_security_;
      long _remotePort;
      bool _hasTimeout;
      String _remoteServer;

      shared_ptr<SecurityRange> _securityRange;

      int _sessionID;
      int _timeout;

      shared_ptr<Event> disconnected_;
      bool is_ssl_;
   };

}