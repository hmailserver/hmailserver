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
   std::shared_ptr<TCPConnection>
   SMTPConnectionFactory::Create(ConnectionSecurity connection_security,
                                    boost::asio::io_service& io_service, 
                                    boost::asio::ssl::context& context)
   {
      SMTPConnection* smtpConn = new SMTPConnection(connection_security, io_service, context);

      return std::shared_ptr<TCPConnection>(smtpConn);
   }

   std::shared_ptr<TCPConnection>
   POP3ConnectionFactory::Create(ConnectionSecurity connection_security,
      boost::asio::io_service& io_service, 
      boost::asio::ssl::context& context)
   {
      POP3Connection* smtpConn = new POP3Connection(connection_security, io_service, context);

      return std::shared_ptr<TCPConnection>(smtpConn);
   }

   std::shared_ptr<TCPConnection>
   IMAPConnectionFactory::Create(ConnectionSecurity connection_security,
      boost::asio::io_service& io_service, 
      boost::asio::ssl::context& context)
   {
      IMAPConnection* smtpConn = new IMAPConnection(connection_security, io_service, context);

      return std::shared_ptr<TCPConnection>(smtpConn);
   }
}