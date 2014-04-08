// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

using boost::asio::ip::tcp;

namespace HM
{
   class SynchronousConnection
   {
   public:
	   SynchronousConnection(int timeoutSeconds);
	   virtual ~SynchronousConnection();

      bool Connect(const AnsiString &hostName, int port);
      bool Write(const AnsiString &data);
      bool Write(const ByteBuffer &buffer);
      bool ReadUntil(const AnsiString &delimiter, AnsiString &readData);
      void Close();

   private:
      
      bool _Write(const unsigned char *buf, int bufSize);

      boost::asio::io_service m_ioservice;
      tcp::socket m_socket;
      int m_timeoutSeconds;
   };


}
