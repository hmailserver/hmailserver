// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "TCPConnectionFactory.h"

#include "../../SMTP/SMTPConnection.h"
#include "../../POP3/POP3Connection.h"
#include "../../IMAP/IMAPConnection.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   shared_ptr<TCPConnection>
   SMTPConnectionFactory::Create(bool useSSL,
                                    boost::asio::io_service& io_service, 
                                    boost::asio::ssl::context& context)
   {
      SMTPConnection* smtpConn = new SMTPConnection(useSSL, io_service, context);

      return shared_ptr<TCPConnection>(smtpConn);
   }

   shared_ptr<TCPConnection>
   POP3ConnectionFactory::Create(bool useSSL,
      boost::asio::io_service& io_service, 
      boost::asio::ssl::context& context)
   {
      POP3Connection* smtpConn = new POP3Connection(useSSL, io_service, context);

      return shared_ptr<TCPConnection>(smtpConn);
   }

   shared_ptr<TCPConnection>
   IMAPConnectionFactory::Create(bool useSSL,
      boost::asio::io_service& io_service, 
      boost::asio::ssl::context& context)
   {
      IMAPConnection* smtpConn = new IMAPConnection(useSSL, io_service, context);

      return shared_ptr<TCPConnection>(smtpConn);
   }
}