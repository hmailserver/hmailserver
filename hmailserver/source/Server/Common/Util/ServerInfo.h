// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../TCPIP/SocketConstants.h"

namespace HM
{
   class ServerInfo
   {
   public:
	   ServerInfo(bool fixed, const String &hostName, int port, const String&userName, const String &passWord, ConnectionSecurity connection_security);
	   virtual ~ServerInfo();

      bool GetFixed();
      String GetHostName();
      int GetPort ();
      String GetUsername();
      String GetPassword();
      ConnectionSecurity GetConnectionSecurity();
      void SetHostName(const String &hostName);


         
      bool operator== (const ServerInfo &other) const;

   private:

      bool fixed_;
      String hostName_;
      int port_;
      String userName_;
      String passWord_;
      ConnectionSecurity connection_security_;


   };
}
