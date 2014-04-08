// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "IMAPCommand.h"
#include "../Common/Util/ByteBuffer.h"

namespace HM
{
   class ByteBuffer;
   class IMAPFolder;
   class Domain;

   class IMAPCommandAppend : public IMAPCommand  
   {
   public:
	   IMAPCommandAppend();
	   virtual ~IMAPCommandAppend();

      virtual IMAPResult ExecuteCommand(shared_ptr<IMAPConnection> pConnection, shared_ptr<IMAPCommandArgument> pArgument);
      void ParseBinary(shared_ptr<IMAPConnection> pConnection, shared_ptr<ByteBuffer> pBuf);
      

   private:

      void _Finish(shared_ptr<IMAPConnection> pConnection);
      bool _TruncateBuffer(const shared_ptr<IMAPConnection> pConnection );
      bool _WriteData(const shared_ptr<IMAPConnection> pConnection, const BYTE *pBuf, int WriteLen);
      void _KillCurrentMessage();
      
      int _GetMaxMessageSize(shared_ptr<const Domain> pDomain);

      String m_sCurrentTag;
      String m_sFlagsToSet;
      String m_sCreateTimeToSet;
      long m_lBytesLeftToReceive;

      String m_sMessageFileName;

      ByteBuffer _appendBuffer;
      shared_ptr<IMAPFolder> m_pDestinationFolder;
      shared_ptr<Message> m_pCurrentMessage;

      
   };

}
