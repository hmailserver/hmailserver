// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../Util/Event.h"

#include "SocketConstants.h"
#include "IOOperationQueue.h"


#include <boost/atomic.hpp>

using boost::asio::ip::tcp;

typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> ssl_socket;

namespace HM
{
   class ByteBuffer;
   class SecurityRange;
   class CipherInfo;

   class TCPConnection :
      public std::enable_shared_from_this<TCPConnection>
   {
   public:
      TCPConnection(ConnectionSecurity connection_security,
                    boost::asio::io_service& io_service,    
                    boost::asio::ssl::context& context,
                    std::shared_ptr<Event> disconnected,
                    AnsiString expected_remote_hostname);
      ~TCPConnection(void);

      enum Consts
      {
         BufferSize = 60000
      };

      bool Connect(const AnsiString &remote_ip_address, long remotePort, const IPAddress &localAddress);
      
      void Start();
      void SetReceiveBinary(bool binary);

      void EnqueueWrite(const AnsiString &sData);
      void EnqueueWrite(std::shared_ptr<ByteBuffer> pByteBuffer);
      void EnqueueRead();
      void EnqueueRead(const AnsiString &delimitor);
      void EnqueueShutdownSend();
      void EnqueueDisconnect();
      void EnqueueHandshake();
      
      IPAddress GetRemoteEndpointAddress();
      unsigned long GetLocalEndpointPort();

      void UpdateAutoLogoutTimer();

      void SetSecurityRange(std::shared_ptr<SecurityRange> securityRange);
      std::shared_ptr<SecurityRange> GetSecurityRange();

      boost::asio::ip::tcp::socket& GetSocket() {return socket_;}

      ConnectionSecurity GetConnectionSecurity() {return connection_security_; }

      bool IsSSLConnection(){return is_ssl_;}

      void Timeout();

      CipherInfo GetCipherInfo();

      void SetAllowConnectToSelf(bool allow)  { allow_connect_to_self_ = allow; }

      int GetSessionID();

   protected:

      ConnectionState GetConnectionState() { return connection_state_;  }

      int GetBufferSize() {return BufferSize; }

      void SetTimeout(int seconds);
      AnsiString GetIPAddressString();

      virtual void OnCouldNotConnect(const AnsiString &sErrorDescription) {};
      virtual void OnConnected() = 0;
      virtual void OnHandshakeCompleted() = 0;
      virtual void OnHandshakeFailed() = 0;
      virtual void OnConnectionTimeout() = 0;
      virtual void OnExcessiveDataReceived() = 0;
      virtual void OnDataSent() {};
      virtual void OnReadError(int errorCode) {};
      virtual AnsiString GetCommandSeparator() const = 0;

      /* PARSING METHODS */
      virtual void ParseData(const AnsiString &sAnsiString) = 0;
      virtual void ParseData(std::shared_ptr<ByteBuffer> pByteBuffer) = 0;
   
      AnsiString GetSslTlsCipher();

   private:

      void ThrowIfNotConnected_();
      void HandshakeFailed_(const boost::system::error_code& error);
      void StartAsyncConnect_(const String &ip_adress, int port);

      static void OnTimeout(std::weak_ptr<TCPConnection> connection, boost::system::error_code const& err);

      String SafeGetIPAddress();

      bool IsClient();

      void ProcessOperationQueue_(int recurse_level);

      void Disconnect();
      void Shutdown(boost::asio::socket_base::shutdown_type);
      
      void AsyncWrite(std::shared_ptr<ByteBuffer> buffer);
      void AsyncRead(const AnsiString &delimitor);
      void AsyncHandshake();

      void AsyncConnectCompleted(const boost::system::error_code& err);
      void AsyncHandshakeCompleted(const boost::system::error_code& error);
      void AsyncReadCompleted(const boost::system::error_code& /*error*/, size_t bytes_transferred);
      void AsyncWriteCompleted(const boost::system::error_code& /*error*/, size_t bytes_transferred);

      void ReportDebugMessage(const String &message, const boost::system::error_code &error);
      void ReportError(ErrorManager::eSeverity sev, int code, const String &context, const String &message, const boost::system::system_error &error);
      void ReportError(ErrorManager::eSeverity sev, int code, const String &context, const String &message, const boost::system::error_code &error);
      void ReportError(ErrorManager::eSeverity sev, int code, const String &context, const String &message);

      boost::asio::ip::tcp::socket socket_;
      ssl_socket ssl_socket_;

      boost::asio::ip::tcp::resolver resolver_;
      boost::asio::deadline_timer timer_;
      boost::asio::streambuf receive_buffer_;
      boost::asio::ssl::context& context_;

      IOOperationQueue operation_queue_;

      bool receive_binary_;
      ConnectionSecurity connection_security_;
      long remote_port_;
      String remote_ip_address_;

      std::shared_ptr<SecurityRange> security_range_;

      int session_id_;
      int timeout_;

      AnsiString expected_remote_hostname_;
      std::shared_ptr<Event> disconnected_;
      bool is_ssl_;
      bool is_client_;
      bool handshake_in_progress_;
      bool allow_connect_to_self_;

      boost::atomic<ConnectionState> connection_state_;
      boost::mutex autologout_timer_;

   };

}