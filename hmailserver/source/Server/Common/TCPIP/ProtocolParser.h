// Copyright (c) 2008 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class ByteBuffer;
   class TCPConnection;
   class SecurityRange;

   class ProtocolParser
   {
   public:
      ProtocolParser();
      ~ProtocolParser();

      void Initialize(TCPConnection* parentConnection);
      
      virtual AnsiString GetCommandSeparator() const = 0;
      
      shared_ptr<TCPConnection> GetTCPConnectionTemporaryPointer();

      // The connection to the server has failed
      virtual void OnCouldNotConnect(const AnsiString &sErrorDescription) {};
      virtual void OnConnected() = 0;
      virtual void OnConnectionTimeout() = 0;
      virtual void OnExcessiveDataReceived() = 0;
      virtual void OnDataSent() {};
      virtual void OnReadError(int errorCode) {};

      int GetTimeout();

      /* PARSING METHODS */
      virtual void ParseData(const AnsiString &sAnsiString) = 0;
      virtual void ParseData(shared_ptr<ByteBuffer> pByteBuffer) = 0;

      bool SendData(const AnsiString &sAnsiString);
      bool SendData(shared_ptr<ByteBuffer> pByteBuffer);

      void PostReceive();
      void PostBufferReceive();

      void PostShutdown(int what);

      void UpdateLogoutTimer();
      int GetBufferSize();

      int GetSessionID() const;

   protected:

      void PostDisconnect();
      void SetTimeout(int seconds);
      

      /* UTILITY FUNCTIONS */
      
      void SetReceiveBinary(bool binary);
      
      const IPAddress GetIPAddress() const;
      unsigned long GetPort() const;
      unsigned long GetLocalPort() const;
      AnsiString GetIPAddressString() const;

      shared_ptr<SecurityRange> GetSecurityRange() const;
   private:

      TCPConnection *_parentConnection;
      int _sessionID;
      int _timeout;
   };
}
