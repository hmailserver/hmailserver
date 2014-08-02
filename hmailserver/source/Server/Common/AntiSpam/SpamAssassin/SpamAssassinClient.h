// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../../TCPIP/AnsiStringConnection.h"

namespace HM
{
   class File;

   class SpamAssassinClient : public AnsiStringConnection
   {
   public:
      SpamAssassinClient(const String &sFile, 
         boost::asio::io_service& io_service, 
         boost::asio::ssl::context& context,
         shared_ptr<Event> disconnected,
         String &message,
         bool &testCompleted);
      ~SpamAssassinClient(void);

      virtual void ParseData(const AnsiString &Request);
      virtual void ParseData(shared_ptr<ByteBuffer> pBuf);

      bool FinishTesting();

      
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

      int ParseFirstBuffer_(shared_ptr<ByteBuffer> pBuffer) const;
      bool SendFileContents_(const String &sFilename);

      String m_sCommandBuffer;

      String m_sMessageFile;
	   int m_iSpamDSize;
	   int m_iMessageSize;
      shared_ptr<File> result_;

      String &m_sMessage;
      bool &m_TestCompleted;
  };
}