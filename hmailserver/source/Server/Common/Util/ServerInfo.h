// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../TCPIP/SocketConstants.h"

namespace HM
{
   class ServerInfo
   {
   public:
	   ServerInfo(bool fixed, const String &host_name, const String &ip_address, int port, const String&userName, const String &passWord, ConnectionSecurity connection_security);
	   virtual ~ServerInfo();

      bool GetFixed();
      String GetHostName();
      String GetIpAddress();
      int GetPort ();
      String GetUsername();
      String GetPassword();
      ConnectionSecurity GetConnectionSecurity();
      
      void SetHostName(const String &hostName);
      void SetIpAddress(const String &ip_address);

      void DisableConnectionSecurity();
         
      bool operator== (const ServerInfo &other) const;

   private:

      bool fixed_;
      String host_name_;
      String ip_address_;
      int port_;
      String userName_;
      String passWord_;
      ConnectionSecurity connection_security_;


   };
}
