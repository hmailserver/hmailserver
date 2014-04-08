// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../../TCPIP/ProtocolParser.h"

namespace HM
{
   class File;

   class SpamAssassinClient : public ProtocolParser
   {
   public:
      SpamAssassinClient(const String &sFile);
      ~SpamAssassinClient(void);

      virtual void ParseData(const AnsiString &Request);
      virtual void ParseData(shared_ptr<ByteBuffer> pBuf);

      bool FinishTesting();

      
   protected:

      virtual void OnCouldNotConnect(const AnsiString &sErrorDescription);
      virtual void OnReadError(int errorCode);
      virtual void OnConnected();
      virtual AnsiString GetCommandSeparator() const;
      virtual void OnConnectionTimeout();
      virtual void OnExcessiveDataReceived();

   private:

      int _ParseFirstBuffer(shared_ptr<ByteBuffer> pBuffer) const;
      bool _SendFileContents(const String &sFilename);

      String m_sCommandBuffer;

      String m_sMessageFile;
	  int m_iSpamDSize;
	  int m_iMessageSize;
      shared_ptr<File> m_pResult;
  };
}