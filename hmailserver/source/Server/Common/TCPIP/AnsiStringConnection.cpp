// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "AnsiStringConnection.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
  

   AnsiStringConnection::AnsiStringConnection(bool useSSL,
                                boost::asio::io_service& io_service, 
                                boost::asio::ssl::context& context) :
      TCPConnection(useSSL, io_service, context)
   {

   }

   AnsiStringConnection::~AnsiStringConnection(void)
   {
     
   }

   void
   AnsiStringConnection::PostReceive()
   {
      PostRead(GetCommandSeparator());
   }

   bool
   AnsiStringConnection::SendData(const AnsiString &sAnsiString)
   {
      PostWrite(sAnsiString);
      return true;
   }

   bool
   AnsiStringConnection::SendData(shared_ptr<ByteBuffer> pByteBuffer)
   {
      PostWrite(pByteBuffer);
      return true;
   }

   void
   AnsiStringConnection::PostBufferReceive()
   {
      PostRead("");
   }
}
