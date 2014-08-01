// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "ServerInfo.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   ServerInfo::ServerInfo(bool fixed, const String &host_name, const String &ip_address, int port, const String&userName, const String &passWord, ConnectionSecurity connection_security) :
      fixed_(fixed),
      host_name_(host_name),
      ip_address_(ip_address),
      port_(port),
      userName_(userName),
      passWord_(passWord),
      connection_security_(connection_security)
   {

   }

   ServerInfo::~ServerInfo()
   {

   }

   bool
   ServerInfo::GetFixed()
   {
      return fixed_;
   }

   String 
   ServerInfo::GetHostName()
   {
      return host_name_;
   }

   void 
   ServerInfo::SetHostName(const String &host_name)
   {
      host_name_ = host_name;
   }

   String 
   ServerInfo::GetIpAddress()
   {
      return ip_address_;
   }

   void 
      ServerInfo::SetIpAddress(const String &ip_address)
   {
      ip_address_ = ip_address;
   }

   int 
   ServerInfo::GetPort()
   {
      return port_;
   }

   String 
   ServerInfo::GetUsername()
   {
      return userName_;
   }

   String 
   ServerInfo::GetPassword()
   {
      return passWord_;
   }

   ConnectionSecurity 
   ServerInfo::GetConnectionSecurity()
   {
      return connection_security_;
   }

   void 
   ServerInfo::DisableConnectionSecurity()
   {
      connection_security_ = CSNone;
   }

   bool 
   ServerInfo::operator== (const ServerInfo &other) const
   {
      return fixed_ == other.fixed_ &&
         host_name_ == other.host_name_ &&
         ip_address_ == other.ip_address_ &&
         port_ == other.port_ &&
         userName_ == other.userName_ &&
         passWord_ == other.passWord_ &&
         connection_security_ == other.connection_security_;
   }
}
