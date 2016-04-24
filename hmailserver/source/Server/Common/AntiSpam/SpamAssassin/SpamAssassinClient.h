// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../../TCPIP/TCPConnection.h"

namespace HM
{
   class File;

   class SpamAssassinClient : public TCPConnection
   {
   public:
      SpamAssassinClient(const String &sFile, 
         boost::asio::io_service& io_service, 
         boost::asio::ssl::context& context,
         std::shared_ptr<Event> disconnected,
         bool &testCompleted);
      ~SpamAssassinClient(void);

      virtual void ParseData(const AnsiString &Request);
      virtual void ParseData(std::shared_ptr<ByteBuffer> pBuf);

      

      
   protected:

      virtual void OnCouldNotConnect(const AnsiString &sErrorDescription);
      virtual void OnReadError(int errorCode);
      virtual void OnConnected();
      virtual void OnHandshakeCompleted() {};
      virtual void OnHandshakeFailed() {};
      virtual AnsiString GetCommandSeparator() const;
      virtual void OnConnectionTimeout();
      virtual void OnExcessiveDataReceived();

   private:

      void Cleanup_();
      void FinishTesting_();
      int ParseFirstBuffer_(std::shared_ptr<ByteBuffer> pBuffer) const;
      bool SendFileContents_(const String &sFilename);

      String command_buffer_;

      String message_file_;
	   size_t spam_dsize_;
	   int message_size_;
      std::shared_ptr<File> result_;
      bool &test_completed_;

      size_t total_result_bytes_written_;
  };
}