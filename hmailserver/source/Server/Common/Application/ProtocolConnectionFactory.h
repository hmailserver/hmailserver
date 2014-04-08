// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class SocketConnection;

   class ProtocolConnectionFactory
   {
   public:
      virtual SocketConnection* CreateConnection() = 0;
   };

   class SMTPConnectionFactory : public ProtocolConnectionFactory
   {
   public :
      virtual SocketConnection* CreateConnection();
   };

   class POP3ConnectionFactory : public ProtocolConnectionFactory
   {
   public :
      virtual SocketConnection* CreateConnection();
   };

   class IMAPConnectionFactory : public ProtocolConnectionFactory
   {
   public :
      virtual SocketConnection* CreateConnection();
   };

}