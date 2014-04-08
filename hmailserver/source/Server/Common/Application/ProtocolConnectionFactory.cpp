#include "StdAfx.h"
#include "ProtocolConnectionFactory.h"

#include "../../SMTP/SMTPConnection.h"
#include "../../POP3/POP3Connection.h"
#include "../../IMAP/IMAPConnection.h"

namespace HM
{
   SocketConnection*
   SMTPConnectionFactory::CreateConnection()
   {
      return new SMTPConnection();
   }

   SocketConnection*
   POP3ConnectionFactory::CreateConnection()
   {
      return new POP3Connection();
   }

   SocketConnection*
   IMAPConnectionFactory::CreateConnection()
   {
      return new IMAPConnection();
   }

}
